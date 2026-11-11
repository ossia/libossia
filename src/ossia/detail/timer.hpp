#pragma once
#include <ossia/detail/logger.hpp>

#include <boost/asio/any_io_executor.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/use_future.hpp>

#include <atomic>
#include <memory>

namespace ossia
{

//! Calls a function periodically on a strand.
//!
//! Once stop() returns, the function is not called any more: stop() raises a
//! flag the handler checks before calling the function and before re-arming,
//! cancels the wait on the strand, and, while the context runs, waits until the
//! strand is past a call in progress. The cancel has to run on the strand: a
//! handler running there re-arms the asio timer, and a cancel from another
//! thread would race with it and miss the new wait.
//! The asio timer and the flag live in a state the handlers share, so a handler
//! that runs late, or after the ossia::timer moved, never reaches freed memory.
class timer
{
public:
  template <typename Executor>
  explicit timer(boost::asio::strand<Executor>& ctx, boost::asio::io_context& ioctx)
      : m_executor{ctx}
      , m_context{ioctx}
      , m_state{std::make_shared<state>(m_executor)}
  {
  }

  explicit timer(boost::asio::io_context& ctx)
      : m_executor{boost::asio::make_strand(ctx)}
      , m_context{ctx}
      , m_state{std::make_shared<state>(m_executor)}
  {
  }

  timer(const timer&) = delete;
  timer(timer&& other) noexcept
      : m_executor{other.m_executor}
      , m_context{other.m_context}
      , m_state{std::move(other.m_state)}
      , m_delay{other.m_delay}
  {
  }

  timer& operator=(const timer&) = delete;
  timer& operator=(timer&& other) noexcept
  {
    stop();
    m_executor = other.m_executor;
    m_state = std::move(other.m_state);
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
    // Handlers of a stopped run may still be queued: they keep the old state.
    if(!m_state || m_state->stopped)
      m_state = std::make_shared<state>(m_executor);
    arm(m_state, m_delay, std::move(f));
  }

  void stop()
  {
    if(!m_state)
      return;
    m_state->stopped = true;

    boost::asio::dispatch(m_executor, [st = m_state] { st->timer.cancel(); });

    if(!m_context.stopped())
    {
      std::future<void> wait
          = boost::asio::dispatch(m_executor, boost::asio::use_future);
      wait.wait_for(std::chrono::seconds(2));
    }
  }

private:
  struct state
  {
    explicit state(const boost::asio::any_io_executor& exec)
        : timer{exec}
    {
    }

    boost::asio::steady_timer timer;
    std::atomic_bool stopped{false};
  };

  template <typename F>
  static void arm(std::shared_ptr<state> st, std::chrono::milliseconds delay, F f)
  {
    auto& t = st->timer;
    t.expires_after(delay);
    t.async_wait([st = std::move(st), delay, ff = std::move(f)](auto ec) mutable {
      if(ec == boost::asio::error::operation_aborted || st->stopped)
        return;
      else if(ec)
      {
        ossia::logger().error("timer error: {}", ec.message());
        return;
      }

      ff();
      if(!st->stopped)
        arm(std::move(st), delay, std::move(ff));
    });
  }

  boost::asio::any_io_executor m_executor;
  boost::asio::io_context& m_context;
  std::shared_ptr<state> m_state;
  std::chrono::milliseconds m_delay{};
};

}
