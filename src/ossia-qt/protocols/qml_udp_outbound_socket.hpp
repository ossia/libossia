#pragma once
#include <ossia/network/context.hpp>
#include <ossia/network/sockets/udp_socket.hpp>

#include <ossia-qt/protocols/utils.hpp>

#include <QJSValue>
#include <QObject>
#include <QQmlEngine>

#include <nano_observer.hpp>

#include <verdigris>

namespace ossia::qt
{

class qml_udp_outbound_socket
    : public QObject
    , public Nano::Observer
    , public protocols_sender
{
  W_OBJECT(qml_udp_outbound_socket)
public:
  qml_udp_outbound_socket(
      const ossia::net::outbound_socket_configuration& conf,
      boost::asio::io_context& ctx)
      : socket{conf, ctx}
  {
  }
  inline boost::asio::io_context& context() noexcept { return socket.m_context; }

  void open()
  {
    if(onClose.isCallable())
      socket.on_close.connect<&qml_udp_outbound_socket::on_close>(*this);

    socket.connect();

    if(onOpen.isCallable())
      onOpen.call({qjsEngine(this)->newQObject(this)});
  }

  void close() { socket.close(); }
  W_SLOT(close)

  void on_close()
  {
    run_on_qt_thread({ onClose.call(); });
  }

  void write(QByteArray buffer)
  {
    run_on_asio_thread({ socket.write(buffer.data(), buffer.size()); });
  }
  W_SLOT(write)

  void osc(QByteArray address, QJSValueList values) { this->send_osc(address, values); }
  W_SLOT(osc)

  QJSValue onOpen;
  QJSValue onClose;
  QJSValue onError;

  ossia::net::udp_send_socket socket;
};
}
