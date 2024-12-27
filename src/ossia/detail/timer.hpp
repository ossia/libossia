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
  explicit timer(boost::asio::strand<Executor>& ctx, boost::asio::io_context& ioctx)
      : m_timer{ctx}
      , m_context{ioctx}
  {
  }

  explicit timer(boost::asio::io_context& ctx)
      : m_timer{boost::asio::make_strand(ctx)}
      , m_context{ctx}
  {
  }

  timer(const timer&) = delete;
  timer(timer&& other) noexcept
      : m_timer{std::move(other.m_timer)}
      , m_context{other.m_context}
      , m_delay{other.m_delay}
  {
  }

  timer& operator=(const timer&) = delete;
  timer& operator=(timer&& other) noexcept
  {
    m_timer = std::move(other.m_timer);
    m_delay = other.m_delay;
    return *this;
  }

  ~timer() { stop(); }

  void set_delay(std::chrono::milliseconds ms) noexcept { m_delay = ms; }
  void set_delay(std::chrono::microseconds ms) noexcept
  {
    m_delay = std::chrono::duration_cast<std::chrono::milliseconds>(ms);
  }

  template <typename F>
  void start(F f)
  {
    m_timer.expires_after(m_delay);
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
    auto exec = m_timer.get_executor();
    boost::asio::dispatch(
        exec, [tm = std::make_shared<boost::asio::steady_timer>(
                   std::move(m_timer))]() mutable { tm->cancel(); });

    if(!m_context.stopped())
    {
      std::future<void> wait = boost::asio::dispatch(exec, boost::asio::use_future);
      wait.wait_for(std::chrono::seconds(2));
    }
  }

private:
  boost::asio::steady_timer m_timer;
  boost::asio::io_context& m_context;
  std::chrono::milliseconds m_delay{};
};

}
