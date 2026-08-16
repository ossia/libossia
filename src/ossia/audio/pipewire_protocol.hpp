#pragma once
#include <ossia/detail/config.hpp>

#if defined(OSSIA_ENABLE_PIPEWIRE)
#if __has_include(<libremidi/backends/linux/pipewire/context.hpp>) \
    && __has_include(<pipewire/filter.h>)           \
    && __has_include(<spa/param/latency-utils.h>)
#define OSSIA_AUDIO_PIPEWIRE 1

#include <ossia/audio/audio_engine.hpp>
#include <ossia/audio/pipewire_quantum.hpp>
#include <ossia/detail/logger.hpp>
#include <ossia/detail/pod_vector.hpp>
#include <ossia/detail/thread.hpp>

#include <libremidi/backends/linux/pipewire/context.hpp>
#include <libremidi/backends/linux/pipewire/filter.hpp>
#include <libremidi/backends/linux/pipewire/loader.hpp>
#include <libremidi/backends/linux/pipewire/subscription.hpp>
#include <libremidi/backends/linux/pipewire/types.hpp>

#include <pipewire/filter.h>
#include <pipewire/keys.h>
#include <pipewire/properties.h>
#include <spa/param/latency-utils.h>
#include <spa/utils/result.h>

#include <fmt/format.h>

#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

namespace ossia
{

struct audio_setup
{
  std::string name;
  std::string card_in;
  std::string card_out;

  std::vector<std::string> inputs;
  std::vector<std::string> outputs;

  int rate{};
  int buffer_size{};
};

class pipewire_audio_protocol : public audio_engine
{
public:
  struct port
  {
  };

  std::shared_ptr<libremidi::pipewire::context> loop;
  pw_filter* filter{};
  std::vector<pw_proxy*> links;

  std::vector<port*> input_ports;
  std::vector<port*> output_ports;

  bool activated{};

