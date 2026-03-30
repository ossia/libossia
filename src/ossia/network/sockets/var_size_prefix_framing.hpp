#pragma once
#include <ossia/detail/pod_vector.hpp>
#include <ossia/network/sockets/writers.hpp>

#include <boost/asio/buffer.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>
#include <boost/endian/conversion.hpp>

#include <array>
#include <type_traits>

namespace ossia::net
{

enum class byte_order
{
  big_endian,
  little_endian
};

template <int HeaderBytes, byte_order Order = byte_order::big_endian>
struct var_size_prefix_framing
{
  static_assert(HeaderBytes == 1 || HeaderBytes == 2 || HeaderBytes == 4);

  using header_type = std::conditional_t<
      HeaderBytes == 1, uint8_t, std::conditional_t<HeaderBytes == 2, uint16_t, uint32_t>>;

  template <typename Socket>
  struct decoder
  {
    Socket& socket;
    header_type m_header{};
    ossia::pod_vector<char> m_data;
    std::size_t max_frame_size
        = HeaderBytes == 1   ? std::size_t(255)
          : HeaderBytes == 2 ? std::size_t(65535)
                             : std::size_t(0x7FFFFFFF);

    explicit decoder(Socket& socket)
        : socket{socket}
    {
      if constexpr(HeaderBytes == 1)
        m_data.reserve(256);
      else
        m_data.reserve(65535);
    }

    template <typename F>
    void receive(F f)
    {
      boost::asio::async_read(
          socket, boost::asio::mutable_buffer(&m_header, sizeof(header_type)),
          boost::asio::transfer_exactly(sizeof(header_type)),
          [this,
           f = std::move(f)](boost::system::error_code ec, std::size_t sz) mutable {
        read_size(std::move(f), ec, sz);
          });
    }

    template <typename F>
    void read_size(F&& f, boost::system::error_code ec, std::size_t sz)
    {
      if(!f.validate_stream(ec))
        return;

      if constexpr(HeaderBytes > 1)
      {
        if constexpr(Order == byte_order::big_endian)
          boost::endian::big_to_native_inplace(m_header);
        else
          boost::endian::little_to_native_inplace(m_header);
      }

      auto packet_size = static_cast<std::size_t>(m_header);
      if(packet_size == 0 || packet_size > max_frame_size)
        return;

      m_data.resize(packet_size, boost::container::default_init);
      boost::asio::async_read(
          socket, boost::asio::mutable_buffer(m_data.data(), packet_size),
          boost::asio::transfer_exactly(packet_size),
          [this,
           f = std::move(f)](boost::system::error_code ec, std::size_t sz) mutable {
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
  struct encoder
  {
    Socket& socket;

    void write(const char* data, std::size_t sz)
    {
      header_type header = static_cast<header_type>(sz);
      if constexpr(HeaderBytes > 1)
      {
        if constexpr(Order == byte_order::big_endian)
          boost::endian::native_to_big_inplace(header);
        else
          boost::endian::native_to_little_inplace(header);
      }

      std::array<boost::asio::const_buffer, 2> bufs = {
          boost::asio::buffer(
              reinterpret_cast<const char*>(&header), sizeof(header_type)),
          boost::asio::buffer(data, sz)};
      this->do_write(socket, bufs);
    }

    template <typename T, std::size_t N>
    void do_write(T& sock, const std::array<boost::asio::const_buffer, N>& bufs)
    {
      boost::asio::write(sock, bufs);
    }

    template <typename T, std::size_t N>
    void do_write(
        multi_socket_writer<T>& sock,
        const std::array<boost::asio::const_buffer, N>& bufs)
    {
      for(const auto& buf : bufs)
        sock.write(buf);
    }
  };
};

// Common configurations
using size_prefix_1byte_framing = var_size_prefix_framing<1>;
using size_prefix_2byte_be_framing = var_size_prefix_framing<2, byte_order::big_endian>;
using size_prefix_2byte_le_framing = var_size_prefix_framing<2, byte_order::little_endian>;
using size_prefix_4byte_be_framing = var_size_prefix_framing<4, byte_order::big_endian>;
using size_prefix_4byte_le_framing = var_size_prefix_framing<4, byte_order::little_endian>;

}
