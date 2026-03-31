#include "qml_protocols.hpp"

#include <ossia-qt/protocols/qml_http_request.hpp>
#include <ossia-qt/protocols/qml_oauth.hpp>
#include <ossia-qt/protocols/qml_midi_inbound_socket.hpp>
#include <ossia-qt/protocols/qml_midi_outbound_socket.hpp>
#include <ossia-qt/protocols/qml_ump_inbound_socket.hpp>
#include <ossia-qt/protocols/qml_ump_outbound_socket.hpp>
#include <ossia-qt/protocols/qml_tcp_inbound_socket.hpp>
#include <ossia-qt/protocols/qml_tcp_outbound_socket.hpp>
#include <ossia-qt/protocols/qml_udp_inbound_socket.hpp>
#include <ossia-qt/protocols/qml_udp_outbound_socket.hpp>
#include <ossia-qt/protocols/qml_unix_inbound_socket.hpp>
#include <ossia-qt/protocols/qml_unix_outbound_socket.hpp>
#include <ossia-qt/protocols/qml_ws_inbound_socket.hpp>
#include <ossia-qt/protocols/qml_ws_outbound_socket.hpp>

#include <ossia/network/sockets/configuration.hpp>
#include <ossia/network/sockets/encoding.hpp>

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

  // Legacy HTTP (GET only, no headers/body):
  Protocols.http("http://127.0.0.1:1234/toto", function(x){console.log(x);},"GET");

  // Full HTTP client (any method, custom headers, request body):
  Protocols.http({
    url: "http://127.0.0.1:8080/api/resource",
    verb: "POST",
    headers: { "Content-Type": "application/json", "Authorization": "Bearer token123" },
    body: JSON.stringify({ action: "start", num: 1 }),
    onResponse: function(status, body) { console.log("HTTP", status, body); },
    onError: function(err) { console.log("HTTP error:", err); }
  });

 // Encoding: optional, applies a binary-to-text encoding to the data
 // Supported types: "base64", "ascii85", "hex", "intel_hex"/"ihex", "srec"/"s_record"/"motorola"
 var sock = Protocols.outboundTCP({
   Transport: { Host: "127.0.0.1", Port: 1234 },
   Framing: { type: "line" },
   Encoding: { type: "base64" },
   onOpen: function(socket) { socket.write("binary data here"); },
 });

 */
W_OBJECT_IMPL(ossia::qt::qml_protocols)
namespace ossia::qt
{

#if defined(BOOST_ASIO_HAS_LOCAL_SOCKETS)
#endif
}

W_OBJECT_IMPL(ossia::qt::qml_udp_outbound_socket)
W_OBJECT_IMPL(ossia::qt::qml_udp_sender)
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
W_OBJECT_IMPL(ossia::qt::qml_midi_inbound_socket)
W_OBJECT_IMPL(ossia::qt::qml_midi_outbound_socket)
W_OBJECT_IMPL(ossia::qt::qml_ump_inbound_socket)
W_OBJECT_IMPL(ossia::qt::qml_ump_outbound_socket)
W_OBJECT_IMPL(ossia::qt::qml_osc_processor)

#if QT_VERSION > QT_VERSION_CHECK(6, 10, 0)
#if QT_NETWORKAUTH_LIB
W_OBJECT_IMPL(ossia::qt::qml_oauth)
#endif
#endif

