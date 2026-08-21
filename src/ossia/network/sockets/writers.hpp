#pragma once
#include <ossia/detail/config.hpp>

#include <boost/asio/buffer.hpp>
#include <boost/asio/error.hpp>

#include <cinttypes>
#include <memory>
#include <vector>

namespace ossia::net
{
//! Liveness token for the asio completion handlers of a socket.
/**
 * Closing a descriptor -- or destroying the object that owns it -- does not make
 * asio forget the operations that were pending on it. They are queued for
 * completion with operation_aborted and run on a later poll(), which is very
 * often after the owner is gone: a protocol object owns a socket, the user
 * deletes the protocol, and the io_context keeps running.
 *
 * The completion handlers all capture `this`, directly or through a
 * stream_processor that holds a reference to the socket so that it can call
 * on_fail() / on_close(); running them then is a use-after-free. Every such
 * handler therefore takes a weak reference to this token and gives up when it
 * has expired.
 *
 * Held *by value* in the object being tracked, so that the token dies exactly
 * when that object does -- which is also why it is not copyable: a copy would
 * hand out a token that outlives what it is meant to describe.
 */
struct lifetime_token
{
  lifetime_token() = default;
  lifetime_token(const lifetime_token&) = delete;
  lifetime_token(lifetime_token&&) = delete;
  lifetime_token& operator=(const lifetime_token&) = delete;
  lifetime_token& operator=(lifetime_token&&) = delete;

  [[nodiscard]] std::weak_ptr<void> watch() const noexcept { return m_token; }

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
