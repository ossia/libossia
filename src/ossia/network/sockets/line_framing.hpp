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

#include <algorithm>
#include <cstring>
#include <memory>
#include <string_view>

namespace ossia::net
{

//! Store a line-framing delimiter in a fixed-size, NUL-terminated buffer.
//! Scanned with strnlen(), so at most N - 1 bytes fit; longer delimiters are
//! truncated and the tail zeroed so a shorter one inherits nothing.
template <std::size_t N>
inline std::size_t set_line_delimiter(char (&buffer)[N], std::string_view delim) noexcept
{
  static_assert(N >= 1);
  const std::size_t sz = std::min(delim.size(), N - 1);
  std::copy_n(delim.data(), sz, buffer);
  std::fill(buffer + sz, buffer + N, '\0');
  return sz;
}

template <typename Socket>
struct line_framing_decoder
{
  using buffer_type = std::vector<char, ossia::pod_allocator_avx2<char>>;

  Socket& socket;
  char delimiter[8] = {0};
  int32_t m_next_packet_size{};

  //! Shared, unlike the read buffers of the other decoders.
  /**
   * async_read_until is a *composed* operation: it holds the dynamic_buffer --
   * which is only a reference to this vector -- across several reads, and it
   * shrinks it back to size on completion, before our handler gets a chance to
   * run and notice that the socket is gone. Owning the buffer through a
   * shared_ptr that the handler keeps a copy of is what makes that final shrink
   * land on live memory; the lifetime token alone cannot help here, as by the
   * time it is checked the damage is done.
   */
  std::shared_ptr<buffer_type> m_data{std::make_shared<buffer_type>()};
  uint8_t m_delimiter_len = 0;
  lifetime_token m_lifetime;

  explicit line_framing_decoder(Socket& socket)
      : socket{socket}
  {
    m_data->reserve(65535);
  }

  void set_delimiter(std::string_view delim) noexcept
  {
    m_delimiter_len = uint8_t(set_line_delimiter(delimiter, delim));
  }

  template <typename F>
  void receive(F f)
  {
    if(m_delimiter_len == 0)
      m_delimiter_len = strnlen(delimiter, 8);
    m_data->clear();

    // Receive until delimiter
    boost::asio::async_read_until(
        socket, boost::asio::dynamic_buffer(*m_data), (const char*)delimiter,
        [this, alive = m_lifetime.watch(), buf = m_data,
         f = std::move(f)](boost::system::error_code ec, std::size_t sz) mutable {
      // The socket may be gone since this read was armed; see lifetime_token.
      // `buf` is held only to keep the read buffer alive that long.
      if(alive.expired())
        return;

      if(ec.failed())
        return;

      int new_sz = sz;
      new_sz -= m_delimiter_len;
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
        f((const unsigned char*)m_data->data(), sz);
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
  uint8_t delimiter_len = 0;

  void set_delimiter(std::string_view delim) noexcept
  {
    delimiter_len = uint8_t(set_line_delimiter(delimiter, delim));
  }

  void write(const char* data, std::size_t sz)
  {
    if(delimiter_len == 0)
      delimiter_len = strnlen(delimiter, 8);

    // Scatter-gather: data + delimiter in single write
    std::array<boost::asio::const_buffer, 2> bufs = {
        boost::asio::buffer(data, sz),
        boost::asio::buffer(delimiter, delimiter_len)};
    this->do_write(socket, bufs);
  }

  // Regular socket: scatter-gather (single syscall)
  template <typename T, std::size_t N>
  void do_write(T& sock, const std::array<boost::asio::const_buffer, N>& bufs)
  {
    boost::asio::write(sock, bufs);
  }

  // Multi socket: write each buffer to each socket
  template <typename T, std::size_t N>
  void do_write(
      multi_socket_writer<T>& sock,
      const std::array<boost::asio::const_buffer, N>& bufs)
  {
    for(const auto& buf : bufs)
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
