#pragma once
#include <asio/high_resolution_timer.hpp>

namespace ossia
{

class timer
{
public:
  explicit timer(asio::io_context& ctx)
    : m_timer{ctx}
  {

  }

  ~timer()
  {
    stop();
  }

  void set_delay(std::chrono::milliseconds ms) noexcept
  {
    m_delay = ms;
  }

  template<typename F>
  void start(F f)
  {
    m_timer.expires_from_now(m_delay);
    m_timer.async_wait([this, ff = std::move(f)] (asio::error_code ec) {
      if(ec)
        return;

      ff();
      this->start(std::move(ff));
    });
  }

  void stop()
  {
    m_timer.cancel();
  }

private:
  asio::high_resolution_timer m_timer;
  std::chrono::milliseconds m_delay{};
};

}
