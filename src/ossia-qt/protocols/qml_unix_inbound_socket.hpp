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
  qml_unix_datagram_inbound_socket(
      const ossia::net::fd_configuration& conf, boost::asio::io_context& ctx)
      : socket{conf, ctx}
  {
  }
  inline boost::asio::io_context& context() noexcept { return socket.m_context; }

  void open()
  {
    if(onClose.isCallable())
      socket.on_close.connect<&qml_unix_datagram_inbound_socket::on_close>(this);

    socket.open();
    if(onOpen.isCallable())

      onOpen.call({qjsEngine(this)->newQObject(this)});

    socket.receive([self = QPointer{this}](const char* data, std::size_t sz) {
      ossia::qt::run_async(self.data(), [self, arg = QByteArray(data, sz)] {
        if(!self)
          return;
        if(self->onMessage.isCallable())
        {
          self->onMessage.call({qjsEngine(self)->toScriptValue(arg)});
        }
      }, Qt::AutoConnection);
    });
  }

  void close() { socket.close(); }
  W_SLOT(close)

  void on_close()
  {
    run_on_qt_thread({ onClose.call(); });
  }

  QJSValue onOpen;
  QJSValue onClose;
  QJSValue onError;
  QJSValue onMessage;

  ossia::net::unix_datagram_socket socket;
};

// FIXME apply same logic than tcp inbound
class qml_unix_stream_connection
    : public QObject
    , public Nano::Observer
{
  W_OBJECT(qml_unix_stream_connection)
public:
  qml_unix_stream_connection(
      ossia::net::unix_stream_listener listener, boost::asio::io_context& ctx)
      : m_context{ctx}
      , m_listener{std::move(listener)}
  {
  }
  inline boost::asio::io_context& context() noexcept { return m_context; }

  void write(QByteArray buffer)
  {
    run_on_asio_thread(
        { m_listener.write(boost::asio::buffer(buffer.data(), buffer.size())); });
  }
  W_SLOT(write)

  void startReceive()
  {
    auto& socket = m_listener.m_socket;
    socket.async_read_some(
        boost::asio::buffer(m_data, sizeof(m_data)),
        [self
         = QPointer{this}](boost::system::error_code ec, std::size_t bytes_transferred) {
      if(!ec)
      {
        ossia::qt::run_async(
            self.get(),
            [self, arg = QString::fromUtf8(self->m_data, bytes_transferred)] {
          if(!self)
            return;
          if(self->onMessage.isCallable())
          {
            self->onMessage.call({arg});
          }
        });
        self->startReceive();
      }
    });
  }

  QJSValue onMessage;

private:
  boost::asio::io_context& m_context;
  ossia::net::unix_stream_listener m_listener;
  char m_data[4096];
};

class qml_unix_stream_inbound_socket
    : public QObject
    , public Nano::Observer
{
  W_OBJECT(qml_unix_stream_inbound_socket)
public:
  qml_unix_stream_inbound_socket(
      const ossia::net::fd_configuration& conf, boost::asio::io_context& ctx)
      : server{conf, ctx}
  {
  }
  inline boost::asio::io_context& context() noexcept { return server.m_context; }

  void open()
  {
    accept();
    if(onOpen.isCallable())

      onOpen.call({qjsEngine(this)->newQObject(this)});
  }

  void close()
  {
    server.m_acceptor.close();
    if(onClose.isCallable())
      onClose.call();
  }
  W_SLOT(close)

  void on_close()
  {
    run_on_qt_thread({ onClose.call(); });
  }

  QJSValue onOpen;
  QJSValue onClose;
  QJSValue onError;
  QJSValue onConnection;

  ossia::net::unix_stream_server server;

private:
  void accept()
  {
    server.m_acceptor.async_accept(
        [self = QPointer{this}](
            boost::system::error_code ec,
            ossia::net::unix_stream_server::proto::socket socket) {
      if(!ec)
      {
        ossia::qt::run_async(self.get(), [self, socket = std::move(socket)]() mutable {
          auto conn = new qml_unix_stream_connection{
              ossia::net::unix_stream_listener{std::move(socket)},
              self->server.m_context};
          conn->onMessage = self->onConnection;
          conn->startReceive();

          if(self->onConnection.isCallable())
          {
            self->onConnection.call(
                {qjsEngine(self)->newQObject(static_cast<QObject*>(conn))});
          }
        });
        self->accept();
      }
    });
  }
};
}
#endif
