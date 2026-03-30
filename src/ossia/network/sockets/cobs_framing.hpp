#pragma once
#include <ossia/detail/pod_vector.hpp>
#include <ossia/network/sockets/writers.hpp>

#include <boost/asio/buffer.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/write.hpp>

namespace ossia::net
{

template <typename Socket>
struct cobs_decoder
{
  Socket& socket;
  alignas(64) uint8_t m_readbuf[4096];
  ossia::pod_vector<uint8_t> m_encoded;
  ossia::pod_vector<char> m_decoded;

  explicit cobs_decoder(Socket& socket)
      : socket{socket}
  {
    m_encoded.reserve(1024);
    m_decoded.reserve(1024);
  }

  template <typename F>
  void receive(F f)
  {
    socket.async_read_some(
        boost::asio::buffer(m_readbuf),
        [this, f = std::move(f)](boost::system::error_code ec, std::size_t sz) mutable {
      if(!f.validate_stream(ec))
        return;

      if(sz > 0)
        process_bytes(f, m_readbuf, sz);

      receive(std::move(f));
        });
  }

  template <typename F>
  void process_bytes(const F& f, const uint8_t* data, std::size_t sz)
  {
    const uint8_t* ptr = data;
    const uint8_t* end = data + sz;

    while(ptr < end)
    {
      // Scan for next 0x00 delimiter
      const uint8_t* delim = ptr;
      while(delim < end && *delim != 0x00)
        ++delim;

      // Bulk append non-delimiter bytes
      if(delim > ptr)
        m_encoded.insert(m_encoded.end(), ptr, delim);

      ptr = delim;

      // Process delimiter
      if(ptr < end)
      {
        ++ptr; // consume the 0x00
        if(!m_encoded.empty())
        {
          if(decode_frame())
            f((const unsigned char*)m_decoded.data(), m_decoded.size());
          m_encoded.clear();
        }
      }
    }
  }

  bool decode_frame()
  {
    m_decoded.clear();
    const uint8_t* ptr = m_encoded.data();
    const uint8_t* end = ptr + m_encoded.size();

    while(ptr < end)
    {
      uint8_t code = *ptr++;
      if(code == 0)
        return false;

      uint8_t count = code - 1;
      if(ptr + count > end)
        return false;

      for(uint8_t j = 0; j < count; ++j)
        m_decoded.push_back(static_cast<char>(*ptr++));

      // If code < 0xFF, a zero byte was removed here (unless at end of packet)
      if(code < 0xFF && ptr < end)
        m_decoded.push_back(0);
    }

    return m_decoded.size() > 0;
  }
};

template <typename Socket>
struct cobs_encoder
{
  Socket& socket;
  ossia::pod_vector<uint8_t> m_buf;

  void write(const char* data, std::size_t sz)
  {
    m_buf.clear();
    m_buf.reserve(sz + sz / 254 + 3);

    const auto* src = reinterpret_cast<const uint8_t*>(data);

    std::size_t code_idx = m_buf.size();
    m_buf.push_back(0); // placeholder for first code byte
    uint8_t code = 1;

    for(std::size_t i = 0; i < sz; ++i)
    {
      if(src[i] == 0)
      {
        m_buf[code_idx] = code;
        code_idx = m_buf.size();
        m_buf.push_back(0);
        code = 1;
      }
      else
      {
        m_buf.push_back(src[i]);
        ++code;
        if(code == 0xFF)
        {
          m_buf[code_idx] = code;
          code_idx = m_buf.size();
          m_buf.push_back(0);
          code = 1;
        }
      }
    }

    m_buf[code_idx] = code;
    m_buf.push_back(0x00); // frame delimiter

    this->do_write(socket, boost::asio::buffer(m_buf.data(), m_buf.size()));
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

struct cobs_framing
{
  template <typename Socket>
  using encoder = cobs_encoder<Socket>;
  template <typename Socket>
  using decoder = cobs_decoder<Socket>;
};

}