namespace ossia::qt
{

static std::pair<ossia::net::framing, std::string> parse_framing(const QVariantMap& conf)
{
  auto framing_conf = conf["Framing"].toMap();
  if(framing_conf.isEmpty())
    return {ossia::net::framing::none, {}};

  auto type_str = framing_conf["type"].toString();
  std::string delimiter = framing_conf["delimiter"].toString().toStdString();

  if(type_str == "slip")
    return {ossia::net::framing::slip, {}};
  else if(type_str == "cobs")
    return {ossia::net::framing::cobs, {}};
  else if(type_str == "stx_etx")
    return {ossia::net::framing::stx_etx, {}};
  else if(type_str == "line")
    return {ossia::net::framing::line_delimiter, delimiter.empty() ? "\n" : delimiter};
  else if(type_str == "size_prefix")
  {
    int bytes = framing_conf["bytes"].toInt();
    auto endian = framing_conf["endian"].toString();
    bool le = (endian == "little");

    switch(bytes)
    {
      case 1:
        return {ossia::net::framing::size_prefix_1byte, {}};
      case 2:
        return {le ? ossia::net::framing::size_prefix_2byte_le
                   : ossia::net::framing::size_prefix_2byte_be,
                {}};
      default: // 0 (unset) or 4
        return {le ? ossia::net::framing::size_prefix_4byte_le
                   : ossia::net::framing::size_prefix,
                {}};
    }
  }
  else if(type_str == "fixed_length")
  {
    int size = framing_conf["size"].toInt();
    return {ossia::net::framing::fixed_length, std::to_string(size > 0 ? size : 64)};
  }
  else
    return {ossia::net::framing::none, {}};
}

static ossia::net::encoding parse_encoding(const QVariantMap& conf)
{
  auto encoding_conf = conf["Encoding"].toMap();
  if(encoding_conf.isEmpty())
    return ossia::net::encoding::none;

  auto type_str = encoding_conf["type"].toString();
  if(type_str == "base64")
    return ossia::net::encoding::base64;
  else if(type_str == "ascii85")
    return ossia::net::encoding::ascii85;
  else if(type_str == "hex")
    return ossia::net::encoding::hex;
  else if(type_str == "intel_hex" || type_str == "ihex")
    return ossia::net::encoding::intel_hex;
  else if(type_str == "srec" || type_str == "s_record" || type_str == "motorola")
    return ossia::net::encoding::srec;
  else
    return ossia::net::encoding::none;
}

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
  auto enc = parse_encoding(conf);

  ossia::net::outbound_socket_configuration ossia_conf{
      .host = host.toStdString(),
      .port = (uint16_t)port.toInt(),
      .broadcast = broadcast};
  auto sock = new qml_udp_outbound_socket{};
  // Doing this ensures that we can call qjsEngine(the_object) afterwards in open() callbacks:
  qjsEngine(this)->newQObject(sock);
  sock->onOpen = conf["onOpen"].value<QJSValue>();
  sock->onClose = conf["onClose"].value<QJSValue>();
  sock->onError = conf["onError"].value<QJSValue>();
  try {
    sock->open(ossia_conf, context->context, enc);
    return sock;
  } catch(...) {
    delete sock;
    return nullptr;
  }
}

QObject* qml_protocols::inboundUDP(QVariant config)
{
  auto conf = config.toMap();
  auto transport = conf["Transport"].toMap();
  QString bind = transport["Bind"].toString();
  if(bind.isEmpty())
    bind = "0.0.0.0";
  QString port = transport["Port"].toString();
  auto enc = parse_encoding(conf);

  ossia::net::inbound_socket_configuration ossia_conf{
      .bind = bind.toStdString(), .port = (uint16_t)port.toInt()};
  auto sock = new qml_udp_inbound_socket{};
  qjsEngine(this)->newQObject(sock);
  sock->onOpen = conf["onOpen"].value<QJSValue>();
  sock->onClose = conf["onClose"].value<QJSValue>();
  sock->onError = conf["onError"].value<QJSValue>();
  sock->onMessage = conf["onMessage"].value<QJSValue>();
  try {
    sock->open(ossia_conf, context->context, enc);
    return sock;
  } catch(const std::exception& e) {
    qDebug() << e.what();
    delete sock;
    return nullptr;
  } catch(...) {
    qDebug() << "Error while creating device";
    delete sock;
    return nullptr;
  }
}

QObject* qml_protocols::osc(QVariant config)
{
  auto conf = config.toMap();
  auto sock = new qml_osc_processor{};
  sock->onOsc = conf["onOsc"].value<QJSValue>();
  return sock;
}

