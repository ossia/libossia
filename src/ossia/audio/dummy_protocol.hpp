#pragma once
#include <ossia/audio/audio_engine.hpp>
#include <thread>

namespace ossia
{
class dummy_engine final : public audio_engine
{
  int effective_sample_rate{}, effective_buffer_size{};
  std::atomic_bool m_active;

public:
  dummy_engine(int rate, int bs)
  {
    effective_sample_rate = rate;
    effective_buffer_size = bs;
    effective_inputs = 0;
    effective_outputs = 0;

    setup_thread();
  }

  bool running() const override
  {
    return m_active;
  }

  void setup_thread()
  {
    m_active = true;

    int us_per_buffer = 1e6 * double(effective_buffer_size) / double(effective_sample_rate);

    m_runThread = std::thread{[this, us_per_buffer] {
      using clk = std::chrono::high_resolution_clock;
      clk::time_point start = clk::now();
      auto orig_start = start;
      auto end = start;
      uint64_t iter_total = 0;
      int64_t ns_total = 0;
      int64_t ns_delta = 0;
      while (m_active)
      {
        iter_total++;
        // TODO condition variables for the sleeping instead
        // linux : https://stackoverflow.com/questions/24051863/how-to-implement-highly-accurate-timers-in-linux-userspace
        // win : https://stackoverflow.com/a/13413019/1495627
        // mac : https://stackoverflow.com/a/52905687/1495627
        // other: naive way
        auto time_to_sleep = std::chrono::microseconds(us_per_buffer);
        auto actual_next = start + time_to_sleep;
        auto now = start;
        auto elapsed = (now - orig_start);
        auto expected_next_elapsed = clk::duration(iter_total * us_per_buffer);
        auto delta = expected_next_elapsed - elapsed;
        double delta_p = (expected_next_elapsed.count() - elapsed.count() / 1000.);
        if(delta_p > 0)
        {
          std::this_thread::sleep_for(std::chrono::microseconds((int)delta_p));
        }
        end = clk::now();
        tick_start();
        if (stop_processing)
        {
          start = clk::now();
          tick_clear();
          continue;
        }

        auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
                      end - start)
                      .count();
        ns += ns_delta;
        ns_total += ns;

        ns_delta = std::chrono::duration_cast<std::chrono::nanoseconds>(end - orig_start).count() - ns_total;

        int samples = std::ceil(double(effective_sample_rate) * ns / 1e9);
        samples = std::min(samples, effective_buffer_size);
        if(samples < 0)
          samples = 0;

        ossia::audio_tick_state ts{nullptr, nullptr, 0, 0, (uint64_t)samples, ns_total / 1e9};
        audio_tick(ts);

        start = clk::now();
        tick_end();
      }
    }};
#if defined(__linux__)
    pthread_setname_np(m_runThread.native_handle(), "ossia execution");
#endif
  }

  ~dummy_engine() override
  {
    m_active = false;
    if (m_runThread.joinable())
      m_runThread.join();
  }

private:
  std::thread m_runThread;
};
}
