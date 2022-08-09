#pragma once
#include <ossia/detail/pod_vector.hpp>
#include <ossia/network/sockets/writers.hpp>

#include <boost/asio/buffer.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/write.hpp>
#include <boost/endian/conversion.hpp>

namespace ossia::net
{

template <typename Socket>
struct line_framing_decoder
{
  Socket& socket;
  char delimiter[8] = {0};
  int32_t m_next_packet_size{};
  std::vector<char, ossia::pod_allocator_avx2<char>> m_data;

  explicit line_framing_decoder(Socket& socket)
      : socket{socket}
  {
    m_data.reserve(65535);
  }

  template <typename F>
  void receive(F f)
  {
    m_data.clear();

    // Receive the size prefix
    boost::asio::async_read_until(
        socket, boost::asio::dynamic_buffer(m_data), (const char*)delimiter,
        [this, f = std::move(f)](boost::system::error_code ec, std::size_t sz) mutable {
      if(ec.failed())
        return;

      int new_sz = sz;
      new_sz -= strlen(delimiter);
      if(new_sz > 0)
        read_data(std::move(f), ec, new_sz);
      else
        this->receive(std::move(f));
        });
  }

  template <typename F>
  void read_data(F&& f, boost::system::error_code ec, std::size_t sz)
  {
    if(!f.validate_stream(ec))
      return;

    if(!ec && sz > 0)
    {
      try
      {
        f((const unsigned char*)m_data.data(), sz);
      }
      catch(...)
      {
      }
    }

    this->receive(std::move(f));
  }
};

template <typename Socket>
struct line_framing_encoder
{
  Socket& socket;
  char delimiter[8] = {0};

  void write(const char* data, std::size_t sz)
  {
    this->write(socket, boost::asio::buffer(data, sz));
    this->write(socket, boost::asio::buffer(delimiter, strlen(delimiter)));
  }

  template <typename T>
  void write(T& sock, const boost::asio::const_buffer& buf)
  {
    boost::asio::write(sock, buf);
  }

  template <typename T>
  void write(multi_socket_writer<T>& sock, const boost::asio::const_buffer& buf)
  {
    sock.write(buf);
  }
};

struct line_framing
{
  template <typename Socket>
  using encoder = line_framing_encoder<Socket>;
  template <typename Socket>
  using decoder = line_framing_decoder<Socket>;
};

}