  explicit pipewire_audio_protocol(
      std::shared_ptr<libremidi::pipewire::context> ctx,
      const audio_setup& setup)
      : loop{std::move(ctx)}
  {
    if (!loop || !loop->ok())
      return;

    auto& pw = libremidi::pipewire::load();
    if (!pw.filter_available)
      return;

    if (setup.buffer_size <= 0 || setup.rate <= 0)
      throw std::runtime_error("PipeWire: invalid buffer size or sample rate");

    // Everything the process callback reads must be in place before
    // filter_connect: cycles start arriving while this constructor is
    // still waiting on the sync loops below.
    this->effective_buffer_size = setup.buffer_size;
    this->effective_sample_rate = setup.rate;
    this->effective_inputs = setup.inputs.size();
    this->effective_outputs = setup.outputs.size();
    m_quantum.expected = setup.buffer_size;
    m_rate.expected = setup.rate;
    m_silence.assign(setup.buffer_size, 0.f);
    m_scratch.assign(setup.buffer_size, 0.f);
    m_cycle_in.resize(setup.inputs.size());
    m_cycle_out.resize(setup.outputs.size());
    m_chunk_in.resize(setup.inputs.size());
    m_chunk_out.resize(setup.outputs.size());
    m_buf_in.resize(setup.inputs.size());
    m_buf_out.resize(setup.outputs.size());

    // static: pw_filter_new_simple stores the pointer, not the table.
    static constexpr const struct pw_filter_events filter_events = {
        .version = PW_VERSION_FILTER_EVENTS,
        .destroy = {},
        .state_changed = {},
        .io_changed = {},
        .param_changed = {},
        .add_buffer = {},
        .remove_buffer = {},
        .process = &on_process,
        .drained = {},
#if PW_VERSION_CORE > 3
        .command = {},
#endif
    };

    std::string default_sink_name = loop->default_audio_sink_name();
    ossia::logger().info(
        "PipeWire filter: default sink name = '{}'", default_sink_name);

    bool created = false;
    loop->with_lock([&] {
      auto* filter_props = pw.properties_new(
          PW_KEY_MEDIA_TYPE, "Audio",
          PW_KEY_MEDIA_CATEGORY, "Duplex",
          PW_KEY_MEDIA_ROLE, "DSP",
          PW_KEY_MEDIA_NAME, setup.name.c_str(),
          PW_KEY_NODE_NAME, setup.name.c_str(),
          PW_KEY_NODE_GROUP, "group.dsp.0",
          PW_KEY_NODE_DESCRIPTION, "ossia score",
          // NODE_LATENCY + NODE_TRANSPORT_SYNC: wireplumber routing
          // hints; without them the filter stays unconnected.
          PW_KEY_NODE_LATENCY,
          fmt::format("{}/{}", setup.buffer_size, setup.rate).c_str(),
          PW_KEY_NODE_FORCE_QUANTUM,
          fmt::format("{}", setup.buffer_size).c_str(),
          PW_KEY_NODE_FORCE_RATE, fmt::format("{}", setup.rate).c_str(),
          // Note: node.lock-rate / node.lock-quantum would be inert here —
          // the driver cancels the lock whenever any follower forces the
          // value (pipewire context.c), and this node always forces both.
          // force-* is last-write-wins between clients and loses to the
          // global clock.force-* settings, so the process callback must
          // (and does) cope with any quantum or rate.
          PW_KEY_NODE_LOCK_RATE, "true",
          PW_KEY_NODE_TRANSPORT_SYNC, "true",
          PW_KEY_NODE_ALWAYS_PROCESS, "true",
          PW_KEY_NODE_PAUSE_ON_IDLE, "false",
          PW_KEY_NODE_SUSPEND_ON_IDLE, "false",
          nullptr);
      if (!filter_props)
        return;
      if (!default_sink_name.empty())
      {
        pw.properties_set(
            filter_props, PW_KEY_TARGET_OBJECT, default_sink_name.c_str());
      }

      this->filter = pw.filter_new_simple(
          loop->bare_loop(), setup.name.c_str(), filter_props,
          &filter_events, this);
      // filter_props ownership taken by pw_filter_new_simple.
      if (!this->filter)
        return;

      for (const auto& name : setup.inputs)
      {
        auto* p = static_cast<port*>(pw.filter_add_port(
            this->filter, PW_DIRECTION_INPUT,
            PW_FILTER_PORT_FLAG_MAP_BUFFERS, sizeof(struct port),
            pw.properties_new(
                PW_KEY_FORMAT_DSP, "32 bit float mono audio",
                PW_KEY_PORT_NAME, name.c_str(), nullptr),
            nullptr, 0));
        input_ports.push_back(p);
      }

      for (const auto& name : setup.outputs)
      {
        auto* p = static_cast<port*>(pw.filter_add_port(
            this->filter, PW_DIRECTION_OUTPUT,
            PW_FILTER_PORT_FLAG_MAP_BUFFERS, sizeof(struct port),
            pw.properties_new(
                PW_KEY_FORMAT_DSP, "32 bit float mono audio",
                PW_KEY_PORT_NAME, name.c_str(), nullptr),
            nullptr, 0));
        output_ports.push_back(p);
      }

      created = (pw.filter_connect(
                     this->filter, PW_FILTER_FLAG_RT_PROCESS, nullptr, 0)
                 >= 0);
    });

    if (!this->filter)
      throw std::runtime_error("PipeWire: could not create filter instance");
    if (!created)
    {
      // The destructor does not run when the constructor throws.
      loop->with_lock([&] {
        pw.filter_destroy(this->filter);
        this->filter = nullptr;
      });
      throw std::runtime_error("PipeWire: cannot connect");
    }

    if (!loop->synchronize())
    {
      ossia::logger().error(
          "PipeWire: synchronize() failed after filter_connect — engine inactive");
      return;
    }
    {
      int k = 0;
      auto node_id = filter_node_id();
      while (node_id == 0xFFFFFFFFu)
      {
        if (!loop->synchronize())
        {
          ossia::logger().error(
              "PipeWire: synchronize() failed while waiting for node id");
          return;
        }
        node_id = filter_node_id();
        if (k++ > 100)
          return;
      }

      // Registry broadcasts trail core_sync done by one round trip.
      const auto num_in = input_ports.size();
      const auto num_out = output_ports.size();
      bool have_ports = false;
      for (int j = 0; j < 200; ++j)
      {
        auto snap = loop->snapshot();
        if (const auto* self = snap.find_by_id(node_id))
        {
          if (self->inputs.size() >= num_in
              && self->outputs.size() >= num_out)
          {
            have_ports = true;
            break;
          }
        }
        if (!loop->synchronize())
        {
          ossia::logger().error(
              "PipeWire: synchronize() failed while waiting for ports");
          return;
        }
      }
      if (!have_ports)
      {
        ossia::logger().error(
            "PipeWire: ports never appeared in graph — engine inactive");
        return;
      }
    }

    // The graph may refuse our rate (global clock.force-rate, or a
    // competing client's newer force-rate stamp): the DSP ports then carry
    // audio at the graph rate, not ours. Report the real rate so the host
    // resamples its material for what will actually be played.
    {
      std::uint32_t seen = 0;
      for (int j = 0; j < 100; ++j)
      {
        seen = m_observed_rate.load(std::memory_order_relaxed);
        if (seen != 0)
          break;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
      }
      if (seen != 0 && seen != static_cast<std::uint32_t>(setup.rate))
      {
        ossia::logger().warn(
            "PipeWire: the graph runs at {} Hz, not the requested {} Hz; "
            "the engine will use {} Hz",
            seen, setup.rate, seen);
        this->effective_sample_rate = seen;
      }
    }

    activated = true;

    // A node can end up scheduled by nothing, silently: if no active
    // driver with priority exists (Dummy-Driver missing or a session
    // manager race), pw_context_recalc_graph's unassigned-node pass calls
    // remove_from_driver and the node just stops — no error reaches the
    // client, playback simply never starts. Watch for the absence of
    // process cycles and re-export the node, which re-runs activation and
    // driver assignment.
    m_watchdog = std::thread{[this] { watchdog_main(); }};
  }