QObject* qml_protocols::outboundUnixDatagram(QVariant config)
{
#if defined(BOOST_ASIO_HAS_LOCAL_SOCKETS)
  auto conf = config.toMap();
  auto transport = conf["Transport"].toMap();
  QString path = transport["Path"].toString();

  auto enc = parse_encoding(conf);
  ossia::net::fd_configuration ossia_conf{.fd = path.toStdString()};
  auto sock = new qml_unix_datagram_outbound_socket{};
  qjsEngine(this)->newQObject(sock);
  sock->onOpen = conf["onOpen"].value<QJSValue>();
  sock->onClose = conf["onClose"].value<QJSValue>();
  sock->onError = conf["onError"].value<QJSValue>();
  try {
    sock->open(ossia_conf, context->context, enc);
    return sock;
  } catch(const std::exception& e) {
    qDebug() << e.what();
    delete sock;
    return nullptr;
  } catch(...) {
    qDebug() << "Error while creating device";
    delete sock;
    return nullptr;
  }
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

  auto enc = parse_encoding(conf);
  ossia::net::fd_configuration ossia_conf{.fd = path.toStdString()};
  auto sock = new qml_unix_datagram_inbound_socket{};
  qjsEngine(this)->newQObject(sock);
  sock->onOpen = conf["onOpen"].value<QJSValue>();
  sock->onClose = conf["onClose"].value<QJSValue>();
  sock->onError = conf["onError"].value<QJSValue>();
  sock->onMessage = conf["onMessage"].value<QJSValue>();
  try {
    sock->open(ossia_conf, context->context, enc);
    return sock;
  } catch(const std::exception& e) {
    qDebug() << e.what();
    delete sock;
    return nullptr;
  } catch(...) {
    qDebug() << "Error while creating device";
    delete sock;
    return nullptr;
  }
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
  auto [framing, delimiter] = parse_framing(conf);
  auto enc = parse_encoding(conf);

  ossia::net::fd_configuration ossia_conf{.fd = path.toStdString()};
  auto sock = new qml_unix_stream_outbound_socket{};
  qjsEngine(this)->newQObject(sock);
  sock->onOpen = conf["onOpen"].value<QJSValue>();
  sock->onClose = conf["onClose"].value<QJSValue>();
  sock->onError = conf["onError"].value<QJSValue>();
  sock->onMessage = conf["onMessage"].value<QJSValue>();
  sock->onBytes = conf["onBytes"].value<QJSValue>();
  try {
    sock->open(ossia_conf, context->context, framing, delimiter, enc);
    return sock;
  } catch(const std::exception& e) {
    qDebug() << e.what();
    delete sock;
    return nullptr;
  } catch(...) {
    qDebug() << "Error while creating device";
    delete sock;
    return nullptr;
  }
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
  auto [framing, delimiter] = parse_framing(conf);
  auto enc = parse_encoding(conf);

  ossia::net::fd_configuration ossia_conf{.fd = path.toStdString()};
  auto sock = new qml_unix_stream_inbound_socket{};
  qjsEngine(this)->newQObject(sock);
  sock->onOpen = conf["onOpen"].value<QJSValue>();
  sock->onClose = conf["onClose"].value<QJSValue>();
  sock->onError = conf["onError"].value<QJSValue>();
  sock->onConnection = conf["onConnection"].value<QJSValue>();

  try {
    sock->open(ossia_conf, context->context, framing, delimiter, enc);
    return sock;
  } catch(const std::exception& e) {
    qDebug() << e.what();
    delete sock;
    return nullptr;
  } catch(...) {
    qDebug() << "Error while creating device";
    delete sock;
    return nullptr;
  }
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
  auto [framing, delimiter] = parse_framing(conf);
  auto enc = parse_encoding(conf);

  ossia::net::outbound_socket_configuration ossia_conf{
      .host = host.toStdString(), .port = (uint16_t)port.toInt()};
  auto sock = new qml_tcp_outbound_socket{};
  qjsEngine(this)->newQObject(sock);
  sock->onOpen = conf["onOpen"].value<QJSValue>();
  sock->onClose = conf["onClose"].value<QJSValue>();
  sock->onError = conf["onError"].value<QJSValue>();
  sock->onMessage = conf["onMessage"].value<QJSValue>();
  sock->onBytes = conf["onBytes"].value<QJSValue>();
  try {
    sock->open(ossia_conf, context->context, framing, delimiter, enc);
    return sock;
  } catch(const std::exception& e) {
    qDebug() << e.what();
    delete sock;
    return nullptr;
  } catch(...) {
    qDebug() << "Error while creating device";
    delete sock;
    return nullptr;
  }
}

