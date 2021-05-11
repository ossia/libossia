#pragma once
#include <ossia/detail/config.hpp>

#include <boost/asio/buffer.hpp>

#include <cinttypes>
#include <vector>

namespace ossia::net
{
template<typename T>
struct socket_writer
{
  T& socket;
  void operator()(const char* data, std::size_t sz) const
  {
    socket.write(data,sz);
  }
};

template<typename Socket>
struct multi_socket_writer
{
  std::vector<Socket>& sockets;
  void write_some(const boost::asio::const_buffer& buf)
  {
    for(auto& sock : sockets)
    {
      sock->write(buf);
    }
  }
};
}
