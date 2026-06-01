#pragma once
#include <ossia/detail/config.hpp>

#if defined(OSSIA_ENABLE_PIPEWIRE)
#if __has_include(<libremidi/backends/linux/pipewire/context.hpp>) \
    && __has_include(<pipewire/filter.h>)           \
    && __has_include(<spa/param/latency-utils.h>)
#define OSSIA_AUDIO_PIPEWIRE 1

#include <ossia/audio/audio_engine.hpp>
#include <ossia/detail/logger.hpp>
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
      throw std::runtime_error("PipeWire: cannot connect");

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

    activated = true;
    this->effective_buffer_size = setup.buffer_size;
    this->effective_sample_rate = setup.rate;
    this->effective_inputs = setup.inputs.size();
    this->effective_outputs = setup.outputs.size();
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

  void stop() override
  {
    audio_engine::stop();
    if (!loop || !activated)
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
  static void
  clear_buffers(pipewire_audio_protocol& self, std::uint32_t nframes,
                std::size_t outputs)
  {
    auto& pw = libremidi::pipewire::load();
    for (std::size_t i = 0; i < outputs; i++)
    {
      auto* chan
          = static_cast<float*>(pw.filter_get_dsp_buffer(self.output_ports[i], nframes));
      if (chan)
        for (std::size_t j = 0; j < nframes; j++)
          chan[j] = 0.f;
    }
  }

  void do_process(std::uint32_t nframes, double secs)
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

    auto* dummy = static_cast<float*>(alloca(sizeof(float) * nframes));
    std::memset(dummy, 0, sizeof(float) * nframes);

    auto** float_input = static_cast<float**>(alloca(sizeof(float*) * inputs));
    auto** float_output = static_cast<float**>(alloca(sizeof(float*) * outputs));
    for (std::size_t i = 0; i < inputs; i++)
    {
      float_input[i]
          = static_cast<float*>(pw.filter_get_dsp_buffer(input_ports[i], nframes));
      if (float_input[i] == nullptr)
        float_input[i] = dummy;
    }
    for (std::size_t i = 0; i < outputs; i++)
    {
      float_output[i]
          = static_cast<float*>(pw.filter_get_dsp_buffer(output_ports[i], nframes));
      if (float_output[i] == nullptr)
        float_output[i] = dummy;
    }

    ossia::audio_tick_state ts{
        float_input, float_output, (int)inputs, (int)outputs, nframes, secs};
    audio_tick(ts);
    tick_end();
  }

  static void on_process(void* userdata, struct spa_io_position* position)
  {
    [[maybe_unused]] static const thread_local auto _ = [] {
      ossia::set_thread_name("ossia audio 0");
      ossia::set_thread_pinned(thread_type::Audio, 0);
      return 0;
    }();

    if (!userdata)
      return;

    auto& self = *static_cast<pipewire_audio_protocol*>(userdata);
    const std::uint32_t nframes = position->clock.duration;
    const double current_time_ns = position->clock.nsec * 1e-9;

    if (nframes != static_cast<std::uint32_t>(self.effective_buffer_size))
    {
      ossia::logger().warn(
          "PipeWire: unexpected block size {} (expected {}), skipping cycle",
          nframes, self.effective_buffer_size);
      return;
    }

    self.do_process(nframes, current_time_ns);
  }
};

} // namespace ossia

#endif
#endif
