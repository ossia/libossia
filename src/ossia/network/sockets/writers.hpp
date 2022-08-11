#pragma once
#include <ossia/detail/config.hpp>

#include <boost/asio/buffer.hpp>
#include <boost/asio/error.hpp>

#include <cinttypes>
#include <vector>

namespace ossia::net
{
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
