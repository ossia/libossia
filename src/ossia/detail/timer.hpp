#pragma once
#include <ossia/detail/logger.hpp>

#include <boost/asio/steady_timer.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/use_future.hpp>

namespace ossia
{

class timer
{
public:
  explicit timer(boost::asio::io_context& ctx)
      : m_ctx{&ctx}
      , m_timer{boost::asio::make_strand(ctx)}
  {
  }

  timer(const timer&) = delete;
  timer(timer&&) = default;
  timer& operator=(const timer&) = delete;
  timer& operator=(timer&&) = default;

  ~timer() { stop(); }

  void set_delay(std::chrono::milliseconds ms) noexcept { m_delay = ms; }

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
    std::future<void> wait
        = boost::asio::post(m_timer.get_executor(), boost::asio::use_future);
    m_ctx->post([tm = std::make_shared<boost::asio::steady_timer>(
                     std::move(m_timer))]() mutable { tm->cancel(); });
    wait.get();
  }

private:
  boost::asio::io_context* m_ctx{};
  boost::asio::steady_timer m_timer;
  std::chrono::milliseconds m_delay{};
};

}