QObject* qml_protocols::inboundTCP(QVariant config)
{
  auto conf = config.toMap();
  auto transport = conf["Transport"].toMap();
  QString bind = transport["Bind"].toString();
  if(bind.isEmpty())
    bind = "0.0.0.0";
  QString port = transport["Port"].toString();
  auto [framing, delimiter] = parse_framing(conf);
  auto enc = parse_encoding(conf);

  ossia::net::inbound_socket_configuration ossia_conf{
      .bind = bind.toStdString(), .port = (uint16_t)port.toInt()};
  auto sock = new qml_tcp_inbound_socket{};
  qjsEngine(this)->newQObject(sock);
  sock->onOpen = conf["onOpen"].value<QJSValue>();
  sock->onClose = conf["onClose"].value<QJSValue>();
  sock->onError = conf["onError"].value<QJSValue>();
  sock->onConnection = conf["onConnection"].value<QJSValue>();
  try {
    sock->open(ossia_conf, context->context, framing, delimiter, enc);
    return sock;
  } catch(const std::exception& e) {
    qDebug() << e.what();
    delete sock;
    return nullptr;
  } catch(...) {
    qDebug() << "Error while creating device";
    delete sock;
    return nullptr;
  }
}

QObject* qml_protocols::outboundWS(QVariant config)
{
  auto conf = config.toMap();
  auto transport = conf["Transport"].toMap();
  QString host = transport["Host"].toString();
  QString port = transport["Port"].toString();

  ossia::net::outbound_socket_configuration ossia_conf{
      .host = host.toStdString(), .port = (uint16_t)port.toInt()};
  auto sock = new qml_websocket_outbound_socket{};
  qjsEngine(this)->newQObject(sock);
  sock->onOpen = conf["onOpen"].value<QJSValue>();
  sock->onClose = conf["onClose"].value<QJSValue>();
  sock->onError = conf["onError"].value<QJSValue>();
  sock->onTextMessage = conf["onTextMessage"].value<QJSValue>();
  sock->onBinaryMessage = conf["onBinaryMessage"].value<QJSValue>();
  try {
    sock->open(ossia_conf, context->context);
    return sock;
  } catch(const std::exception& e) {
    qDebug() << e.what();
    delete sock;
    return nullptr;
  } catch(...) {
    qDebug() << "Error while creating device";
    delete sock;
    return nullptr;
  }
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
      .bind = bind.toStdString(), .port = (uint16_t)port.toInt()};
  auto sock = new qml_websocket_inbound_socket{};
  qjsEngine(this)->newQObject(sock);
  sock->onOpen = conf["onOpen"].value<QJSValue>();
  sock->onClose = conf["onClose"].value<QJSValue>();
  sock->onError = conf["onError"].value<QJSValue>();
  sock->onConnection = conf["onConnection"].value<QJSValue>();

  try {
    sock->open(ossia_conf, context->context);
    return sock;
  } catch(const std::exception& e) {
    qDebug() << e.what();
    delete sock;
    return nullptr;
  } catch(...) {
    qDebug() << "Error while creating device";
    delete sock;
    return nullptr;
  }
}

void qml_protocols::http(QUrl qurl, QJSValue val, QString verb)
{
  auto path = qurl.path();
  qml_protocols_http_answer a{this, std::move(val)};
  qml_protocols_http_error e;
  auto hrq = std::make_shared<request_type>(
      a, e, this->context->context, "ossia score", path.toStdString(),
      verb.toStdString());
  try {
  hrq->resolve(qurl.host().toStdString(), std::to_string(qurl.port(80)));
  } catch(const std::exception& e) {
    qDebug() << e.what();
  } catch(...) {
    qDebug() << "Error while sending HTTP request";
  }
}