  std::uint32_t filter_node_id() const noexcept
  {
    if (!this->filter)
      return 0xFFFFFFFFu;
    auto& pw = libremidi::pipewire::load();
    if (!pw.filter_get_node_id)
      return 0xFFFFFFFFu;
    return pw.filter_get_node_id(this->filter);
  }

  // Modern PipeWire/WirePlumber does NOT set port.physical=true on
  // ALSA hw ports — match the node's media.class (Audio/Source,
  // Audio/Sink) rather than n.physical.
  void autoconnect()
  {
    const auto our_node = filter_node_id();
    if (our_node == 0xFFFFFFFFu)
      return;

    const std::string default_sink = loop->default_audio_sink_name();
    const std::string default_source = loop->default_audio_source_name();
    ossia::logger().info(
        "PipeWire autoconnect: defaults src='{}' sink='{}'",
        default_source, default_sink);

    std::vector<std::uint32_t> source_outputs;
    std::vector<std::uint32_t> sink_inputs;
    std::vector<std::uint32_t> self_in_ids, self_out_ids;
    bool have_self = false;

    for (int attempt = 0; attempt < 50; ++attempt)
    {
      auto snap = loop->snapshot();

      self_in_ids.clear();
      self_out_ids.clear();
      have_self = false;
      if (const auto* self_node = snap.find_by_id(our_node))
      {
        have_self = true;
        for (const auto& p : self_node->inputs)
          self_in_ids.push_back(p.id);
        for (const auto& p : self_node->outputs)
          self_out_ids.push_back(p.id);
      }

      source_outputs.clear();
      sink_inputs.clear();
      if (!default_source.empty())
      {
        if (const auto* n = snap.find_by_name(default_source))
        {
          for (const auto& p : n->outputs)
            source_outputs.push_back(p.id);
        }
      }
      if (!default_sink.empty())
      {
        if (const auto* n = snap.find_by_name(default_sink))
        {
          for (const auto& p : n->inputs)
            sink_inputs.push_back(p.id);
        }
      }

      // A suspended sink legitimately has no ports — target.object
      // on the filter wakes it during activation.
      if (have_self && !source_outputs.empty() && !sink_inputs.empty())
        break;
      if (!loop->synchronize())
        break;
    }

    if (!have_self)
      return;

    ossia::logger().info(
        "PipeWire autoconnect: src_ports={}, sink_ports={}, "
        "self_in={}, self_out={}",
        source_outputs.size(), sink_inputs.size(),
        self_in_ids.size(), self_out_ids.size());

    {
      auto snap = loop->snapshot();
      ossia::logger().info(
          "PipeWire autoconnect: snapshot has {} nodes", snap.nodes.size());
      for (const auto& n : snap.nodes)
      {
        ossia::logger().info(
            "PipeWire autoconnect: snap id={} name='{}' class='{}' "
            "inputs={} outputs={}",
            n.id, n.name, n.media_class_str, n.inputs.size(),
            n.outputs.size());
      }
    }

    for (std::size_t i = 0;
         i < self_in_ids.size() && i < source_outputs.size(); ++i)
    {
      if (auto* link = libremidi::pipewire::link_ports(
              *loop, source_outputs[i], self_in_ids[i]))
        links.push_back(link);
    }
    for (std::size_t i = 0;
         i < self_out_ids.size() && i < sink_inputs.size(); ++i)
    {
      if (auto* link = libremidi::pipewire::link_ports(
              *loop, self_out_ids[i], sink_inputs[i]))
        links.push_back(link);
    }
  }

