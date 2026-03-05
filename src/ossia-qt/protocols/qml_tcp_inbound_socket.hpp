#pragma once
#include <ossia/network/context.hpp>
#include <ossia/network/sockets/tcp_socket.hpp>

#include <ossia-qt/protocols/utils.hpp>

#include <QJSValue>
#include <QObject>
#include <QQmlEngine>

#include <nano_observer.hpp>

#include <verdigris>

namespace ossia::qt
{
class qml_tcp_connection
    : public QObject
    , public Nano::Observer
{
  W_OBJECT(qml_tcp_connection)
public:
  struct state
  {
    boost::asio::io_context& context;
    ossia::net::tcp_listener listener;
    char data[4096];
    std::atomic_bool alive{true};

    state(ossia::net::tcp_listener l, boost::asio::io_context& ctx)
        : context{ctx}
        , listener{std::move(l)}
    {
    }
  };

  explicit qml_tcp_connection(
      ossia::net::tcp_listener listener, boost::asio::io_context& ctx)
      : m_state{std::make_shared<state>(std::move(listener), ctx)}
  {
  }

  ~qml_tcp_connection() { m_state->alive = false; }
  inline boost::asio::io_context& context() noexcept { return m_state->context; }

  void write(QByteArray buffer)
  {
    auto st = m_state;
    boost::asio::dispatch(st->context, [st, buffer] {
      if(st->alive)
        st->listener.write(boost::asio::const_buffer(buffer.data(), buffer.size()));
    });
  }
  W_SLOT(write)

  void close(QByteArray)
  {
    auto st = m_state;
    boost::asio::dispatch(st->context, [st] {
      if(st->alive)
        st->listener.close();
    });
  }
  W_SLOT(close)

  void receive(QJSValue v)
  {
    onBytes = v;
    receive_impl(m_state, QPointer{this});
  }
  W_SLOT(receive)

  QJSValue onBytes;
  QJSValue onClose;
  W_PROPERTY(QJSValue, onClose W_MEMBER onClose);

private:
  static void receive_impl(
      std::shared_ptr<state> st, QPointer<qml_tcp_connection> self)
  {
    st->listener.m_socket.async_read_some(
        boost::asio::buffer(st->data, sizeof(st->data)),
        [self, st](boost::system::error_code ec, std::size_t bytes_transferred) {
      if(!st->alive)
        return;
      if(!ec)
      {
        auto buf = QByteArray(st->data, bytes_transferred);
        ossia::qt::run_async(
            self.get(),
            [self, buf] {
          if(!self.get())
            return;
          if(self->onBytes.isCallable())
          {
            auto engine = qjsEngine(self.get());
            if(engine)
              self->onBytes.call({engine->toScriptValue(buf)});
          }
        },
            Qt::AutoConnection);
        receive_impl(st, self);
      }
      else
      {
        ossia::qt::run_async(
            self.get(),
            [self] {
          if(self && self->onClose.isCallable())
            self->onClose.call();
        },
            Qt::AutoConnection);
      }
    });
  }

  std::shared_ptr<state> m_state;
};

class qml_tcp_inbound_socket
    : public QObject
    , public Nano::Observer
{
  W_OBJECT(qml_tcp_inbound_socket)
public:
  struct state
  {
    ossia::net::tcp_server server;
    std::atomic_bool alive{true};
    std::atomic_bool open{false};

    state(
        const ossia::net::inbound_socket_configuration& conf,
        boost::asio::io_context& ctx)
        : server{conf, ctx}
    {
    }
  };

  qml_tcp_inbound_socket(
      const ossia::net::inbound_socket_configuration& conf, boost::asio::io_context& ctx)
      : m_state{std::make_shared<state>(conf, ctx)}
  {
  }

  ~qml_tcp_inbound_socket() { m_state->alive = false; }

  inline boost::asio::io_context& context() noexcept { return m_state->server.m_context; }

  void open()
  {
    m_state->open = true;
    accept_impl(m_state, QPointer{this});
    if(onOpen.isCallable())
      onOpen.call({qjsEngine(this)->newQObject(this)});
  }

  void close()
  {
    m_state->open = false;
    m_state->server.m_acceptor.close();
    if(onClose.isCallable())
      onClose.call();
  }
  W_SLOT(close)

  void on_close()
  {
    m_state->open = false;
    ossia::qt::run_async(this, [=, this] { onClose.call(); }, Qt::AutoConnection);
  }

  QJSValue onOpen;
  QJSValue onClose;
  QJSValue onError;
  QJSValue onConnection;

private:
  static void accept_impl(
      std::shared_ptr<state> st, QPointer<qml_tcp_inbound_socket> self)
  {
    st->server.m_acceptor.async_accept(
        [self, st](
            boost::system::error_code ec, ossia::net::tcp_server::proto::socket socket) {
      if(!st->alive || !st->open)
        return;
      if(!ec)
      {
        ossia::qt::run_async(
            self.get(),
            [self, st, socket = std::move(socket)]() mutable {
          if(!self.get())
            return;
          auto conn = new qml_tcp_connection{
              ossia::net::tcp_listener{std::move(socket)}, st->server.m_context};

          if(self->onConnection.isCallable())
          {
            self->onConnection.call({qjsEngine(self.get())->newQObject(conn)});
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
