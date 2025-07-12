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
class qml_websocket_inbound_socket
    : public QObject
    , public Nano::Observer
{
  W_OBJECT(qml_websocket_inbound_socket)
public:
  qml_websocket_inbound_socket(
      const ossia::net::inbound_socket_configuration& conf, boost::asio::io_context& ctx)
      : m_context{ctx}
  {
    // FIXME
    // WebSocket server implementation simplified for now
  }
  inline boost::asio::io_context& context() noexcept { return m_context; }

  void open()
  {
    if(onOpen.isCallable())

      onOpen.call({qjsEngine(this)->newQObject(this)});
  }

  void close()
  {
    run_on_qt_thread({ onClose.call(); });
  }
  W_SLOT(close)

  boost::asio::io_context& m_context;

  QJSValue onOpen;
  QJSValue onClose;
  QJSValue onError;
  QJSValue onConnection;
  QJSValue onBytes;
  QJSValue onTextMessage;
  QJSValue onBinaryMessage;
};
}
