#pragma once
#if __has_include( \
    <alsa/asoundlib.h>) && __has_include(<zita-alsa-pcmi-ardour.h>) && !defined(__EMSCRIPTEN__)

#include <ossia/audio/audio_engine.hpp>
#include <ossia/audio/libasound.hpp>
#include <ossia/detail/logger.hpp>
#include <ossia/detail/pod_vector.hpp>
#include <ossia/detail/thread.hpp>

#include <alsa/asoundlib.h>

#include <atomic>
#include <chrono>
#include <cstring>
#include <future>
#include <memory>
#include <stdexcept>
#include <string>
#include <thread>

#include <zita-alsa-pcmi-ardour.h>

#define OSSIA_AUDIO_ALSA 1

namespace ossia
{
class alsa_engine final : public audio_engine
{
public:
  //! periods: 0 chooses the smallest count the device sustains.
  alsa_engine(
      std::string card_in, std::string card_out, int inputs, int outputs, int rate,
      int bs, int periods = 0)
  {
    // A direction without channels is not opened at all: a capture device
    // that is busy or missing would otherwise fail the whole engine. One of
    // the two stays open, as it is the one that clocks the engine: playback
    // when neither has channels.
    const bool capture = inputs > 0 && !card_in.empty();
    const bool playback = (outputs > 0 && !card_out.empty()) || !capture;
    if(playback && card_out.empty())
      throw std::runtime_error("ALSA: no device selected");

    // Linked streams run on one clock and are fine with 2 periods. Streams
    // that cannot be linked (dmix with dsnoop) drift in phase and need 3.
    // Linking two cards succeeds but only starts them together: their clocks
    // still differ, which the retry below handles. Some devices need more than
    // the period they accept: full-speed USB (1 ms packets) under-runs
    // continuously with two periods shorter than 2 ms. With periods == 0, a
    // start that keeps under-running is retried with one more period.
    unsigned nfrag = periods > 0 ? periods : 2;
    for(;;)
    {
      m_client.reset();
      m_client = std::make_unique<Alsa_pcmi>(
          playback ? card_out.c_str() : nullptr, capture ? card_in.c_str() : nullptr,
          nullptr, rate, bs, nfrag, nfrag, Alsa_pcmi::DEBUG_INIT,
          playback ? std::max(outputs, 0) : 0, capture ? inputs : 0);
      auto& client = *m_client;
      if(periods <= 0 && nfrag == 2 && client.state() == 0 && client.play_handle()
         && client.capt_handle() && !client.synced())
      {
        nfrag = 3;
        continue;
      }

      // Playback clocks the engine and is what the user hears: without it the
      // engine would run, silently. A missing capture only removes inputs.
      if(client.state() != 0 || (playback && !client.play_handle()))
        throw std::runtime_error(
            describe_error(client, card_in, card_out, playback, capture, rate, bs));

      start(inputs, outputs, playback ? card_out : card_in);

      if(periods > 0 || nfrag >= 4 || !keeps_underrunning())
        break;

      ossia::logger().info(
          "ALSA: {} under-runs with {} periods, trying {}",
          playback ? card_out : card_in, nfrag, nfrag + 1);
      shutdown();
      nfrag++;
    }

    if(capture && !m_client->capt_handle())
      ossia::logger().warn(
          "ALSA: cannot open {} for capture ({}), continuing without inputs", card_in,
          ossia::libasound::instance().strerror(m_client->capt_open_error()));
    if(m_fsize != bs)
      ossia::logger().info(
          "ALSA: {} uses a period of {} frames instead of {}",
          playback ? card_out : card_in, m_fsize, bs);

    m_activated = true;
  }

  void stop() override
  {
    audio_engine::stop();
    shutdown();
    m_activated = false;
  }

  ~alsa_engine() override { stop(); }

  bool running() const override
  {
    if(!m_client)
      return false;
    return m_activated && !m_failed.load(std::memory_order_relaxed);
  }

  bool failed() const noexcept override
  {
    return m_failed.load(std::memory_order_relaxed);
  }

private:
  void start(int inputs, int outputs, const std::string& name)
  {
    auto& client = *m_client;
    m_fsize = client.fsize();
    effective_sample_rate = client.fsamp();
    effective_buffer_size = m_fsize;
    effective_inputs = std::min(inputs, (int)client.ncapt());
    effective_outputs = std::min(outputs, (int)client.nplay());
    effective_input_latency = client.capt_handle() ? m_fsize : 0;
    effective_output_latency
        = client.play_handle() ? m_fsize * (int)client.real_play_nfrag() : 0;

    m_capture.assign(m_fsize * std::max(effective_inputs, 1), 0.f);
    m_playback.assign(m_fsize * std::max(effective_outputs, 1), 0.f);
    m_capture_ptrs.clear();
    m_playback_ptrs.clear();
    for(int i = 0; i < effective_inputs; i++)
      m_capture_ptrs.push_back(m_capture.data() + i * m_fsize);
    for(int i = 0; i < effective_outputs; i++)
      m_playback_ptrs.push_back(m_playback.data() + i * m_fsize);

    xruns = 0;
    m_failed = false;
    m_stop_token = true;

    std::promise<int> started;
    auto start_result = started.get_future();
    m_thread = ossia::thread{[this, &started] {
      init_thread();
      const int err = m_client->pcm_start();
      started.set_value(err);
      if(err == 0)
        thread_duplex();
    }};
    set_thread_realtime(m_thread);

    if(start_result.get() != 0)
    {
      m_thread.join();
      m_client.reset();
      throw std::runtime_error("ALSA: " + name + " could not be started");
    }
  }

