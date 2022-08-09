#pragma once
#include <ossia/network/sockets/size_prefix_framing.hpp>
#include <ossia/network/sockets/slip_framing.hpp>
#include <ossia/network/sockets/tcp_socket.hpp>
#include <ossia/network/sockets/udp_socket.hpp>
#include <ossia/network/sockets/unix_socket.hpp>
#include <ossia/network/sockets/writers.hpp>

namespace ossia::net
{

template <typename Base, typename Framing>
struct framed_listener : public Base
{
  using proto = typename Base::proto;
  using socket = typename proto::socket;
  using decoder = typename Framing::template decoder<socket>;

  template <typename F>
  framed_listener(socket&& sock, F f)
      : Base{std::move(sock)}
      , m_decoder{this->m_socket}
  {
    m_decoder.receive(stream_processor<framed_listener, F>{*this, std::move(f)});
  }

  decoder m_decoder;
};

template <typename Base, typename Framing>
struct framed_server : Base
{
public:
  using proto = typename Base::proto;
  using socket = typename proto::socket;
  using listener = framed_listener<typename Base::listener, Framing>;
  using encoder =
      typename Framing::template encoder<multi_socket_writer<std::unique_ptr<listener>>>;
  using decoder = typename Framing::template decoder<socket>;

  template <typename... Args>
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
      if(!ec)
      {
        m_sockets.push_back(std::make_unique<listener>(std::move(socket), f));
      }

      listen(std::move(f));
    });
  }

  void write(const char* data, std::size_t sz)
  {
    multi_socket_writer<std::unique_ptr<listener>> wr{this->m_sockets};
    encoder{wr}.write(data, sz);
  }

  std::vector<std::unique_ptr<listener>> m_sockets;
};

template <typename Base, typename Framing>
struct framed_client : Base
{
  using socket = typename Base::socket;
  using encoder = typename Framing::template encoder<socket>;
  using decoder = typename Framing::template decoder<socket>;

  template <typename... Args>
  framed_client(Args&&... args)
      : Base{std::forward<Args>(args)...}
      , m_decoder{this->m_socket}
  {
  }

  template <typename F>
  void receive(F f)
  {
    m_decoder.receive(stream_processor<framed_client, F>{*this, std::move(f)});
  }

  void write(const char* data, std::size_t sz)
  {
    encoder{this->m_socket}.write(data, sz);
  }

  decoder m_decoder;
};

template <typename Base, typename Framing>
struct framed_socket : Base
{
  using socket = typename Base::socket;
  using encoder = typename Framing::template encoder<socket>;
  using decoder = typename Framing::template decoder<socket>;

  template <typename... Args>
  framed_socket(Args&&... args)
      : Base{std::forward<Args>(args)...}
      , m_decoder{this->m_socket}
  {
  }

  template <typename F>
  void listen(F f)
  {
    Base::open();
    m_decoder.receive(stream_processor<framed_socket, F>{*this, std::move(f)});
  }

  template <typename F>
  void receive(F f)
  {
    m_decoder.receive(stream_processor<framed_socket, F>{*this, std::move(f)});
  }

  void write(const char* data, std::size_t sz)
  {
    encoder{this->m_socket}.write(data, sz);
  }

  decoder m_decoder;
};

struct tcp_size_prefix_server : framed_server<tcp_server, size_prefix_framing>
{
  using framed_server::framed_server;
};
struct tcp_slip_server : framed_server<tcp_server, slip_framing>
{
  using framed_server::framed_server;
};

struct tcp_size_prefix_client : framed_client<tcp_client, size_prefix_framing>
{
  using framed_client::framed_client;
};
struct tcp_slip_client : framed_client<tcp_client, slip_framing>
{
  using framed_client::framed_client;
};

#if defined(BOOST_ASIO_HAS_LOCAL_SOCKETS)
struct unix_stream_size_prefix_server
    : framed_server<unix_stream_server, size_prefix_framing>
{
  using framed_server::framed_server;
};
struct unix_stream_slip_server : framed_server<unix_stream_server, slip_framing>
{
  using framed_server::framed_server;
};

struct unix_stream_size_prefix_client
    : framed_socket<unix_stream_client, size_prefix_framing>
{
  using framed_socket::framed_socket;
};
struct unix_stream_slip_client : framed_socket<unix_stream_client, slip_framing>
{
  using framed_socket::framed_socket;
};
#endif

}
