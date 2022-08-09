#pragma once
#include <ossia/detail/logger.hpp>

#include <boost/asio/high_resolution_timer.hpp>

namespace ossia
{

class timer
{
public:
  explicit timer(boost::asio::io_context& ctx)
      : m_ctx{&ctx}
      , m_timer{ctx}
  {
  }

  timer(const timer&) = delete;
  timer(timer&&) = default;
  timer& operator=(const timer&) = delete;
  timer& operator=(timer&&) = default;

  ~timer()
  {
    stop();
  }

  void set_delay(std::chrono::milliseconds ms) noexcept
  {
    m_delay = ms;
  }

  template <typename F>
  void start(F f)
  {
    m_timer.expires_from_now(m_delay);
    m_timer.async_wait([this, ff = std::move(f)](auto ec) {
      if(ec)
      {
        ossia::logger().error("timer error: {}", ec.message());
        return;
      }

      ff();
      this->start(std::move(ff));
    });
  }

  void stop()
  {
    m_ctx->post([tm = std::make_shared<boost::asio::high_resolution_timer>(
                     std::move(m_timer))]() mutable { tm->cancel(); });
  }

private:
  boost::asio::io_context* m_ctx{};
  boost::asio::high_resolution_timer m_timer;
  std::chrono::milliseconds m_delay{};
};

}
