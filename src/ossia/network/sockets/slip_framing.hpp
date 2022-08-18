#pragma once
#include <ossia/detail/pod_vector.hpp>
#include <ossia/network/sockets/writers.hpp>

#include <boost/asio/error.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/streambuf.hpp>
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
  boost::asio::streambuf m_data;
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
  }

  template <typename F>
  void receive(F f)
  {
    socket.async_read_some(
        boost::asio::mutable_buffer(m_data.prepare(1024)),
        [this, f = std::move(f)](boost::system::error_code ec, std::size_t sz) mutable {
      if(!f.validate_stream(ec))
        return;

      if(sz > 0)
      {
        process_bytes(f, sz);
      }

      receive(std::move(f));
        });
  }

  template <typename F>
  void process_bytes(const F& f, std::size_t sz)
  {
    auto begin = (const uint8_t*)m_data.data().data();
    for(std::size_t i = 0; i < sz; i++)
    {
      const uint8_t next_char = *begin;
      process_byte(f, next_char);
      ++begin;
    }
    m_data.consume(sz);
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

  // This is tailored for OSC which uses double-ended encoding
  void write(const char* data, std::size_t sz)
  {
    this->write(socket, boost::asio::buffer(&slip::eot, 1));

    const uint8_t* begin = reinterpret_cast<const uint8_t*>(data);
    const uint8_t* end = begin + sz;
    while(begin < end)
    {
      std::size_t written = this->write(begin, end);
      begin += written;
    }
    this->write(socket, boost::asio::buffer(&slip::eot, 1));
  }

  std::size_t write(const uint8_t* begin, const uint8_t* end)
  {
    const uint8_t byte = *begin;
    switch(byte)
    {
      case slip::eot: {
        const uint8_t data[2] = {slip::esc, slip::esc_end};
        this->write(socket, boost::asio::buffer(data, 2));
        return 1;
      }
      case slip::esc: {
        const uint8_t data[2] = {slip::esc, slip::esc_esc};
        this->write(socket, boost::asio::buffer(data, 2));
        return 1;
      }
      default: {
        auto sub_end = begin + 1;
        while(sub_end != end && *sub_end != slip::eot && *sub_end != slip::esc)
          ++sub_end;

        this->write(socket, boost::asio::buffer(begin, sub_end - begin));
        return sub_end - begin;
      }
    }
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

struct slip_framing
{
  template <typename Socket>
  using encoder = slip_encoder<Socket>;
  template <typename Socket>
  using decoder = slip_decoder<Socket>;
};
}
