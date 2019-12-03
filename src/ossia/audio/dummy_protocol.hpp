#pragma once
#include <ossia/audio/audio_protocol.hpp>
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

    setup_thread();
  }

  bool running() const override
  {
    return !m_runThread.joinable();
  }
  void setup_thread()
  {
    m_active = false;
    if (m_runThread.joinable())
      m_runThread.join();
    m_active = true;

    int us_per_buffer = 1e6 * double(effective_buffer_size) / double(effective_sample_rate);

    m_runThread = std::thread{[=] {
      using clk = std::chrono::high_resolution_clock;
      clk::time_point start = clk::now();
      auto orig_start = start;
      auto end = start;
      int64_t ns_total = 0;
      int64_t ns_delta = 0;
      while (m_active)
      {
        // TODO condition variables for the sleeping instead
        // linux : https://stackoverflow.com/questions/24051863/how-to-implement-highly-accurate-timers-in-linux-userspace
        // win : https://stackoverflow.com/a/13413019/1495627
        // mac : https://stackoverflow.com/a/52905687/1495627
        // other: naive way
        std::this_thread::sleep_for(std::chrono::microseconds(us_per_buffer));
        end = clk::now();
        if (!stop_processing)
        {
          if (auto proto = protocol.load())
          {
            auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
                          end - start)
                          .count();
            ns += ns_delta;
            ns_total += ns;

            ns_delta = std::chrono::duration_cast<std::chrono::nanoseconds>(end - orig_start).count() - ns_total;

            int samples = std::ceil(double(effective_sample_rate) * ns / 1e9);
            proto->process_generic(*proto, nullptr, nullptr, 0, 0, samples);
            proto->audio_tick(effective_buffer_size, 0);

            // Run a tick
            if (proto->replace_tick)
            {
              proto->audio_tick = std::move(proto->ui_tick);
              proto->ui_tick = {};
              proto->replace_tick = false;
            }
          }
        }
        start = clk::now();
      }
    }};
  }

  ~dummy_engine() override
  {
    m_active = false;
    protocol = nullptr;

    while (processing)
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    if (m_runThread.joinable())
      m_runThread.join();
  }

  void reload(audio_protocol* p) override
  {
    if (this->protocol)
      this->protocol.load()->engine = nullptr;
    stop();

    this->protocol = p;
    if (!p)
      return;

    auto& proto = *p;
    proto.engine = this;

    proto.setup_tree(0, 0);

    stop_processing = false;
    setup_thread();
  }

  void stop() override
  {
    stop_processing = true;
  }

private:
  std::thread m_runThread;
};
}
