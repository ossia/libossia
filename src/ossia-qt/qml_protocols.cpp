#include "qml_protocols.hpp"

#include <ossia-qt/protocols/qml_http_request.hpp>
#include <ossia-qt/protocols/qml_tcp_inbound_socket.hpp>
#include <ossia-qt/protocols/qml_tcp_outbound_socket.hpp>
#include <ossia-qt/protocols/qml_udp_inbound_socket.hpp>
#include <ossia-qt/protocols/qml_udp_outbound_socket.hpp>
#include <ossia-qt/protocols/qml_unix_inbound_socket.hpp>
#include <ossia-qt/protocols/qml_unix_outbound_socket.hpp>
#include <ossia-qt/protocols/qml_ws_inbound_socket.hpp>
#include <ossia-qt/protocols/qml_ws_outbound_socket.hpp>

#include <boost/asio/io_context.hpp>

#include <QJSValue>
#include <QJSValueList>
#include <QQmlEngine>

#include <libremidi/libremidi.hpp>

#include <wobjectimpl.h>
/**
 * Examples of usage
 *

 var sock = Protocols.outboundUDP({Transport: { Host: "127.0.0.1", Port: 1234 } });
 sock.write("/foo\0\0\0\0,\0\0\0");

 var sock = Protocols.inboundUDP({
    Transport: { Bind: "127.0.0.1", Port: 1234 }
  , onMessage: function(bytes) { console.log(bytes); }
 });

 var sock = Protocols.outboundTCP({
   Transport: { Host: "127.0.0.1", Port: 1234 },
   onOpen: function(socket) {
    console.log("open!");
    socket.write("hello world");
   },
   onClose: function() { console.log("close!"); },
   onFail: function() { console.log("fail!"); },
 });

 var sock = Protocols.inboundTCP({
   Transport: { Host: "127.0.0.1", Port: 1234 },
   onOpen: function() {
    console.log("open!");
   },
   onConnection: function(socket) {
    console.log("connection!", socket);
    socket.onClose = function() { console.log("socket.onClose"); }
    socket.receive(function(b) { console.log(" => ", b); });
   },
   onClose: function() { console.log("close!"); },
   onFail: function() { console.log("fail!"); },
 });

  Protocols.http("http://127.0.0.1:1234/toto", function(x){console.log(x);},"GET");
 */
W_OBJECT_IMPL(ossia::qt::qml_protocols)
namespace ossia::qt
{

#if defined(BOOST_ASIO_HAS_LOCAL_SOCKETS)
#endif
}

W_OBJECT_IMPL(ossia::qt::qml_udp_outbound_socket)
W_OBJECT_IMPL(ossia::qt::qml_udp_inbound_socket)
W_OBJECT_IMPL(ossia::qt::qml_tcp_outbound_socket)
W_OBJECT_IMPL(ossia::qt::qml_tcp_connection)
W_OBJECT_IMPL(ossia::qt::qml_tcp_inbound_socket)
W_OBJECT_IMPL(ossia::qt::qml_websocket_outbound_socket)
W_OBJECT_IMPL(ossia::qt::qml_websocket_inbound_socket)
#if defined(BOOST_ASIO_HAS_LOCAL_SOCKETS)
W_OBJECT_IMPL(ossia::qt::qml_unix_datagram_outbound_socket)
W_OBJECT_IMPL(ossia::qt::qml_unix_datagram_inbound_socket)
W_OBJECT_IMPL(ossia::qt::qml_unix_stream_outbound_socket)
W_OBJECT_IMPL(ossia::qt::qml_unix_stream_connection)
W_OBJECT_IMPL(ossia::qt::qml_unix_stream_inbound_socket)
#endif