  void wait(int ms) override
  {
    if (ms > 0)
      std::this_thread::sleep_for(std::chrono::milliseconds(ms));
  }

  bool running() const override { return loop && activated; }

  // Must not be called from the pipewire loop thread: joining the
  // watchdog while it waits for the loop lock held by the caller would
  // deadlock.
  void stop() override
  {
    audio_engine::stop();

    // The watchdog reconnects the filter from its own thread; it must be
    // gone before the teardown below starts destroying what it touches.
    m_watchdog_quit.store(true, std::memory_order_release);
    if (m_watchdog.joinable())
      m_watchdog.join();
    // Tear down whenever a filter exists, not just when fully activated:
    // the constructor's failure paths after a successful filter_connect
    // leave a connected filter whose process callback keeps firing, and
    // destroying this object without disconnecting it first would let the
    // RT thread run over freed members.
    if (!loop)
      return;

    auto& pw = libremidi::pipewire::load();

    // Tear-down order: disconnect → sync → drop links → destroy → sync.
    // pw_filter_* / pw_proxy_destroy must run under the thread_loop lock.
    if (this->filter)
    {
      loop->with_lock([&] {
        if (int res = pw.filter_disconnect(this->filter); res < 0)
        {
          ossia::logger().warn(
              "PipeWire: filter_disconnect failed: {}", spa_strerror(res));
        }
      });
      (void)loop->synchronize();
    }

    for (auto* link : this->links)
      libremidi::pipewire::unlink_ports(*loop, link);
    this->links.clear();

    if (this->filter)
    {
      loop->with_lock([&] {
        pw.filter_destroy(this->filter);
        this->filter = nullptr;
      });
    }

    (void)loop->synchronize();
    activated = false;
  }

  ~pipewire_audio_protocol() override { stop(); }

  // RT thread: no locks, no allocation, no logging.
  //
  // The buffers are mmapped with a capacity taken from *this client's*
  // clock.quantum-limit while clock.duration is bounded by the *daemon's*
  // clock.quantum-limit; the two can be configured apart, and
  // pw_filter_get_dsp_buffer would then stamp and let us write past
  // maxsize. So dequeue ourselves, take the capacity into account, and
  // requeue — exactly once per port per cycle, since a second dequeue in
  // the same cycle hands out a different buffer.

  // Dequeues one port buffer; caps `safe` to its capacity. The buffer is
  // NOT requeued yet: outputs are stamped with the final safe count first.
  static pw_buffer* dequeue_cycle_buffer(
      const auto& pw, void* port, float*& data, std::uint32_t& safe) noexcept
  {
    pw_buffer* b = pw.filter_dequeue_buffer(port);
    if (!b || !b->buffer || b->buffer->n_datas < 1
        || !b->buffer->datas[0].data)
    {
      data = nullptr;
      return b;
    }
    auto& d = b->buffer->datas[0];
    data = static_cast<float*>(d.data);
    const std::uint32_t cap = d.maxsize / sizeof(float);
    if (cap < safe)
      safe = cap;
    return b;
  }