void qml_protocols::http(QVariant config)
{
  auto conf = config.toMap();
  QUrl qurl = conf["url"].toUrl();
  if(qurl.isEmpty())
    qurl = QUrl(conf["url"].toString());
  if(!qurl.isValid())
    return;

  QString verb = conf["verb"].toString();
  if(verb.isEmpty())
    verb = "GET";

  QString body = conf["body"].toString();

  // Build path with query string
  QString path = qurl.path(QUrl::FullyEncoded);
  if(qurl.hasQuery())
    path += "?" + qurl.query(QUrl::FullyEncoded);
  if(path.isEmpty())
    path = "/";

  auto host = qurl.host();

  // Parse custom headers from QVariantMap
  std::vector<std::pair<std::string, std::string>> headers;
  auto headersMap = conf["headers"].toMap();
  for(auto it = headersMap.begin(); it != headersMap.end(); ++it)
    headers.emplace_back(it.key().toStdString(), it.value().toString().toStdString());

  qml_protocols_fetch_answer a{this, conf["onResponse"].value<QJSValue>()};
  qml_protocols_fetch_error e{this, conf["onError"].value<QJSValue>()};

  auto hrq = std::make_shared<fetch_request_type>(
      std::move(a), std::move(e), this->context->context, verb.toStdString(),
      host.toStdString(), path.toStdString(), headers, body.toStdString());

  try
  {
    hrq->resolve(host.toStdString(), std::to_string(qurl.port(80)));
  }
  catch(const std::exception& e)
  {
    qDebug() << e.what();
  }
  catch(...)
  {
    qDebug() << "Error while sending HTTP request";
  }
}

static void midi_port_information(
    QJSEngine* qjs, const libremidi::port_information& port, QJSValue& portInfo)
{
  portInfo.setProperty("Name", QString::fromStdString(port.port_name));
  portInfo.setProperty("DisplayName", QString::fromStdString(port.display_name));
  portInfo.setProperty("Manufacturer", QString::fromStdString(port.manufacturer));
  portInfo.setProperty("DeviceName", QString::fromStdString(port.device_name));

  // Port type flags
  auto typeObj = qjs->newArray();
  using pflags = libremidi::transport_type;
  auto pushy = typeObj.property("push");
  if(static_cast<bool>(port.type & pflags::software))
    pushy.call({"Software"});
  if(static_cast<bool>(port.type & pflags::loopback))
    pushy.call({"Loopback"});

  if(static_cast<bool>(port.type & pflags::hardware))
    pushy.call({"Hardware"});
  if(static_cast<bool>(port.type & pflags::usb))
    pushy.call({"USB"});
  if(static_cast<bool>(port.type & pflags::bluetooth))
    pushy.call({"Bluetooth"});
  if(static_cast<bool>(port.type & pflags::pci))
    pushy.call({"PCI"});
  if(static_cast<bool>(port.type & pflags::network))
    pushy.call({"Network"});

  portInfo.setProperty("Type", typeObj);

  // Port handle
  portInfo.setProperty("PortHandle", QString::number(port.port));

  // Client handle
  portInfo.setProperty("ClientHandle", QString::number(port.client));

  // Container identifier (convert variant to string if possible)
  if(auto str = get_if<std::string>(&port.container))
  {
    portInfo.setProperty("ContainerID", QString::fromStdString(*str));
  }
  else if(auto num = get_if<std::uint64_t>(&port.container))
  {
    portInfo.setProperty("ContainerID", QString::number(*num));
  }
  else if(auto uid = get_if<libremidi::uuid>(&port.container))
  {
    auto uuidArr = qjs->newArray(16);
    for(int j = 0; j < 16; ++j)
    {
      uuidArr.setProperty(j, uid->bytes[j]);
    }
    portInfo.setProperty("ContainerID", uuidArr);
  }
  // Device identifier (convert variant to string if possible)
  if(auto str = get_if<std::string>(&port.device))
  {
    portInfo.setProperty("DeviceID", QString::fromStdString(*str));
  }
  else if(auto num = get_if<std::uint64_t>(&port.device))
  {
    portInfo.setProperty("DeviceID", QString::number(*num));
  }
}

