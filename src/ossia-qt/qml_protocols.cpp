#include "qml_protocols.hpp"

#include <ossia/network/sockets/serial_socket.hpp>
#include <ossia/network/sockets/tcp_socket.hpp>
#include <ossia/network/sockets/udp_socket.hpp>
#include <ossia/network/sockets/websocket_client.hpp>
#include <ossia/network/sockets/websocket_server.hpp>

#include <QJSValue>

#include <wobjectimpl.h>
W_OBJECT_IMPL(ossia::qt::qml_protocols)
namespace ossia::qt
{
class qml_udp_outbound_socket : public QObject
{
  W_OBJECT(qml_udp_outbound_socket)
public:
  qml_udp_outbound_socket(
      const ossia::net::outbound_socket_configuration& conf,
      boost::asio::io_context& ctx)
      : socket{conf, ctx}
  {
  }

  void open()
  {
    socket.connect();
    if(onOpen.isCallable())
      onOpen.call();
  }

  void write(QByteArray buffer) { socket.write(buffer.data(), buffer.size()); }
  W_SLOT(write)

  QJSValue onOpen;
  QJSValue onClose;
  QJSValue onError;

  ossia::net::udp_send_socket socket;
};
}

W_OBJECT_IMPL(ossia::qt::qml_udp_outbound_socket)
namespace ossia::qt
{

qml_protocols::qml_protocols(QObject* parent)
    : QObject{parent}
{
}
qml_protocols::~qml_protocols() { }

QObject* qml_protocols::outboundUDP(QVariant config)
{
  // var x = Protocols.outboundUDP({Transport: { Host: "127.0.0.1", Port: 1992 } })
  // x.write("/foo")
  auto conf = config.toMap();
  auto transport = conf["Transport"].toMap();
  QString host = transport["Host"].toString();
  QString port = transport["Port"].toString();
  bool broadcast = transport["Broadcast"].toBool();

  ossia::net::outbound_socket_configuration ossia_conf{
      .host = host.toStdString(),
      .port = (uint16_t)port.toInt(),
      .broadcast = broadcast};
  auto sock = new qml_udp_outbound_socket{ossia_conf, context};
  sock->onOpen = conf["onOpen"].value<QJSValue>();
  sock->onClose = conf["onClose"].value<QJSValue>();
  sock->onError = conf["onError"].value<QJSValue>();
  sock->open();
  // sock->onMessage = conf["onMessage"].value<QJSValue>().call();
  return sock;
}

QObject* qml_protocols::inboundUDP(QVariant config)
{ //TODO
  return nullptr;
}

QObject* qml_protocols::outboundUnixDatagram(QVariant config)
{ //TODO
  return nullptr;
}

QObject* qml_protocols::inboundUnixDatagram(QVariant config)
{ //TODO
  return nullptr;
}

QObject* qml_protocols::outboundUnixStream(QVariant config)
{ //TODO
  return nullptr;
}

QObject* qml_protocols::inboundUnixStream(QVariant config)
{ //TODO
  return nullptr;
}

QObject* qml_protocols::outboundTCP(QVariant config)
{ //TODO
  return nullptr;
}

QObject* qml_protocols::inboundTCP(QVariant config)
{ //TODO
  return nullptr;
}

QObject* qml_protocols::outboundWS(QVariant config)
{ //TODO
  return nullptr;
}

QObject* qml_protocols::inboundWS(QVariant config)
{ //TODO
  return nullptr;
}

QObject* qml_protocols::file(QVariant config)
{
  return nullptr;
}

QObject* qml_protocols::midi(QVariant config)
{
  return nullptr;
}

QObject* qml_protocols::serial(QVariant config)
{
  return nullptr;
}
}
