#pragma once
#include <ossia/network/context.hpp>
#if defined(BOOST_ASIO_HAS_LOCAL_SOCKETS)
#include <ossia/network/sockets/unix_socket.hpp>

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

    state(const ossia::net::fd_configuration& conf, boost::asio::io_context& ctx)
        : socket{conf, ctx}
    {
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

  void open(const ossia::net::fd_configuration& conf, boost::asio::io_context& ctx)
  {
    m_state = std::make_shared<state>(conf, ctx);

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
          [self, arg = QByteArray(data, sz)] {
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
  struct state
  {
    boost::asio::io_context& context;
    ossia::net::unix_stream_listener listener;
    char data[4096];
    std::atomic_bool alive{true};

    state(ossia::net::unix_stream_listener l, boost::asio::io_context& ctx)
        : context{ctx}
        , listener{std::move(l)}
    {
    }
  };

  qml_unix_stream_connection(
      ossia::net::unix_stream_listener listener, boost::asio::io_context& ctx)
      : m_state{std::make_shared<state>(std::move(listener), ctx)}
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
    auto st = m_state;
    boost::asio::dispatch(st->context, [st, buffer] {
      if(st->alive)
        st->listener.write(boost::asio::buffer(buffer.data(), buffer.size()));
    });
  }
  W_SLOT(write)

  void close(QByteArray)
  {
    auto st = m_state;
    boost::asio::dispatch(st->context, [st] { st->listener.close(); });
  }
  W_SLOT(close)

  void startReceive() { receive_impl(m_state, QPointer{this}); }

  QJSValue onMessage;

private:
  static void receive_impl(
      std::shared_ptr<state> st, QPointer<qml_unix_stream_connection> self)
  {
    st->listener.m_socket.async_read_some(
        boost::asio::buffer(st->data, sizeof(st->data)),
        [self, st](boost::system::error_code ec, std::size_t bytes_transferred) {
      if(!st->alive)
        return;
      if(!ec)
      {
        ossia::qt::run_async(
            self.get(),
            [self, arg = QString::fromUtf8(st->data, bytes_transferred)] {
          if(!self.get())
            return;
          if(self->onMessage.isCallable())
          {
            self->onMessage.call({arg});
          }
        },
            Qt::AutoConnection);
        receive_impl(st, self);
      }
    });
  }

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

    state(const ossia::net::fd_configuration& conf, boost::asio::io_context& ctx)
        : server{conf, ctx}
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

  void open(const ossia::net::fd_configuration& conf, boost::asio::io_context& ctx)
  {
    m_state = std::make_shared<state>(conf, ctx);

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
              st->server.m_context};
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
