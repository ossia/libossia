#pragma once
#include <ossia/detail/pod_vector.hpp>
#include <ossia/network/sockets/writers.hpp>

#include <boost/asio/buffer.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>

namespace ossia::net
{

template <typename Socket>
struct fixed_length_decoder
{
  Socket& socket;
  std::size_t frame_size{64};
  ossia::pod_vector<char> m_data;

  explicit fixed_length_decoder(Socket& socket)
      : socket{socket}
  {
  }

  template <typename F>
  void receive(F f)
  {
    m_data.resize(frame_size, boost::container::default_init);
    boost::asio::async_read(
        socket, boost::asio::mutable_buffer(m_data.data(), frame_size),
        boost::asio::transfer_exactly(frame_size),
        [this, f = std::move(f)](boost::system::error_code ec, std::size_t sz) mutable {
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
        });
  }
};

template <typename Socket>
struct fixed_length_encoder
{
  Socket& socket;

  void write(const char* data, std::size_t sz)
  {
    this->do_write(socket, boost::asio::buffer(data, sz));
  }

  template <typename T>
  void do_write(T& sock, const boost::asio::const_buffer& buf)
  {
    boost::asio::write(sock, buf);
  }

  template <typename T>
  void do_write(multi_socket_writer<T>& sock, const boost::asio::const_buffer& buf)
  {
    sock.write(buf);
  }
};

struct fixed_length_framing
{
  template <typename Socket>
  using encoder = fixed_length_encoder<Socket>;
  template <typename Socket>
  using decoder = fixed_length_decoder<Socket>;
};

}
