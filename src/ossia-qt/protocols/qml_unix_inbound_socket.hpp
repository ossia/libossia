#pragma once
#include <ossia/network/context.hpp>
#if defined(BOOST_ASIO_HAS_LOCAL_SOCKETS)
#include <ossia/detail/variant.hpp>
#include <ossia/network/sockets/configuration.hpp>
#include <ossia/network/sockets/cobs_framing.hpp>
#include <ossia/network/sockets/fixed_length_framing.hpp>
#include <ossia/network/sockets/line_framing.hpp>
#include <ossia/network/sockets/no_framing.hpp>
#include <ossia/network/sockets/size_prefix_framing.hpp>
#include <ossia/network/sockets/slip_framing.hpp>
#include <ossia/network/sockets/stx_etx_framing.hpp>
#include <ossia/network/sockets/unix_socket.hpp>
#include <ossia/network/sockets/var_size_prefix_framing.hpp>

#include <ossia-qt/protocols/utils.hpp>

#include <QJSValue>
#include <QObject>
#include <QQmlEngine>

#include <nano_observer.hpp>

#include <verdigris>

namespace ossia::qt
{
class qml_unix_datagram_inbound_socket
    : public QObject
    , public Nano::Observer
{
  W_OBJECT(qml_unix_datagram_inbound_socket)
public:
  struct state
  {
    ossia::net::unix_datagram_socket socket;
    std::atomic_bool alive{true};
    ossia::net::encoding enc{ossia::net::encoding::none};

    state(
        const ossia::net::fd_configuration& conf, boost::asio::io_context& ctx,
        ossia::net::encoding e = ossia::net::encoding::none)
        : socket{conf, ctx}
    {
      enc = e;
    }
  };

  qml_unix_datagram_inbound_socket() { }

  ~qml_unix_datagram_inbound_socket()
  {
    if(m_state)
    {
      m_state->alive = false;
      close();
    }
  }

  bool isOpen() const noexcept { return m_state != nullptr; }

  void open(
      const ossia::net::fd_configuration& conf, boost::asio::io_context& ctx,
      ossia::net::encoding e = ossia::net::encoding::none)
  {
    m_state = std::make_shared<state>(conf, ctx, e);

    if(onClose.isCallable())
      m_state->socket.on_close.connect<&qml_unix_datagram_inbound_socket::on_close>(this);

    m_state->socket.open();
    if(onOpen.isCallable())
      onOpen.call({qjsEngine(this)->newQObject(this)});

    auto st = m_state;
    auto self = QPointer{this};
    st->socket.receive([st, self](const char* data, std::size_t sz) {
      if(!st->alive)
        return;
      ossia::qt::run_async(
          self.get(),
          [self, arg = apply_decoding(st->enc, data, sz)] {
        if(!self.get())
          return;
        if(self->onMessage.isCallable())
        {
          self->onMessage.call({qjsEngine(self.get())->toScriptValue(arg)});
        }
      },
          Qt::AutoConnection);
    });
  }

  void close()
  {
    if(!m_state)
      return;
    if(!m_state->socket.m_socket.is_open())
      return;
    auto st = m_state;
    boost::asio::post(st->socket.m_context, [st] {
      try
      {
        st->socket.m_socket.shutdown(boost::asio::ip::udp::socket::shutdown_both);
      }
      catch(...)
      {
      }
      st->socket.m_socket.close();
      st->socket.on_close();
    });
  }
  W_SLOT(close)

  void on_close()
  {
    if(!m_state || !m_state->alive)
      return;
    ossia::qt::run_async(this, [=, this] { onClose.call(); }, Qt::AutoConnection);
  }

  QJSValue onOpen;
  QJSValue onClose;
  QJSValue onError;
  QJSValue onMessage;

private:
  std::shared_ptr<state> m_state;
};

class qml_unix_stream_connection
    : public QObject
    , public Nano::Observer
{
  W_OBJECT(qml_unix_stream_connection)
public:
  using socket_t = boost::asio::local::stream_protocol::socket;
  using decoder_type = ossia::slow_variant<
      ossia::net::no_framing::decoder<socket_t>,
      ossia::net::slip_decoder<socket_t>,
      ossia::net::size_prefix_decoder<socket_t>,
      ossia::net::line_framing_decoder<socket_t>,
      ossia::net::cobs_decoder<socket_t>,
      ossia::net::stx_etx_framing::decoder<socket_t>,
      ossia::net::size_prefix_1byte_framing::decoder<socket_t>,
      ossia::net::size_prefix_2byte_be_framing::decoder<socket_t>,
      ossia::net::size_prefix_2byte_le_framing::decoder<socket_t>,
      ossia::net::size_prefix_4byte_le_framing::decoder<socket_t>,
      ossia::net::fixed_length_decoder<socket_t>>;

  struct state
  {
    boost::asio::io_context& context;
    ossia::net::unix_stream_listener listener;
    std::atomic_bool alive{true};
    ossia::net::framing framing{ossia::net::framing::none};
    ossia::net::encoding enc{ossia::net::encoding::none};
    char line_delimiter[8] = {};
    decoder_type decoder;

    state(
        ossia::net::unix_stream_listener l, boost::asio::io_context& ctx,
        ossia::net::framing f = ossia::net::framing::none,
        const std::string& delim = {},
        ossia::net::encoding e = ossia::net::encoding::none)
        : context{ctx}
        , listener{std::move(l)}
        , decoder{ossia::in_place_index<0>, listener.m_socket}
    {
      framing = f;
      enc = e;
      if(!delim.empty())
      {
        auto sz = std::min(delim.size(), (size_t)7);
        std::copy_n(delim.begin(), sz, line_delimiter);
      }
      switch(f)
      {
        default:
        case ossia::net::framing::none:
          break;
        case ossia::net::framing::slip:
          decoder.template emplace<1>(listener.m_socket);
          break;
        case ossia::net::framing::size_prefix:
          decoder.template emplace<2>(listener.m_socket);
          break;
        case ossia::net::framing::line_delimiter:
          decoder.template emplace<3>(listener.m_socket);
          {
            auto& dec = ossia::get<3>(decoder);
            std::copy_n(line_delimiter, 8, dec.delimiter);
          }
          break;
        case ossia::net::framing::cobs:
          decoder.template emplace<4>(listener.m_socket);
          break;
        case ossia::net::framing::stx_etx:
          decoder.template emplace<5>(listener.m_socket);
          break;
        case ossia::net::framing::size_prefix_1byte:
          decoder.template emplace<6>(listener.m_socket);
          break;
        case ossia::net::framing::size_prefix_2byte_be:
          decoder.template emplace<7>(listener.m_socket);
          break;
        case ossia::net::framing::size_prefix_2byte_le:
          decoder.template emplace<8>(listener.m_socket);
          break;
        case ossia::net::framing::size_prefix_4byte_le:
          decoder.template emplace<9>(listener.m_socket);
          break;
        case ossia::net::framing::fixed_length:
          decoder.template emplace<10>(listener.m_socket);
          if(!delim.empty())
            ossia::get<10>(decoder).frame_size = std::stoul(delim);
          break;
      }
    }

    void write_encoded(const char* data, std::size_t sz)
    {
      switch(framing)
      {
        default:
        case ossia::net::framing::none:
          listener.write(boost::asio::const_buffer(data, sz));
          break;
        case ossia::net::framing::slip:
          ossia::net::slip_encoder<socket_t>{listener.m_socket}.write(data, sz);
          break;
        case ossia::net::framing::size_prefix:
          ossia::net::size_prefix_encoder<socket_t>{listener.m_socket}.write(data, sz);
          break;
        case ossia::net::framing::line_delimiter: {
          ossia::net::line_framing_encoder<socket_t> enc{listener.m_socket};
          std::copy_n(line_delimiter, 8, enc.delimiter);
          enc.write(data, sz);
          break;
        }
        case ossia::net::framing::cobs:
          ossia::net::cobs_encoder<socket_t>{listener.m_socket}.write(data, sz);
          break;
        case ossia::net::framing::stx_etx:
          ossia::net::stx_etx_framing::encoder<socket_t>{listener.m_socket}.write(
              data, sz);
          break;
        case ossia::net::framing::size_prefix_1byte:
          ossia::net::size_prefix_1byte_framing::encoder<socket_t>{listener.m_socket}
              .write(data, sz);
          break;
        case ossia::net::framing::size_prefix_2byte_be:
          ossia::net::size_prefix_2byte_be_framing::encoder<socket_t>{listener.m_socket}
              .write(data, sz);
          break;
        case ossia::net::framing::size_prefix_2byte_le:
          ossia::net::size_prefix_2byte_le_framing::encoder<socket_t>{listener.m_socket}
              .write(data, sz);
          break;
        case ossia::net::framing::size_prefix_4byte_le:
          ossia::net::size_prefix_4byte_le_framing::encoder<socket_t>{listener.m_socket}
              .write(data, sz);
          break;
        case ossia::net::framing::fixed_length:
          ossia::net::fixed_length_encoder<socket_t>{listener.m_socket}.write(data, sz);
          break;
      }
    }
  };

  struct receive_callback
  {
    std::shared_ptr<state> st;
    QPointer<qml_unix_stream_connection> self;

    void operator()(const unsigned char* data, std::size_t sz) const
    {
      if(!st->alive)
        return;
      auto buf = apply_decoding(st->enc, data, sz);
      ossia::qt::run_async(
          self.get(),
          [self = self, buf] {
        if(!self.get())
          return;
        if(self->onMessage.isCallable())
        {
          self->onMessage.call({qjsEngine(self.get())->toScriptValue(buf)});
        }
      },
          Qt::AutoConnection);
    }

    bool validate_stream(boost::system::error_code ec) const
    {
      if(ec == boost::asio::error::operation_aborted)
        return false;
      if(ec == boost::asio::error::eof)
        return false;
      return true;
    }
  };

  qml_unix_stream_connection(
      ossia::net::unix_stream_listener listener, boost::asio::io_context& ctx,
      ossia::net::framing f = ossia::net::framing::none,
      const std::string& delim = {},
      ossia::net::encoding e = ossia::net::encoding::none)
      : m_state{std::make_shared<state>(std::move(listener), ctx, f, delim, e)}
  {
  }

  ~qml_unix_stream_connection()
  {
    if(m_state)
    {
      m_state->alive = false;
      close({});
    }
  }

  bool isOpen() const noexcept { return m_state && m_state->alive; }

  inline boost::asio::io_context& context() noexcept { return m_state->context; }

  void write(QByteArray buffer)
  {
    if(!m_state)
      return;
    auto st = m_state;
    if(st->enc != ossia::net::encoding::none)
      buffer = apply_encoding(st->enc, buffer);
    boost::asio::dispatch(st->context, [st, buffer = std::move(buffer)] {
      if(st->alive)
        st->write_encoded(buffer.data(), buffer.size());
    });
  }
  W_SLOT(write)

  void close(QByteArray)
  {
    if(!m_state)
      return;
    auto st = m_state;
    boost::asio::dispatch(st->context, [st] { st->listener.close(); });
  }
  W_SLOT(close)

  void startReceive()
  {
    auto st = m_state;
    auto self = QPointer{this};
    ossia::visit(
        [cb = receive_callback{st, self}](auto& decoder) mutable {
      decoder.receive(std::move(cb));
    },
        st->decoder);
  }

  QJSValue onMessage;

private:
  std::shared_ptr<state> m_state;
};

class qml_unix_stream_inbound_socket
    : public QObject
    , public Nano::Observer
{
  W_OBJECT(qml_unix_stream_inbound_socket)
public:
  struct state
  {
    ossia::net::unix_stream_server server;
    std::atomic_bool alive{true};
    ossia::net::framing framing{ossia::net::framing::none};
    std::string framing_delimiter;
    ossia::net::encoding enc{ossia::net::encoding::none};

    state(
        const ossia::net::fd_configuration& conf, boost::asio::io_context& ctx,
        ossia::net::framing f = ossia::net::framing::none,
        std::string delim = {},
        ossia::net::encoding e = ossia::net::encoding::none)
        : server{conf, ctx}
        , framing{f}
        , framing_delimiter{std::move(delim)}
        , enc{e}
    {
    }
  };

  qml_unix_stream_inbound_socket() { }

  ~qml_unix_stream_inbound_socket()
  {
    if(m_state)
    {
      m_state->alive = false;
      close();
    }
  }

  bool isOpen() const noexcept { return m_state != nullptr; }

  void open(
      const ossia::net::fd_configuration& conf, boost::asio::io_context& ctx,
      ossia::net::framing f = ossia::net::framing::none,
      const std::string& delim = {},
      ossia::net::encoding e = ossia::net::encoding::none)
  {
    m_state = std::make_shared<state>(conf, ctx, f, delim, e);

    accept_impl(m_state, QPointer{this});
    if(onOpen.isCallable())
      onOpen.call({qjsEngine(this)->newQObject(this)});
  }

  void close()
  {
    if(!m_state)
      return;
    m_state->server.m_acceptor.close();
    if(onClose.isCallable())
      onClose.call();
  }
  W_SLOT(close)

  void on_close()
  {
    if(!m_state || !m_state->alive)
      return;
    ossia::qt::run_async(this, [=, this] { onClose.call(); }, Qt::AutoConnection);
  }

  QJSValue onOpen;
  QJSValue onClose;
  QJSValue onError;
  QJSValue onConnection;

private:
  static void accept_impl(
      std::shared_ptr<state> st, QPointer<qml_unix_stream_inbound_socket> self)
  {
    st->server.m_acceptor.async_accept(
        [self, st](
            boost::system::error_code ec,
            ossia::net::unix_stream_server::proto::socket socket) {
      if(!st->alive)
        return;
      if(!ec)
      {
        ossia::qt::run_async(
            self.get(),
            [self, st, socket = std::move(socket)]() mutable {
          if(!self.get())
            return;
          auto conn = new qml_unix_stream_connection{
              ossia::net::unix_stream_listener{std::move(socket)},
              st->server.m_context, st->framing, st->framing_delimiter, st->enc};
          conn->onMessage = self->onConnection;
          conn->startReceive();

          if(self->onConnection.isCallable())
          {
            self->onConnection.call(
                {qjsEngine(self.get())->newQObject(static_cast<QObject*>(conn))});
          }
        },
            Qt::AutoConnection);
        accept_impl(st, self);
      }
    });
  }

  std::shared_ptr<state> m_state;
};
}
#endif