static libremidi::port_information qjs_to_midi_port_information(const QJSValue& portInfo)
{
  libremidi::port_information port;

  // Basic string properties
  port.port_name = portInfo.property("Name").toString().toStdString();
  port.display_name = portInfo.property("DisplayName").toString().toStdString();
  port.manufacturer = portInfo.property("Manufacturer").toString().toStdString();
  port.device_name = portInfo.property("DeviceName").toString().toStdString();

  // Port type flags
  port.type = {};
  if(portInfo.hasProperty("Type"))
  {
    auto typeArr = portInfo.property("Type");
    if(typeArr.isArray())
    {
      auto length = typeArr.property("length").toInt();
      for(int i = 0; i < length; ++i)
      {
        auto typeStr = typeArr.property(i).toString();
        using pftype = libremidi::transport_type;
        using pflags = libremidi::transport_type;
        if(typeStr == "Software")
          port.type = pftype((uint8_t)port.type | (uint8_t)pflags::software);
        else if(typeStr == "Loopback")
          port.type = pftype((uint8_t)port.type | (uint8_t)pflags::loopback);
        else if(typeStr == "Hardware")
          port.type = pftype((uint8_t)port.type | (uint8_t)pflags::hardware);
        else if(typeStr == "USB")
          port.type = pftype((uint8_t)port.type | (uint8_t)pflags::usb);
        else if(typeStr == "Bluetooth")
          port.type = pftype((uint8_t)port.type | (uint8_t)pflags::bluetooth);
        else if(typeStr == "PCI")
          port.type = pftype((uint8_t)port.type | (uint8_t)pflags::pci);
        else if(typeStr == "Network")
          port.type = pftype((uint8_t)port.type | (uint8_t)pflags::network);
      }
    }
  }

  // Port handle
  port.port = portInfo.property("PortHandle").toString().toULongLong();

  // Client handle
  port.client = portInfo.property("ClientHandle").toString().toULongLong();

  // Container identifier
  if(portInfo.hasProperty("ContainerID"))
  {
    auto containerProp = portInfo.property("ContainerID");
    if(containerProp.isString())
    {
      auto str = containerProp.toString();
      bool ok;
      auto num = str.toULongLong(&ok);
      if(ok)
        port.container = num;
      else
        port.container = str.toStdString();
    }
    else if(containerProp.isArray())
    {
      // UUID case
      libremidi::uuid uid;
      auto length = containerProp.property("length").toInt();
      if(length == 16)
      {
        for(int i = 0; i < 16; ++i)
        {
          uid.bytes[i] = static_cast<uint8_t>(containerProp.property(i).toInt());
        }
        port.container = uid;
      }
    }
  }

  // Device identifier
  if(portInfo.hasProperty("DeviceID"))
  {
    auto deviceProp = portInfo.property("DeviceID");
    if(deviceProp.isString())
    {
      auto str = deviceProp.toString();
      bool ok;
      auto num = str.toULongLong(&ok);
      if(ok)
        port.device = num;
      else
        port.device = str.toStdString();
    }
  }

  return port;
}

// for(let p of Protocols.inboundMIDIDevices()) { console.log(JSON.stringify(p)); }
QJSValue qml_protocols::inboundMIDIDevices()
{
  try
  {
    libremidi::observer observer{
        libremidi::observer_configuration{}, libremidi::midi1::default_api()};
    auto ports = observer.get_input_ports();
    auto qjs = qjsEngine(this);
    auto result = qjs->newArray(ports.size());
    int i = 0;
    for(const auto& port : ports)
    {
      auto portInfo = qjs->newObject();
      midi_port_information(qjs, port, portInfo);
      result.setProperty(i, portInfo);
      i++;
    }
    return result;
  }
  catch(...)
  {
  }
  return {};
}

QJSValue qml_protocols::inboundUMPDevices()
{
  try
  {
    libremidi::observer observer{
        libremidi::observer_configuration{}, libremidi::midi2::default_api()};
    auto ports = observer.get_input_ports();
    auto qjs = qjsEngine(this);
    auto result = qjs->newArray(ports.size());
    int i = 0;
    for(const auto& port : ports)
    {
      auto portInfo = qjs->newObject();
      midi_port_information(qjs, port, portInfo);
      result.setProperty(i, portInfo);
      i++;
    }
    return result;
  }
  catch(...)
  {
  }
  return {};
}

