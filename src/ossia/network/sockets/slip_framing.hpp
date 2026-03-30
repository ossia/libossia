#pragma once
#include <ossia/detail/pod_vector.hpp>
#include <ossia/network/sockets/writers.hpp>

#include <boost/asio/buffer.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/write.hpp>

namespace ossia::net
{
struct slip
{
  static const constexpr uint8_t eot = 192;
  static const constexpr uint8_t esc = 219;
  static const constexpr uint8_t esc_end = 220;
  static const constexpr uint8_t esc_esc = 221;
};

template <typename Socket>
struct slip_decoder
{
  Socket& socket;
  alignas(64) uint8_t m_readbuf[4096];
  ossia::pod_vector<char> m_decoded;
  enum
  {
    waiting,
    reading_char,
    reading_esc
  } m_status{waiting};

  explicit slip_decoder(Socket& socket)
      : socket{socket}
  {
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
      if(m_status == reading_char)
      {
        // Scan for next special byte
        const uint8_t* run_end = ptr;
        while(run_end < end && *run_end != slip::eot && *run_end != slip::esc)
          ++run_end;

        // Bulk append normal bytes
        if(run_end > ptr)
        {
          m_decoded.insert(
              m_decoded.end(), reinterpret_cast<const char*>(ptr),
              reinterpret_cast<const char*>(run_end));
          ptr = run_end;
        }

        // Process the special byte if any
        if(ptr < end)
          process_byte(f, *ptr++);
      }
      else
      {
        // waiting or reading_esc: one byte at a time
        process_byte(f, *ptr++);
      }
    }
  }

  template <typename F>
  void process_byte(const F& f, const uint8_t next_char)
  {
    switch(m_status)
    {
      case waiting: {
        switch(next_char)
        {
          // Start of a message
          case slip::eot:
            m_status = reading_char;
            break;

          // Any other character than eot is an error, do nothing.
          default:
            m_decoded.clear();
            break;
        }
        break;
      }

      case reading_char: {
        switch(next_char)
        {
          // End of a message, we can process it
          case slip::eot:
            m_status = waiting;
            if(m_decoded.size() > 0)
            {
              f((const unsigned char*)m_decoded.data(), m_decoded.size());
            }
            m_decoded.clear();
            break;

          // Start of an escape sequence
          case slip::esc:
            m_status = reading_esc;
            break;

          // Normal characters
          default:
            m_decoded.push_back(next_char);
            break;
        }
        break;
      }

      case reading_esc: {
        switch(next_char)
        {
          case slip::esc_end:
            m_status = reading_char;
            m_decoded.push_back(slip::eot);
            break;

          case slip::esc_esc:
            m_status = reading_char;
            m_decoded.push_back(slip::esc);
            break;

          // Invalid case, reset the decoder
          default:
            m_status = waiting;
            m_decoded.clear();
            break;
        }

        break;
      }
    }
  }
};

template <typename Socket>
struct slip_encoder
{
  Socket& socket;
  ossia::pod_vector<uint8_t> m_buf;

  // Encodes entire SLIP frame into a buffer, then writes once.
  void write(const char* data, std::size_t sz)
  {
    m_buf.clear();
    m_buf.reserve(sz * 2 + 2);
    m_buf.push_back(slip::eot);

    auto* src = reinterpret_cast<const uint8_t*>(data);
    for(std::size_t i = 0; i < sz; ++i)
    {
      switch(src[i])
      {
        case slip::eot:
          m_buf.push_back(slip::esc);
          m_buf.push_back(slip::esc_end);
          break;
        case slip::esc:
          m_buf.push_back(slip::esc);
          m_buf.push_back(slip::esc_esc);
          break;
        default:
          m_buf.push_back(src[i]);
          break;
      }
    }
    m_buf.push_back(slip::eot);

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

struct slip_framing
{
  template <typename Socket>
  using encoder = slip_encoder<Socket>;
  template <typename Socket>
  using decoder = slip_decoder<Socket>;
};
}