  static void requeue_cycle_buffer(
      const auto& pw, void* port, pw_buffer* b, bool output,
      std::uint32_t frames) noexcept
  {
    if (!b)
      return;
    if (output && b->buffer && b->buffer->n_datas >= 1)
    {
      if (auto* chunk = b->buffer->datas[0].chunk)
      {
        chunk->offset = 0;
        chunk->size = frames * sizeof(float);
        chunk->stride = sizeof(float);
        chunk->flags = 0;
      }
    }
    pw.filter_queue_buffer(port, b);
  }

  static bool can_dequeue(const auto& pw) noexcept
  {
    return pw.filter_dequeue_buffer && pw.filter_queue_buffer;
  }

  // Fetches every port's buffer for the cycle into m_cycle_in/out and
  // returns the frame count that is safe to read and write everywhere.
  std::uint32_t fetch_cycle_buffers(const auto& pw, std::uint32_t nframes)
  {
    const auto inputs = input_ports.size();
    const auto outputs = output_ports.size();

    if (!can_dequeue(pw))
    {
      // Old libpipewire without the dequeue API: keep the historical
      // behaviour (no capacity check).
      for (std::size_t i = 0; i < inputs; i++)
        m_cycle_in[i] = static_cast<float*>(
            pw.filter_get_dsp_buffer(input_ports[i], nframes));
      for (std::size_t i = 0; i < outputs; i++)
        m_cycle_out[i] = static_cast<float*>(
            pw.filter_get_dsp_buffer(output_ports[i], nframes));
      return nframes;
    }

    std::uint32_t safe = nframes;
    for (std::size_t i = 0; i < inputs; i++)
      m_buf_in[i] = dequeue_cycle_buffer(pw, input_ports[i], m_cycle_in[i], safe);
    for (std::size_t i = 0; i < outputs; i++)
      m_buf_out[i] = dequeue_cycle_buffer(pw, output_ports[i], m_cycle_out[i], safe);

    for (std::size_t i = 0; i < inputs; i++)
      requeue_cycle_buffer(pw, input_ports[i], m_buf_in[i], false, safe);
    for (std::size_t i = 0; i < outputs; i++)
      requeue_cycle_buffer(pw, output_ports[i], m_buf_out[i], true, safe);
    return safe;
  }

  static void
  clear_buffers(pipewire_audio_protocol& self, std::uint32_t nframes,
                std::size_t outputs)
  {
    auto& pw = libremidi::pipewire::load();
    if (!can_dequeue(pw))
    {
      for (std::size_t i = 0; i < outputs; i++)
      {
        auto* chan = static_cast<float*>(
            pw.filter_get_dsp_buffer(self.output_ports[i], nframes));
        if (chan)
          for (std::size_t j = 0; j < nframes; j++)
            chan[j] = 0.f;
      }
      return;
    }

    for (std::size_t i = 0; i < outputs; i++)
    {
      float* data{};
      std::uint32_t safe = nframes;
      auto* b = dequeue_cycle_buffer(pw, self.output_ports[i], data, safe);
      if (data)
        for (std::size_t j = 0; j < safe; j++)
          data[j] = 0.f;
      requeue_cycle_buffer(pw, self.output_ports[i], b, true, data ? safe : 0);
    }
  }

  void do_process(std::uint32_t nframes, double secs, double rate)
  {
    auto& pw = libremidi::pipewire::load();

    tick_start();

    const auto inputs = input_ports.size();
    const auto outputs = output_ports.size();
    if (stop_processing)
    {
      tick_clear();
      clear_buffers(*this, nframes, outputs);
      return;
    }

    const std::uint32_t frames = fetch_cycle_buffers(pw, nframes);

    bool missing_input = false;
    for (std::size_t i = 0; i < inputs; i++)
      missing_input |= !m_cycle_in[i];
    if (missing_input)
      std::memset(m_silence.data(), 0, m_silence.size() * sizeof(float));

    // The graph quantum tracks what we forced only eventually (and not at
    // all under a global clock.force-quantum or a competing client); the
    // engine's buffers are sized for effective_buffer_size, so process any
    // larger cycle in slices of it rather than skipping the cycle, which
    // would leave the outputs in NEED_DATA — i.e. permanent silence.
    const auto block = static_cast<std::uint32_t>(effective_buffer_size);
    ossia::pipewire::for_each_chunk(
        frames, block, [&](std::uint32_t offset, std::uint32_t n) {
      ossia::pipewire::assign_chunk_pointers(
          m_cycle_in.data(), m_chunk_in.data(), inputs, offset,
          m_silence.data());
      ossia::pipewire::assign_chunk_pointers(
          m_cycle_out.data(), m_chunk_out.data(), outputs, offset,
          m_scratch.data());

      ossia::audio_tick_state ts{
          m_chunk_in.data(), m_chunk_out.data(), (int)inputs, (int)outputs,
          n, secs + offset / rate};
      audio_tick(ts);
    });
    tick_end();
  }