  //! A configuration the device cannot sustain under-runs on almost every
  //! period. The first ones are skipped: two cards starting together often
  //! under-run once or twice while their streams settle.
  bool keeps_underrunning()
  {
    const int period_ms = std::max(1, 1000 * m_fsize / std::max(effective_sample_rate, 1));
    std::this_thread::sleep_for(std::chrono::milliseconds(std::max(50, 4 * period_ms)));
    const auto before = xruns.load();
    std::this_thread::sleep_for(std::chrono::milliseconds(std::max(100, 20 * period_ms)));
    return xruns.load() - before >= 2 || m_failed.load();
  }

  void shutdown()
  {
    if(m_client)
    {
      m_stop_token.store(false, std::memory_order_relaxed);
      if(m_thread.joinable())
        m_thread.join();
      m_client.reset();
    }
  }

  void thread_duplex()
  {
    auto& client = *this->m_client;
    const int n = m_fsize;
    int failures = 0;
    int idle_xruns = 0;

    while(m_stop_token.load(std::memory_order_relaxed))
    {
      long k = client.pcm_wait();

      // pcm_wait reports 1 after recovering from an xrun, and below 0 when
      // poll() timed out (1 s) or the recovery failed.
      if(client.state() < 0)
      {
        if(++failures >= 3)
        {
          ossia::logger().error("ALSA: the device stopped responding");
          m_failed = true;
          break;
        }
        continue;
      }
      failures = 0;
      if(client.state() > 0)
      {
        xruns.fetch_add(1, std::memory_order_relaxed);
        if(++idle_xruns >= 200)
        {
          ossia::logger().error(
              "ALSA: the device under-runs continuously; try a larger buffer size");
          m_failed = true;
          break;
        }
      }
      if(k >= n)
        idle_xruns = 0;

      while(k >= n)
      {
        client.capt_init(n);
        for(int c = 0; c < effective_inputs; c++)
          client.capt_chan(c, m_capture_ptrs[c], n, 1);
        client.capt_done(n);

        std::fill(m_playback.begin(), m_playback.end(), 0.f);
        process(m_capture_ptrs.data(), m_playback_ptrs.data(), n);

        client.play_init(n);
        for(int c = 0; c < effective_outputs; c++)
          client.play_chan(c, m_playback_ptrs[c], n, 1);
        for(int c = effective_outputs; c < (int)client.nplay(); c++)
          client.clear_chan(c, n);
        client.play_done(n);

        k -= n;
      }
    }

    client.pcm_stop();
  }

  static std::string describe_error(
      const Alsa_pcmi& client, const std::string& card_in, const std::string& card_out,
      bool playback, bool capture, int rate, int bs)
  {
    if(playback && !client.play_handle() && client.play_open_error() < 0)
      return "ALSA: cannot open " + card_out + " for playback: "
             + ossia::libasound::instance().strerror(client.play_open_error());
    if(capture && !client.capt_handle() && client.capt_open_error() < 0)
      return "ALSA: cannot open " + card_in + " for capture: "
             + ossia::libasound::instance().strerror(client.capt_open_error());

    const char* stream = client.error_stream() ? client.error_stream() : "the device";
    const std::string& name = (client.error_stream()
                               && std::strcmp(client.error_stream(), "capture") == 0)
                                  ? card_in
                                  : card_out;
    switch(client.state())
    {
      case -3:
        return "ALSA: " + name + " does not support " + std::to_string(rate)
               + " Hz for " + stream;
      case -4:
        return "ALSA: " + name + " does not support a buffer of " + std::to_string(bs)
               + " frames for " + stream;
      case -5:
        return "ALSA: " + name + " does not support this number of periods for "
               + stream;
      case -6:
        return "ALSA: " + name + " uses a sample format score cannot handle for "
               + stream;
      default:
        return "ALSA: " + name + " rejected the configuration for " + stream + " ("
               + std::to_string(rate) + " Hz, " + std::to_string(bs) + " frames)";
    }
  }

  void init_thread()
  {
    ossia::set_thread_name("ossia audio 0");
    ossia::set_thread_pinned(thread_type::Audio, 0);
  }

  void process(float** float_input, float** float_output, uint64_t nframes)
  {
    tick_start();

    if(stop_processing)
    {
      tick_clear();
      return;
    }

    using namespace std::chrono;
    const auto now = clk::now();
    if(m_start_time == clk::time_point{})
      m_start_time = now;
    ossia::audio_tick_state ts{
        float_input,
        float_output,
        effective_inputs,
        effective_outputs,
        nframes,
        duration_cast<nanoseconds>(now - m_start_time).count() / 1e9};
    audio_tick(ts);

    tick_end();
  }

  std::unique_ptr<Alsa_pcmi> m_client;
  ossia::thread m_thread;
  using clk = std::chrono::steady_clock;
  clk::time_point m_start_time{};
  ossia::float_vector m_capture;
  ossia::float_vector m_playback;
  ossia::pod_vector<float*> m_capture_ptrs;
  ossia::pod_vector<float*> m_playback_ptrs;
  int m_fsize{};
  std::atomic_bool m_stop_token{true};
  std::atomic_bool m_activated{};
  std::atomic_bool m_failed{};
};
}

#endif
