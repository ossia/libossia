#pragma once
#if __has_include( \
    <alsa/asoundlib.h>) && __has_include(<zita-alsa-pcmi-ardour.h>) && !defined(__EMSCRIPTEN__)

#include <ossia/audio/audio_engine.hpp>
#include <ossia/dataflow/float_to_sample.hpp>
#include <ossia/dataflow/sample_to_float.hpp>
#include <ossia/detail/logger.hpp>
#include <ossia/detail/pod_vector.hpp>
#include <ossia/detail/thread.hpp>

#include <alsa/asoundlib.h>
#include <kfr/base/conversion.hpp>

#include <cmath>

#include <atomic>
#include <string_view>
#include <thread>
#include <vector>

#include <zita-alsa-pcmi-ardour.h>

#define OSSIA_AUDIO_ALSA 1

namespace ossia
{
class alsa_engine final : public audio_engine
{
public:
  alsa_engine(
      std::string card_in, std::string card_out, int inputs, int outputs, int rate,
      int bs)
  {
    m_client = std::make_unique<Alsa_pcmi>(
        card_in.c_str(), card_out.c_str(), nullptr, rate, bs, 2, 2,
        Alsa_pcmi::DEBUG_ALL);

    if(m_client->state() != 0)
    {
      throw std::runtime_error("Could not open ALSA");
    }

    effective_sample_rate = rate;
    effective_buffer_size = bs;
    effective_inputs = std::min(inputs, (int)m_client->ncapt());
    effective_outputs = std::min(outputs, (int)m_client->nplay());
    m_thread = std::thread{[this] {
      init_thread();
      thread_duplex();
    }};
    set_thread_realtime(m_thread);

    m_activated = true;
  }

  void stop() override
  {
    audio_engine::stop();

    if(m_client)
    {
      m_stop_token.store(false, std::memory_order_relaxed);
      assert(m_thread.joinable());
      m_thread.join();

      m_client.reset();
    }

    m_activated = false;
  }

  ~alsa_engine() override { stop(); }

  bool running() const override
  {
    if(!m_client)
      return false;
    return m_activated;
  }

  void thread_duplex()
  {
    auto& client = *this->m_client.get();

    std::vector<float> buf_capture;
    buf_capture.resize(effective_buffer_size * effective_inputs);
    std::vector<float> buf_capture_deint;
    buf_capture_deint.resize(effective_buffer_size * effective_inputs);
    std::vector<float*> ptrs_capture;
    for(int i = 0; i < effective_inputs; i++)
      ptrs_capture.push_back(buf_capture_deint.data() + i * effective_buffer_size);

    std::vector<float> buf_playback;
    buf_playback.resize(effective_buffer_size * effective_outputs);
    std::vector<float> buf_playback_deint;
    buf_playback_deint.resize(effective_buffer_size * effective_outputs);
    std::vector<float*> ptrs_playback;
    for(int i = 0; i < effective_outputs; i++)
      ptrs_playback.push_back(buf_playback_deint.data() + i * effective_buffer_size);

    client.pcm_start();

    int k = 0;
    while(m_stop_token.load(std::memory_order_relaxed))
    {
      k = client.pcm_wait();

      while(k >= effective_buffer_size)
      {
        client.capt_init(effective_buffer_size);
        for(int c = 0; c < effective_inputs; c++)
          client.capt_chan(
              c, buf_capture.data() + c, effective_buffer_size, effective_inputs);
        client.capt_done(effective_buffer_size);

        kfr::deinterleave(
            ptrs_capture.data(), buf_capture.data(), effective_inputs,
            effective_buffer_size);

        std::fill_n(
            buf_playback_deint.data(), effective_buffer_size * effective_outputs, 0);

        process(ptrs_capture.data(), ptrs_playback.data(), effective_buffer_size);

        kfr::interleave(
            buf_playback.data(), (const float**)ptrs_playback.data(), effective_outputs,
            effective_buffer_size);

        client.play_init(effective_buffer_size);
        for(int c = 0; c < effective_outputs; c++)
          client.play_chan(
              c, buf_playback.data() + c, effective_buffer_size, effective_outputs);

        for(std::size_t i = effective_outputs; i < client.nplay(); i++)
          client.clear_chan(i, effective_buffer_size);

        client.play_done(effective_buffer_size);

        k -= effective_buffer_size;
      }
    }

    client.pcm_stop();
  }

private:
  void init_thread()
  {
    ossia::set_thread_name("ossia audio 0");
    ossia::set_thread_pinned(thread_type::Audio, 0);
  }

  void process(float** float_input, float** float_output, uint64_t nframes)
  {
    auto& self = *static_cast<alsa_engine*>(this);
    self.tick_start();

    const auto inputs = this->effective_inputs;
    const auto outputs = this->effective_outputs;
    if(self.stop_processing)
    {
      self.tick_clear();
      return;
    }

    using namespace std::chrono;
    ossia::audio_tick_state ts{
        float_input,
        float_output,
        (int)inputs,
        (int)outputs,
        nframes,
        duration_cast<nanoseconds>(m_last_time - m_start_time).count() / 1e9};
    self.audio_tick(ts);

    self.tick_end();
    m_last_time = clk::now();
  }

  std::unique_ptr<Alsa_pcmi> m_client;
  std::thread m_thread;
  using clk = std::chrono::steady_clock;
  clk::time_point m_start_time{};
  clk::time_point m_last_time{};
  std::atomic_bool m_stop_token{true};
  std::atomic_bool m_activated{};
};
}

#endif
