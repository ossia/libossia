#pragma once
#include <ossia/detail/pod_vector.hpp>
#include <ossia/network/sockets/writers.hpp>

#include <boost/asio/buffer.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/write.hpp>

namespace ossia::net
{

template <uint8_t Start = 0x02, uint8_t End = 0x03, uint8_t Escape = 0x10>
struct delimiter_framing
{
  static_assert(
      Start != End && Start != Escape && End != Escape,
      "start, end, and escape bytes must be distinct");

  template <typename Socket>
  struct decoder
  {
    Socket& socket;
    alignas(64) uint8_t m_readbuf[4096];
    ossia::pod_vector<char> m_decoded;
    enum
    {
      waiting_start,
      reading_data,
      reading_escape
    } m_status{waiting_start};

    explicit decoder(Socket& socket)
        : socket{socket}
    {
      m_decoded.reserve(1024);
    }

    template <typename F>
    void receive(F f)
    {
      socket.async_read_some(
          boost::asio::buffer(m_readbuf),
          [this,
           f = std::move(f)](boost::system::error_code ec, std::size_t sz) mutable {
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
        if(m_status == reading_data)
        {
          // Bulk scan for special bytes
          const uint8_t* run_end = ptr;
          while(run_end < end && *run_end != End && *run_end != Escape
                && *run_end != Start)
            ++run_end;

          if(run_end > ptr)
          {
            m_decoded.insert(
                m_decoded.end(), reinterpret_cast<const char*>(ptr),
                reinterpret_cast<const char*>(run_end));
            ptr = run_end;
          }

          if(ptr < end)
            process_byte(f, *ptr++);
        }
        else
        {
          process_byte(f, *ptr++);
        }
      }
    }

    template <typename F>
    void process_byte(const F& f, uint8_t byte)
    {
      switch(m_status)
      {
        case waiting_start:
          if(byte == Start)
          {
            m_status = reading_data;
            m_decoded.clear();
          }
          break;

        case reading_data:
          switch(byte)
          {
            case End:
              m_status = waiting_start;
              if(m_decoded.size() > 0)
                f((const unsigned char*)m_decoded.data(), m_decoded.size());
              m_decoded.clear();
              break;
            case Escape:
              m_status = reading_escape;
              break;
            case Start:
              // Unexpected start mid-frame: restart
              m_decoded.clear();
              break;
            default:
              m_decoded.push_back(byte);
              break;
          }
          break;

        case reading_escape:
          m_decoded.push_back(byte);
          m_status = reading_data;
          break;
      }
    }
  };

  template <typename Socket>
  struct encoder
  {
    Socket& socket;
    ossia::pod_vector<uint8_t> m_buf;

    void write(const char* data, std::size_t sz)
    {
      m_buf.clear();
      m_buf.reserve(sz * 2 + 2);
      m_buf.push_back(Start);

      auto* src = reinterpret_cast<const uint8_t*>(data);
      for(std::size_t i = 0; i < sz; ++i)
      {
        switch(src[i])
        {
          case Start:
          case End:
          case Escape:
            m_buf.push_back(Escape);
            m_buf.push_back(src[i]);
            break;
          default:
            m_buf.push_back(src[i]);
            break;
        }
      }
      m_buf.push_back(End);

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
};

// Standard STX/ETX with DLE escaping
using stx_etx_framing = delimiter_framing<0x02, 0x03, 0x10>;

}
