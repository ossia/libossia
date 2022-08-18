#pragma once
#include <ossia/network/sockets/writers.hpp>

#include <boost/asio/buffer.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/write.hpp>

namespace ossia::net
{

struct no_framing
{
  template <typename Socket>
  struct encoder
  {
    Socket& socket;
    void write(const char* data, std::size_t sz)
    {
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

  template <typename Socket>
  struct decoder
  {
    Socket& socket;
    boost::asio::streambuf m_data;

    explicit decoder(Socket& s)
        : socket{s}
    {
    }

    decoder() = delete;
    decoder(const decoder&) = delete;
    decoder(decoder&&) = delete;
    decoder& operator=(const decoder&) = delete;
    decoder& operator=(decoder&&) = delete;
    template <typename F>
    void receive(F f)
    {
      socket.async_read_some(
          boost::asio::mutable_buffer(m_data.prepare(1024)),
          [this,
           f = std::move(f)](boost::system::error_code ec, std::size_t sz) mutable {
        if(!f.validate_stream(ec))
          return;

        auto data = (const uint8_t*)m_data.data().data();
        f(data, sz);
        receive(std::move(f));
          });
    }
  };
};

}