QJSValue qml_protocols::outboundMIDIDevices()
{
  try
  {
    libremidi::observer observer{
        libremidi::observer_configuration{}, libremidi::midi1::default_api()};
    auto ports = observer.get_output_ports();
    auto qjs = qjsEngine(this);
    auto result = qjs->newArray(ports.size());
    int i = 0;
    for(const auto& port : ports)
    {
      auto portInfo = qjs->newObject();
      midi_port_information(qjs, port, portInfo);
      result.setProperty(i, portInfo);
      i++;
    }
    return result;
  }
  catch(...)
  {
  }
  return {};
}

QJSValue qml_protocols::outboundUMPDevices()
{
  try
  {
    libremidi::observer observer{
        libremidi::observer_configuration{}, libremidi::midi2::default_api()};
    auto ports = observer.get_output_ports();

    auto qjs = qjsEngine(this);
    auto result = qjs->newArray(ports.size());
    int i = 0;
    for(const auto& port : ports)
    {
      auto portInfo = qjs->newObject();
      midi_port_information(qjs, port, portInfo);
      result.setProperty(i, portInfo);
      i++;
    }
    return result;
  }
  catch(...)
  {
  }
  return {};
}

QObject* qml_protocols::inboundMIDI(QJSValue config)
{
  auto transport = config.property("Transport");
  auto port = qjs_to_midi_port_information(transport);

  auto sock = new qml_midi_inbound_socket{};
  qjsEngine(this)->newQObject(sock);

  // Set callbacks
  sock->onOpen = config.property("onOpen");
  sock->onClose = config.property("onClose");
  sock->onError = config.property("onError");
  sock->onMessage = config.property("onMessage");

  // Open the MIDI port
  try {
    sock->open(port);
    return sock;
  } catch(const std::exception& e) {
    qDebug() << e.what();
    delete sock;
    return nullptr;
  } catch(...) {
    qDebug() << "Error while creating device";
    delete sock;
    return nullptr;
  }
}

QObject* qml_protocols::inboundUMP(QJSValue config)
{
  auto transport = config.property("Transport");
  auto port = qjs_to_midi_port_information(transport);

  auto sock = new qml_ump_inbound_socket{};
  qjsEngine(this)->newQObject(sock);

  // Set callbacks
  sock->onOpen = config.property("onOpen");
  sock->onClose = config.property("onClose");
  sock->onError = config.property("onError");
  sock->onMessage = config.property("onMessage");

  // Open the UMP port
  try {
    sock->open(port);
    return sock;
  } catch(const std::exception& e) {
    qDebug() << e.what();
    delete sock;
    return nullptr;
  } catch(...) {
    qDebug() << "Error while creating device";
    delete sock;
    return nullptr;
  }
}

QObject* qml_protocols::outboundMIDI(QJSValue config)
{
  auto transport = config.property("Transport");
  auto port = qjs_to_midi_port_information(transport);

  auto sock = new qml_midi_outbound_socket{};
  qjsEngine(this)->newQObject(sock);

  // Set callbacks
  sock->onOpen = config.property("onOpen");
  sock->onClose = config.property("onClose");
  sock->onError = config.property("onError");

  // Open the MIDI port
  try {
    sock->open(port);
    return sock;
  } catch(const std::exception& e) {
    qDebug() << e.what();
    delete sock;
    return nullptr;
  } catch(...) {
    qDebug() << "Error while creating device";
    delete sock;
    return nullptr;
  }
}

QObject* qml_protocols::outboundUMP(QJSValue config)
{
  auto transport = config.property("Transport");
  auto port = qjs_to_midi_port_information(transport);

  auto sock = new qml_ump_outbound_socket{};
  qjsEngine(this)->newQObject(sock);

  // Set callbacks
  sock->onOpen = config.property("onOpen");
  sock->onClose = config.property("onClose");
  sock->onError = config.property("onError");

  // Open the UMP port
  try {
    sock->open(port);
    return sock;
  } catch(const std::exception& e) {
    qDebug() << e.what();
    delete sock;
    return nullptr;
  } catch(...) {
    qDebug() << "Error while creating device";
    delete sock;
    return nullptr;
  }
}

QObject* qml_protocols::serial(QVariant config)
{
  return nullptr;
}
}
