#pragma once
#include <ossia/audio/audio_protocol.hpp>
namespace ossia
{
class dummy_engine final : public audio_engine
{
  int m_rate{}, m_bs{};
  std::atomic_bool m_active;

public:
  dummy_engine(int rate, int bs)
  {
    m_rate = rate;
    m_bs = bs;

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

    int us_per_buffer = 1e6 * double(m_bs) / double(m_rate);
    m_runThread = std::thread{[=] {
      using clk = std::chrono::high_resolution_clock;
      clk::time_point start = clk::now();
      auto end = start;
      while (m_active)
      {
        std::this_thread::sleep_for(std::chrono::microseconds(us_per_buffer));
        end = clk::now();
        if (!stop_processing)
        {
          if (auto proto = protocol.load())
          {
            auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
                          end - start)
                          .count();
            int samples = std::ceil(double(m_rate) * ns / 1e9);
            proto->process_generic(*proto, nullptr, nullptr, 0, 0, samples);
            proto->audio_tick(m_bs, 0);

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