  static void on_process(void* userdata, struct spa_io_position* position)
  {
    [[maybe_unused]] static const thread_local auto _ = [] {
      ossia::set_thread_name("ossia audio 0");
      ossia::set_thread_pinned(thread_type::Audio, 0);
      return 0;
    }();

    if (!userdata || !position)
      return;

    auto& self = *static_cast<pipewire_audio_protocol*>(userdata);
    self.m_cycles.fetch_add(1, std::memory_order_relaxed);
    const std::uint32_t nframes = position->clock.duration;
    const std::uint32_t rate = position->clock.rate.denom;
    const double current_time = position->clock.nsec * 1e-9;

    // Logging from the process callback is not realtime-safe; these fire
    // only when the graph reconfigures (rare), which beats both per-cycle
    // spam and silent misbehaviour — and a pathologically flapping graph
    // is cut off after a few transitions. A zero duration/rate is an idle
    // or reconfiguring cycle, not a value: it must not disturb the
    // trackers (observe(0) would reset the first-cycle sentinel and eat
    // the next 'restored' notification).
    using event = ossia::pipewire::quantum_tracker::event;
    if (nframes != 0)
    {
      switch (self.m_quantum.observe(nframes))
      {
        case event::mismatch:
          if (may_log(self.m_quantum_logs))
            ossia::logger().warn(
                "PipeWire: graph quantum is {} but {} was requested; "
                "adapting by processing in chunks",
                nframes, self.effective_buffer_size);
          break;
        case event::recovered:
          if (may_log(self.m_quantum_logs))
            ossia::logger().info(
                "PipeWire: graph quantum restored to {}", nframes);
          break;
        default:
          break;
      }
    }
    if (rate != 0)
    {
      switch (self.m_rate.observe(rate))
      {
        case event::mismatch:
          if (may_log(self.m_rate_logs))
            ossia::logger().warn(
                "PipeWire: graph sample rate is {} but {} was requested; "
                "audio will play at the wrong speed until it is restored",
                rate, self.m_rate.expected);
          break;
        case event::recovered:
          if (may_log(self.m_rate_logs))
            ossia::logger().info(
                "PipeWire: graph sample rate restored to {}", rate);
          break;
        default:
          break;
      }
    }

    if (rate != 0)
      self.m_observed_rate.store(rate, std::memory_order_relaxed);

    if (nframes == 0)
      return;

    // Chunk offsets are in graph-rate samples; use the cycle's actual rate
    // for the time math (m_rate.expected as a fallback for a zero clock).
    self.do_process(
        nframes, current_time, rate != 0 ? rate : self.m_rate.expected);
  }

