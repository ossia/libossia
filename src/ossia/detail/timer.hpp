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
  template <typename Executor>
  explicit timer(boost::asio::strand<Executor>& ctx)
      : m_timer{ctx}
  {
  }

  explicit timer(boost::asio::io_context& ctx)
      : m_timer{boost::asio::make_strand(ctx)}
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
      if(ec == boost::asio::error::operation_aborted)
        return;
      else if(ec)
      {
        ossia::logger().error("timer error: {}", ec.message());
        return;
      }
      else
      {
        ff();
        this->start(std::move(ff));
      }
    });
  }

  void stop()
  {
    boost::asio::dispatch(
        m_timer.get_executor(), [tm = std::make_shared<boost::asio::steady_timer>(
                                     std::move(m_timer))]() mutable { tm->cancel(); });
    std::future<void> wait
        = boost::asio::dispatch(m_timer.get_executor(), boost::asio::use_future);
    wait.get();
  }

private:
  boost::asio::steady_timer m_timer;
  std::chrono::milliseconds m_delay{};
};

}
