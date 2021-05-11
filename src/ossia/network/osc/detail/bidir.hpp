#pragma once
#include <ossia/network/osc/detail/tcp_socket.hpp>
#include <ossia/network/osc/detail/udp_socket.hpp>
#include <ossia/network/osc/detail/unix_socket.hpp>
#include <ossia/network/osc/detail/size_prefix_framing.hpp>
#include <ossia/network/osc/detail/slip_framing.hpp>
namespace ossia::net
{
template<typename Socket>
struct multi_socket_writer
{
  std::vector<Socket>& sockets;
  void write_some(const asio::ASIO_CONST_BUFFER& buf)
  {
    for(auto& sock : sockets)
    {
      sock->write(buf);
    }
  }
};

template<typename Base, typename Decode>
struct framed_listener : public Base
{
  using proto = typename Base::proto;
  using socket = typename proto::socket;

  template<typename F>
  framed_listener(socket&& sock, F f)
      : Base{std::move(sock)}
      , decoder{this->m_socket}
  {
    decoder.receive(std::move(f));
  }

  Decode decoder;
};

template<typename Base, typename Listener, typename Encode>
struct framed_server : Base
{
public:
  using proto = typename Base::proto;
  using socket = typename proto::socket;
  using listener = Listener;

  template<typename... Args>
  framed_server(Args&&... args)
      : Base{std::forward<Args>(args)...}
  {

  }

  template <typename F>
  void close(F f)
  {
    this->m_context.post([this, f] {
      for(auto& sock : m_sockets)
      {
        sock.close();
      }
      f();
    });
  }

  template <typename F>
  void listen(F f)
  {
    this->m_acceptor.async_accept(
        [this, f = std::move(f)](std::error_code ec, socket socket) {
          if (!ec)
          {
            m_sockets.push_back(std::make_unique<listener>(std::move(socket), f));
          }

          listen(std::move(f));
    });
  }

  void write(const char* data, std::size_t sz)
  {
    multi_socket_writer<std::unique_ptr<listener>> wr{this->m_sockets};
    Encode{wr}.write(data, sz);
  }

  std::vector<std::unique_ptr<listener>> m_sockets;
};

template<typename Base, typename Decode, typename Encode>
struct framed_client : Base
{
  template<typename... Args>
  framed_client(Args&&... args)
      : Base{std::forward<Args>(args)...}
      , decoder{this->m_socket}
  {

  }

  template <typename F>
  void receive(F f) {
    decoder.receive(std::move(f));
  }

  void write(const char* data, std::size_t sz)
  {
    Encode{this->m_socket}.write(data, sz);
  }

  Decode decoder;
};

template<typename Base, typename Decode, typename Encode>
struct framed_socket : Base
{
  template<typename... Args>
  framed_socket(Args&&... args)
      : Base{std::forward<Args>(args)...}
      , decoder{this->m_socket}
  {

  }

  template <typename F>
  void listen(F f)
  {
    Base::open();
    decoder.receive(std::move(f));
  }

  template <typename F>
  void receive(F f) {
    decoder.receive(std::move(f));
  }

  void write(const char* data, std::size_t sz)
  {
    Encode{this->m_socket}.write(data, sz);
  }

  Decode decoder;
};

struct tcp_slip_listener
    : framed_listener<tcp_listener, slip_decoder<asio::ip::tcp::socket>>
{
  using framed_listener::framed_listener;
};
struct tcp_size_prefix_listener
    : framed_listener<tcp_listener, size_prefix_decoder<asio::ip::tcp::socket>>
{
  using framed_listener::framed_listener;
};

struct tcp_size_prefix_server
    : framed_server<tcp_server, tcp_size_prefix_listener, size_prefix_encoder<multi_socket_writer<std::unique_ptr<tcp_size_prefix_listener>>>> {
  using framed_server::framed_server;
};
struct tcp_slip_server
    : framed_server<tcp_server, tcp_slip_listener, slip_encoder<multi_socket_writer<std::unique_ptr<tcp_slip_listener>>>> {
  using framed_server::framed_server;
};

struct tcp_size_prefix_client
    : framed_client<tcp_client, size_prefix_decoder<asio::ip::tcp::socket>, size_prefix_encoder<asio::ip::tcp::socket>> {
  using framed_client::framed_client;
};
struct tcp_slip_client
    : framed_client<tcp_client, slip_decoder<asio::ip::tcp::socket>, slip_encoder<asio::ip::tcp::socket>> {
  using framed_client::framed_client;
};

#if defined(ASIO_HAS_LOCAL_SOCKETS)
struct unix_stream_slip_listener
    : framed_listener<unix_stream_listener, slip_decoder<asio::local::stream_protocol::socket>>
{
  using framed_listener::framed_listener;
};
struct unix_stream_size_prefix_listener
    : framed_listener<unix_stream_listener, size_prefix_decoder<asio::local::stream_protocol::socket>>
{
  using framed_listener::framed_listener;
};

struct unix_stream_size_prefix_server
    : framed_server<unix_stream_server, unix_stream_size_prefix_listener, size_prefix_encoder<multi_socket_writer<std::unique_ptr<unix_stream_size_prefix_listener>>>> {
  using framed_server::framed_server;
};
struct unix_stream_slip_server
    : framed_server<unix_stream_server, unix_stream_slip_listener, slip_encoder<multi_socket_writer<std::unique_ptr<unix_stream_slip_listener>>>> {
  using framed_server::framed_server;
};

struct unix_stream_size_prefix_client
    : framed_socket<unix_stream_client, size_prefix_decoder<asio::local::stream_protocol::socket>, size_prefix_encoder<asio::local::stream_protocol::socket>> {
  using framed_socket::framed_socket;
};
struct unix_stream_slip_client
    : framed_socket<unix_stream_client, slip_decoder<asio::local::stream_protocol::socket>, slip_encoder<asio::local::stream_protocol::socket>> {
  using framed_socket::framed_socket;
};
#endif

template<typename T>
struct socket_writer
{
  T& socket;
  void operator()(const char* data, std::size_t sz) const
  {
    socket.write(data,sz);
  }
};
}
