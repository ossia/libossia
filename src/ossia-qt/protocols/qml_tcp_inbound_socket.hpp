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
  explicit qml_tcp_connection(
      ossia::net::tcp_listener listener, boost::asio::io_context& ctx)
      : m_context{ctx}
      , m_listener{std::move(listener)}
  {
  }
  inline boost::asio::io_context& context() noexcept { return m_context; }

  void write(QByteArray buffer)
  {
    run_on_asio_thread(
        { m_listener.write(boost::asio::const_buffer(buffer.data(), buffer.size())); });
  }
  W_SLOT(write)

  void close(QByteArray buffer)
  {
    run_on_asio_thread({ m_listener.close(); });
  }
  W_SLOT(close)

  void receive(QJSValue v)
  {
    onBytes = v;
    do_receive();
  }
  W_SLOT(receive)
  void do_receive()
  {
    auto& socket = m_listener.m_socket;
    socket.async_read_some(
        boost::asio::buffer(m_data, sizeof(m_data)),
        [self
         = QPointer{this}](boost::system::error_code ec, std::size_t bytes_transferred) {
      if(!ec)
      {
        auto buf = QByteArray(self->m_data, bytes_transferred);
        ossia::qt::run_async(self.get(), [self, buf] {
          if(!self.get())
            return;
          if(self->onBytes.isCallable())
          {
            self->onBytes.call({qjsEngine(self.get())->toScriptValue(buf)});
          }
        }, Qt::AutoConnection);
        self->do_receive();
      }
      else
      {
        ossia::qt::run_async(self.get(), [self] {
          if(self && self->onClose.isCallable())
            self->onClose.call();
        });
      }
    });
  }

  QJSValue onBytes;
  QJSValue onClose;
  W_PROPERTY(QJSValue, onClose W_MEMBER onClose);

private:
  boost::asio::io_context& m_context;
  ossia::net::tcp_listener m_listener;
  char m_data[4096];
};

class qml_tcp_inbound_socket
    : public QObject
    , public Nano::Observer
{
  W_OBJECT(qml_tcp_inbound_socket)
public:
  qml_tcp_inbound_socket(
      const ossia::net::inbound_socket_configuration& conf, boost::asio::io_context& ctx)
      : server{conf, ctx}
  {
  }

  inline boost::asio::io_context& context() noexcept { return server.m_context; }

  void open()
  {
    m_open = true;
    accept();
    if(onOpen.isCallable())
      onOpen.call({qjsEngine(this)->newQObject(this)});
  }

  void close()
  {
    m_open = false;
    server.m_acceptor.close();
    if(onClose.isCallable())
      onClose.call();
  }
  W_SLOT(close)

  void on_close()
  {
    m_open = false;
    run_on_qt_thread({ onClose.call(); });
  }

  QJSValue onOpen;
  QJSValue onClose;
  QJSValue onError;
  QJSValue onConnection;
  ossia::net::tcp_server server;

private:
  void accept()
  {
    server.m_acceptor.async_accept(
        [self = QPointer{this}](
            boost::system::error_code ec, ossia::net::tcp_server::proto::socket socket) {
      if(!self->m_open)
        return;
      if(!ec)
      {
        ossia::qt::run_async(self.get(), [self, socket = std::move(socket)]() mutable {
          auto conn = new qml_tcp_connection{
              ossia::net::tcp_listener{std::move(socket)}, self->server.m_context};

          if(self->onConnection.isCallable())
          {
            self->onConnection.call({qjsEngine(self.get())->newQObject(conn)});
          }
        });
        self->accept();
      }
    });
  }

  std::atomic_bool m_open{};
};

}