  // Stall watchdog state, public so hosts and tests can observe it.
  // stall_timeout_ms may be lowered at runtime (tests) or raised by hosts
  // that expect long scheduling gaps.
  std::atomic<int> stall_timeout_ms{3000};
  std::atomic<std::uint32_t> stalls_detected{};
  std::atomic<std::uint32_t> recover_attempts{};
  static constexpr std::uint32_t max_recover_attempts = 5;

private:
  void watchdog_main()
  {
    ossia::set_thread_name("ossia pw wdog");
    auto& pw = libremidi::pipewire::load();

    std::uint64_t last = m_cycles.load(std::memory_order_relaxed);
    auto last_progress = std::chrono::steady_clock::now();
    while (!m_watchdog_quit.load(std::memory_order_acquire))
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      const auto now = std::chrono::steady_clock::now();
      const auto cur = m_cycles.load(std::memory_order_relaxed);
      if (cur != last)
      {
        last = cur;
        last_progress = now;
        // The graph resumed: this outage is over, a future one gets a
        // fresh set of recovery attempts.
        recover_attempts.store(0, std::memory_order_relaxed);
        continue;
      }
      const auto stalled_ms
          = std::chrono::duration_cast<std::chrono::milliseconds>(
                now - last_progress)
                .count();
      if (stalled_ms < stall_timeout_ms.load(std::memory_order_relaxed))
        continue;

      stalls_detected.fetch_add(1, std::memory_order_relaxed);
      const auto attempt
          = recover_attempts.fetch_add(1, std::memory_order_relaxed) + 1;
      if (attempt > max_recover_attempts)
      {
        ossia::logger().error(
            "PipeWire: still no process cycles after {} reconnections; "
            "giving up. The daemon is likely not scheduling any audio "
            "(no usable driver); restarting PipeWire may help",
            max_recover_attempts);
        return;
      }

      ossia::logger().warn(
          "PipeWire: no process cycles for {} ms; re-exporting the node "
          "(attempt {}/{}). This happens when the graph has no usable "
          "driver or the node was left unscheduled",
          stalled_ms, attempt, max_recover_attempts);

      bool connect_failed = false;
      loop->with_lock([&] {
        if (!this->filter)
          return;
        if (int res = pw.filter_disconnect(this->filter); res < 0)
          ossia::logger().warn(
              "PipeWire: watchdog filter_disconnect failed: {}",
              spa_strerror(res));
        if (int res = pw.filter_connect(
                this->filter, PW_FILTER_FLAG_RT_PROCESS, nullptr, 0);
            res < 0)
        {
          // A failed connect leaves the filter in the CONNECTING state
          // (only a successful proxy teardown resets it), so every later
          // connect would return -EBUSY: retrying is pointless.
          connect_failed = true;
          ossia::logger().error(
              "PipeWire: watchdog filter_connect failed: {}; automatic "
              "recovery is not possible, restart the audio engine",
              spa_strerror(res));
        }
      });
      if (connect_failed)
        return;
      (void)loop->synchronize();

      // Give the re-exported node a full timeout window to come up.
      last = m_cycles.load(std::memory_order_relaxed);
      last_progress = std::chrono::steady_clock::now();
    }
  }

  std::atomic<std::uint64_t> m_cycles{};
  std::thread m_watchdog;
  std::atomic_bool m_watchdog_quit{};

  ossia::pipewire::quantum_tracker m_quantum{};
  ossia::pipewire::quantum_tracker m_rate{};

  // Written by the process callback, read by the constructor to learn the
  // rate the graph actually granted us.
  std::atomic<std::uint32_t> m_observed_rate{};

  // A graph flapping between quantums/rates every cycle would otherwise
  // turn the transition logs back into per-cycle RT logging.
  std::uint32_t m_quantum_logs{};
  std::uint32_t m_rate_logs{};

  static bool may_log(std::uint32_t& n) noexcept
  {
    if (n >= 16)
      return false;
    if (++n == 16)
      ossia::logger().warn(
          "PipeWire: the graph configuration keeps changing; further "
          "changes will not be logged");
    return true;
  }

  // Cycle-wide buffer starts (one entry per port; null when pipewire had
  // no buffer for the port this cycle) and the per-chunk views handed to
  // the tick. Sized in the constructor, touched only by the process
  // callback afterwards.
  ossia::pod_vector<float*> m_cycle_in, m_cycle_out;
  ossia::pod_vector<float*> m_chunk_in, m_chunk_out;
  ossia::pod_vector<pw_buffer*> m_buf_in, m_buf_out;

  // Missing inputs read zeroes, missing outputs write into a discard
  // buffer. Keep them distinct: one shared dummy would feed the previous
  // chunk's discarded output back into the missing inputs.
  ossia::pod_vector<float> m_silence;
  ossia::pod_vector<float> m_scratch;
};

} // namespace ossia

#endif
#endif
