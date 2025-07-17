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
class qml_tcp_outbound_socket
    : public QObject
    , public Nano::Observer
    , public protocols_sender
{
  W_OBJECT(qml_tcp_outbound_socket)
public:
  qml_tcp_outbound_socket(
      const ossia::net::outbound_socket_configuration& conf,
      boost::asio::io_context& ctx)
      : socket{conf, ctx}
  {
  }
  inline boost::asio::io_context& context() noexcept { return socket.m_context; }

  void open()
  {
    try
    {
      if(onOpen.isCallable())
        socket.on_open.connect<&qml_tcp_outbound_socket::on_open>(this);
      if(onClose.isCallable())
        socket.on_close.connect<&qml_tcp_outbound_socket::on_close>(this);
      if(onError.isCallable())
        socket.on_fail.connect<&qml_tcp_outbound_socket::on_fail>(this);
      socket.connect();
    }
    catch(const std::exception& e)
    {
      if(onError.isCallable())
      {
        onError.call({QString::fromStdString(e.what())});
      }
    }
  }

  void write(QByteArray buffer)
  {
    run_on_asio_thread({ socket.write(buffer.data(), buffer.size()); });
  }
  W_SLOT(write)

  void close() { socket.close(); }
  W_SLOT(close)

  void on_open()
  {
    run_on_qt_thread({ onOpen.call({qjsEngine(this)->newQObject(this)}); });
  }
  void on_fail()
  {
    run_on_qt_thread({ onError.call(); });
  }
  void on_close()
  {
    run_on_qt_thread({ onClose.call(); });
  }

  void osc(QByteArray address, QJSValueList values) { this->send_osc(address, values); }
  W_SLOT(osc)

  QJSValue onOpen;
  QJSValue onClose;
  QJSValue onError;
  QJSValue onBytes;

  ossia::net::tcp_client socket;
};

}
