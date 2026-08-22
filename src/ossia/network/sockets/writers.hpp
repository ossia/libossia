#pragma once
#include <ossia/detail/config.hpp>

#include <boost/asio/buffer.hpp>
#include <boost/asio/error.hpp>

#include <cinttypes>
#include <memory>
#include <vector>

namespace ossia::net
{
/**
 * Liveness token for a socket's asio completion handlers.
 *
 * Closing a descriptor does not make asio forget the operations pending on it:
 * they complete with operation_aborted on a later poll(), often after the owner
 * is gone. Handlers capture `this`, so they take a weak reference to this token
 * and give up once it has expired.
 *
 * Held by value in the tracked object, hence non-copyable.
 */
struct lifetime_token
{
  lifetime_token() = default;
  lifetime_token(const lifetime_token&) = delete;
  lifetime_token(lifetime_token&&) = delete;
  lifetime_token& operator=(const lifetime_token&) = delete;
  lifetime_token& operator=(lifetime_token&&) = delete;

  [[nodiscard]] std::weak_ptr<void> watch() const noexcept { return m_token; }

  //! Expire every watch. Members die after the destructor body, so a socket
  //! closing itself there still looks alive unless this is called first.
  void reset() noexcept { m_token.reset(); }

private:
  std::shared_ptr<void> m_token{std::make_shared<char>()};
};

template <typename T>
struct socket_writer
{
  T& socket;
  void operator()(const char* data, std::size_t sz) const { socket.write(data, sz); }
};

template <typename Socket>
struct multi_socket_writer
{
  std::vector<Socket>& sockets;
  void write(const boost::asio::const_buffer& buf)
  {
    for(auto& sock : sockets)
    {
      sock->write(buf);
    }
  }
  void write_some(const boost::asio::const_buffer& buf)
  {
    for(auto& sock : sockets)
    {
      sock->write(buf);
    }
  }
};

template <typename T, typename F>
struct stream_processor
{
  T& self;
  F on_message;
  template <typename... Args>
  void operator()(Args&&... args) const
  {
    this->on_message(std::forward<Args>(args)...);
  }

  bool validate_stream(boost::system::error_code ec) const
  {
    if(ec == boost::asio::error::operation_aborted)
    {
      self.on_fail();
      return false;
    }

    if(ec == boost::asio::error::eof)
    {
      self.on_close();
      return false;
    }

    return true;
  }
};
}
