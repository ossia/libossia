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
class qml_udp_inbound_socket
    : public QObject
    , public Nano::Observer
{
  W_OBJECT(qml_udp_inbound_socket)
public:
  qml_udp_inbound_socket(
      const ossia::net::inbound_socket_configuration& conf, boost::asio::io_context& ctx)
      : socket{conf, ctx}
  {
  }
  inline boost::asio::io_context& context() noexcept { return socket.m_context; }

  void open()
  {
    if(onClose.isCallable())
      socket.on_close.connect<&qml_udp_inbound_socket::on_close>(*this);

    socket.open();
    if(onOpen.isCallable())
      onOpen.call({qjsEngine(this)->newQObject(this)});

    socket.receive([this](const char* data, std::size_t sz) {
      ossia::qt::run_async(this, [this, arg = QByteArray(data, sz)] {
        if(onMessage.isCallable())
        {
          onMessage.call({qjsEngine(this)->toScriptValue(arg)});
        }
      }, Qt::AutoConnection);
    });
  }

  void on_close()
  {
    run_on_qt_thread({ onClose.call(); });
  }

  void close() { socket.close(); }
  W_SLOT(close)

  QJSValue onOpen;
  QJSValue onClose;
  QJSValue onError;
  QJSValue onMessage;

  ossia::net::udp_receive_socket socket;
};

}