namespace ossia::qt
{
qml_protocols::qml_protocols(ossia::net::network_context_ptr ctx, QObject* parent)
    : QObject{parent}
    , context{ctx}
{
}

qml_protocols::~qml_protocols() { }

QObject* qml_protocols::outboundUDP(QVariant config)
{
  auto conf = config.toMap();
  auto transport = conf["Transport"].toMap();
  QString host = transport["Host"].toString();
  QString port = transport["Port"].toString();
  bool broadcast = transport["Broadcast"].toBool();

  ossia::net::outbound_socket_configuration ossia_conf{
      .host = host.toStdString(),
      .port = (uint16_t)port.toInt(),
      .broadcast = broadcast};
  auto sock = new qml_udp_outbound_socket{ossia_conf, context->context};
  // Doing this ensures that we can call qjsEngine(the_object) afterwards in open() callbacks:
  qjsEngine(this)->newQObject(sock);
  sock->onOpen = conf["onOpen"].value<QJSValue>();
  sock->onClose = conf["onClose"].value<QJSValue>();
  sock->onError = conf["onError"].value<QJSValue>();
  sock->open();
  return sock;
}

QObject* qml_protocols::inboundUDP(QVariant config)
{
  auto conf = config.toMap();
  auto transport = conf["Transport"].toMap();
  QString bind = transport["Bind"].toString();
  if(bind.isEmpty())
    bind = "0.0.0.0";
  QString port = transport["Port"].toString();

  ossia::net::inbound_socket_configuration ossia_conf{
      .bind = bind.toStdString(),
      .port = (uint16_t)port.toInt()};
  auto sock = new qml_udp_inbound_socket{ossia_conf, context->context};
  qjsEngine(this)->newQObject(sock);
  sock->onOpen = conf["onOpen"].value<QJSValue>();
  sock->onClose = conf["onClose"].value<QJSValue>();
  sock->onError = conf["onError"].value<QJSValue>();
  sock->onMessage = conf["onMessage"].value<QJSValue>();
  sock->open();
  return sock;
}

QObject* qml_protocols::outboundUnixDatagram(QVariant config)
{
#if defined(BOOST_ASIO_HAS_LOCAL_SOCKETS)
  auto conf = config.toMap();
  auto transport = conf["Transport"].toMap();
  QString path = transport["Path"].toString();

  ossia::net::fd_configuration ossia_conf{
      .fd = path.toStdString()};
  auto sock = new qml_unix_datagram_outbound_socket{ossia_conf, context->context};
  qjsEngine(this)->newQObject(sock);
  sock->onOpen = conf["onOpen"].value<QJSValue>();
  sock->onClose = conf["onClose"].value<QJSValue>();
  sock->onError = conf["onError"].value<QJSValue>();
  sock->open();
  return sock;
#else
  return nullptr;
#endif
}

QObject* qml_protocols::inboundUnixDatagram(QVariant config)
{
#if defined(BOOST_ASIO_HAS_LOCAL_SOCKETS)
  auto conf = config.toMap();
  auto transport = conf["Transport"].toMap();
  QString path = transport["Path"].toString();

  ossia::net::fd_configuration ossia_conf{
      .fd = path.toStdString()};
  auto sock = new qml_unix_datagram_inbound_socket{ossia_conf, context->context};
  qjsEngine(this)->newQObject(sock);
  sock->onOpen = conf["onOpen"].value<QJSValue>();
  sock->onClose = conf["onClose"].value<QJSValue>();
  sock->onError = conf["onError"].value<QJSValue>();
  sock->onMessage = conf["onMessage"].value<QJSValue>();
  sock->open();
  return sock;
#else
  return nullptr;
#endif
}

QObject* qml_protocols::outboundUnixStream(QVariant config)
{
#if defined(BOOST_ASIO_HAS_LOCAL_SOCKETS)
  auto conf = config.toMap();
  auto transport = conf["Transport"].toMap();
  QString path = transport["Path"].toString();

  ossia::net::fd_configuration ossia_conf{
      .fd = path.toStdString()};
  auto sock = new qml_unix_stream_outbound_socket{ossia_conf, context->context};
  qjsEngine(this)->newQObject(sock);
  sock->onOpen = conf["onOpen"].value<QJSValue>();
  sock->onClose = conf["onClose"].value<QJSValue>();
  sock->onError = conf["onError"].value<QJSValue>();
  sock->open();
  return sock;
#else
  return nullptr;
#endif
}

QObject* qml_protocols::inboundUnixStream(QVariant config)
{
#if defined(BOOST_ASIO_HAS_LOCAL_SOCKETS)
  auto conf = config.toMap();
  auto transport = conf["Transport"].toMap();
  QString path = transport["Path"].toString();

  ossia::net::fd_configuration ossia_conf{
      .fd = path.toStdString()};
  auto sock = new qml_unix_stream_inbound_socket{ossia_conf, context->context};
  qjsEngine(this)->newQObject(sock);
  sock->onOpen = conf["onOpen"].value<QJSValue>();
  sock->onClose = conf["onClose"].value<QJSValue>();
  sock->onError = conf["onError"].value<QJSValue>();
  sock->onConnection = conf["onConnection"].value<QJSValue>();
  sock->open();
  return sock;
#else
  return nullptr;
#endif
}

QObject* qml_protocols::outboundTCP(QVariant config)
{
  auto conf = config.toMap();
  auto transport = conf["Transport"].toMap();
  QString host = transport["Host"].toString();
  QString port = transport["Port"].toString();

  ossia::net::outbound_socket_configuration ossia_conf{
      .host = host.toStdString(),
      .port = (uint16_t)port.toInt()};
  auto sock = new qml_tcp_outbound_socket{ossia_conf, context->context};
  qjsEngine(this)->newQObject(sock);
  sock->onOpen = conf["onOpen"].value<QJSValue>();
  sock->onClose = conf["onClose"].value<QJSValue>();
  sock->onError = conf["onError"].value<QJSValue>();
  sock->onBytes = conf["onBytes"].value<QJSValue>();
  sock->open();
  return sock;
}

QObject* qml_protocols::inboundTCP(QVariant config)
{
  auto conf = config.toMap();
  auto transport = conf["Transport"].toMap();
  QString bind = transport["Bind"].toString();
  if(bind.isEmpty())
    bind = "0.0.0.0";
  QString port = transport["Port"].toString();

  ossia::net::inbound_socket_configuration ossia_conf{
      .bind = bind.toStdString(),
      .port = (uint16_t)port.toInt()};
  auto sock = new qml_tcp_inbound_socket{ossia_conf, context->context};
  qjsEngine(this)->newQObject(sock);
  sock->onOpen = conf["onOpen"].value<QJSValue>();
  sock->onClose = conf["onClose"].value<QJSValue>();
  sock->onError = conf["onError"].value<QJSValue>();
  sock->onConnection = conf["onConnection"].value<QJSValue>();
  sock->open();
  return sock;
}

QObject* qml_protocols::outboundWS(QVariant config)
{
  auto conf = config.toMap();
  auto transport = conf["Transport"].toMap();
  QString host = transport["Host"].toString();
  QString port = transport["Port"].toString();

  ossia::net::outbound_socket_configuration ossia_conf{
      .host = host.toStdString(),
      .port = (uint16_t)port.toInt()};
  auto sock = new qml_websocket_outbound_socket{ossia_conf, context->context};
  qjsEngine(this)->newQObject(sock);
  sock->onOpen = conf["onOpen"].value<QJSValue>();
  sock->onClose = conf["onClose"].value<QJSValue>();
  sock->onError = conf["onError"].value<QJSValue>();
  sock->onTextMessage = conf["onTextMessage"].value<QJSValue>();
  sock->onBinaryMessage = conf["onBinaryMessage"].value<QJSValue>();
  sock->open();
  return sock;
}

QObject* qml_protocols::inboundWS(QVariant config)
{
  auto conf = config.toMap();
  auto transport = conf["Transport"].toMap();
  QString bind = transport["Bind"].toString();
  if(bind.isEmpty())
    bind = "0.0.0.0";
  QString port = transport["Port"].toString();

  ossia::net::inbound_socket_configuration ossia_conf{
      .bind = bind.toStdString(),
      .port = (uint16_t)port.toInt()};
  auto sock = new qml_websocket_inbound_socket{ossia_conf, context->context};
  qjsEngine(this)->newQObject(sock);
  sock->onOpen = conf["onOpen"].value<QJSValue>();
  sock->onClose = conf["onClose"].value<QJSValue>();
  sock->onError = conf["onError"].value<QJSValue>();
  sock->onConnection = conf["onConnection"].value<QJSValue>();
  sock->open();
  return sock;
}

void qml_protocols::http(QUrl qurl, QJSValue val, QString verb)
{
  auto path = qurl.path();
  qml_protocols_http_answer a{this, std::move(val)};
  qml_protocols_http_error e;
  auto hrq = std::make_shared<request_type>(
      a, e, this->context->context, "ossia score", path.toStdString(),
      verb.toStdString());
  hrq->resolve(qurl.host().toStdString(), std::to_string(qurl.port(80)));
}

QObject* qml_protocols::inboundMIDI(QVariant config)
{
  return nullptr;
}

QObject* qml_protocols::inboundUMP(QVariant config)
{
  return nullptr;
}

QObject* qml_protocols::outboundMIDI(QVariant config)
{
  return nullptr;
}

QObject* qml_protocols::outboundUMP(QVariant config)
{
  return nullptr;
}

QObject* qml_protocols::serial(QVariant config)
{
  return nullptr;
}
}
