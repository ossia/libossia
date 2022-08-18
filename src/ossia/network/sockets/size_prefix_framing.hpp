#pragma once
#include <ossia/detail/pod_vector.hpp>
#include <ossia/network/sockets/writers.hpp>

#include <boost/asio/buffer.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/write.hpp>
#include <boost/endian/conversion.hpp>

namespace ossia::net
{

template <typename Socket>
struct size_prefix_decoder
{
  Socket& socket;
  int32_t m_next_packet_size{};
  ossia::pod_vector<char> m_data;

  explicit size_prefix_decoder(Socket& socket)
      : socket{socket}
  {
    m_data.reserve(65535);
  }

  template <typename F>
  void receive(F f)
  {
    // Receive the size prefix
    socket.async_read_some(
        boost::asio::mutable_buffer(&m_next_packet_size, sizeof(int32_t)),
        [this, f = std::move(f)](boost::system::error_code ec, std::size_t sz) mutable {
      read_size(std::move(f), ec, sz);
        });
  }

  template <typename F>
  void read_size(F&& f, boost::system::error_code ec, std::size_t sz)
  {
    if(!f.validate_stream(ec))
      return;

    boost::endian::big_to_native_inplace(m_next_packet_size);
    if(m_next_packet_size <= 0)
      return;

    m_data.resize(m_next_packet_size);
    boost::asio::async_read(
        socket, boost::asio::mutable_buffer(m_data.data(), m_next_packet_size),
        boost::asio::transfer_exactly(m_next_packet_size),
        [this, f = std::move(f)](boost::system::error_code ec, std::size_t sz) mutable {
      read_data(std::move(f), ec, sz);
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
struct size_prefix_encoder
{
  Socket& socket;

  void write(const char* data, std::size_t sz)
  {
    int32_t packet_size = sz;
    boost::endian::native_to_big_inplace(packet_size);
    this->write(
        socket, boost::asio::buffer(
                    reinterpret_cast<const char*>(&packet_size), sizeof(int32_t)));
    this->write(socket, boost::asio::buffer(data, sz));
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

struct size_prefix_framing
{
  template <typename Socket>
  using encoder = size_prefix_encoder<Socket>;
  template <typename Socket>
  using decoder = size_prefix_decoder<Socket>;
};

}
