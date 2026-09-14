#include "qml_protocols.hpp"

#include <ossia/detail/logger.hpp>
#include <ossia/network/sockets/configuration.hpp>
#include <ossia/network/sockets/encoding.hpp>

#include <ossia-qt/invoke.hpp>
#include <ossia-qt/protocols/qml_bluetooth.hpp>
#include <ossia-qt/protocols/qml_can_socket.hpp>
#include <ossia-qt/protocols/qml_http_request.hpp>
#include <ossia-qt/protocols/qml_midi_inbound_socket.hpp>
#include <ossia-qt/protocols/qml_midi_outbound_socket.hpp>
#include <ossia-qt/protocols/qml_nfc.hpp>
#include <ossia-qt/protocols/qml_oauth.hpp>
#include <ossia-qt/protocols/qml_serial_socket.hpp>
#include <ossia-qt/protocols/qml_tcp_inbound_socket.hpp>
#include <ossia-qt/protocols/qml_tcp_outbound_socket.hpp>
#include <ossia-qt/protocols/qml_udp_inbound_socket.hpp>
#include <ossia-qt/protocols/qml_udp_outbound_socket.hpp>
#include <ossia-qt/protocols/qml_ump_inbound_socket.hpp>
#include <ossia-qt/protocols/qml_ump_outbound_socket.hpp>
#include <ossia-qt/protocols/qml_unix_inbound_socket.hpp>
#include <ossia-qt/protocols/qml_unix_outbound_socket.hpp>
#include <ossia-qt/protocols/qml_ws_inbound_socket.hpp>
#include <ossia-qt/protocols/qml_ws_outbound_socket.hpp>

#include <boost/asio/io_context.hpp>

#include <QFile>
#include <QJSValue>
#include <QJSValueList>
#include <QQmlEngine>

#include <libremidi/libremidi.hpp>

#if defined(__linux__)
// for the CAN interface enumeration
#include <net/if.h>
#include <net/if_arp.h>
#endif

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

 // Error callback of the UDP, TCP, Unix and WebSocket sockets: "onError" is the
 // canonical spelling, "onFail" an accepted alias for it. They name one single
 // callback: if a configuration carries both, the canonical spelling is used.
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
W_OBJECT_IMPL(ossia::qt::qml_websocket_connection)
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
W_OBJECT_IMPL(ossia::qt::qml_serial_socket)
#if defined(__linux__)
W_OBJECT_IMPL(ossia::qt::qml_can_socket)
#endif
W_OBJECT_IMPL(ossia::qt::qml_osc_processor)

#if defined(OSSIA_HAS_BLUETOOTH)
W_OBJECT_IMPL(ossia::qt::qml_bluetooth_scanner)
W_OBJECT_IMPL(ossia::qt::qml_bluetooth_socket)
W_OBJECT_IMPL(ossia::qt::qml_ble_service)
W_OBJECT_IMPL(ossia::qt::qml_ble_controller)
#endif

#if defined(OSSIA_HAS_NFC)
W_OBJECT_IMPL(ossia::qt::qml_nfc_target)
W_OBJECT_IMPL(ossia::qt::qml_nfc_scanner)
#endif

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

//! The error callback of a socket configuration: "onError" is the canonical
//! spelling, "onFail" an alias for that very same callback. A configuration
//! carrying both is honoured with the canonical spelling.
static QJSValue error_callback(const QVariantMap& conf)
{
  auto on_error = conf["onError"].value<QJSValue>();
  if(on_error.isCallable())
    return on_error;
  return conf["onFail"].value<QJSValue>();
}

//! Calls a script callback on the next iteration of the event loop, for the
//! same reason as open_later(): a factory runs while the QML component that
//! hosts it is still being created, so the device tree a callback would write
//! to does not exist yet.
static void call_later(QObject* owner, const QJSValue& callback, const QString& argument)
{
  if(!callback.isCallable())
    return;
  ossia::qt::run_async(
      owner, [callback, argument]() mutable { callback.call({argument}); });
}

//! Reports a socket that could not be opened through its error callback.
static void report_open_failure(QJSValue& onError, const QString& error)
{
  ossia::logger().error("Protocols: {}", error.toStdString());
  qDebug() << "Protocols:" << error;
  if(onError.isCallable())
    onError.call({error});
}

//! Opens a socket once the component that created it is in place.
/**
 * A factory is called while the QML properties of its host - a Mapper, for
 * instance - are being evaluated, that is, before createTree() has built the
 * device tree. Transports whose open, or whose failure, completes
 * synchronously (MIDI and UMP ports, a unix-domain connect, an immediately
 * refused endpoint) would then run onOpen / onError inside that window, where
 * Device.write() has no address to resolve and is silently dropped.
 *
 * Opening on the next iteration of the event loop puts the whole dispatch
 * after the tree exists, which is where the asynchronous transports already
 * land. Qt drops queued invocations whose receiver died first, so a socket the
 * script did not keep simply never opens; a synchronous failure is reported
 * through the configured error callback like an asynchronous one.
 */
template <typename Socket, typename Open>
static QObject* open_later(Socket* sock, Open open)
{
  ossia::qt::run_async(sock, [sock, open = std::move(open)]() mutable {
    try
    {
      open();
    }
    catch(const std::exception& e)
    {
      report_open_failure(sock->onError, QString::fromUtf8(e.what()));
    }
    catch(...)
    {
      report_open_failure(sock->onError, QStringLiteral("could not open socket"));
    }
  });
  return sock;
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
  sock->onError = error_callback(conf);
  return open_later(
      sock, [sock, ossia_conf, ctx = context, enc] {
    sock->open(ossia_conf, ctx->context, enc);
  });
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
  sock->onError = error_callback(conf);
  sock->onMessage = conf["onMessage"].value<QJSValue>();
  return open_later(
      sock, [sock, ossia_conf, ctx = context, enc] {
    sock->open(ossia_conf, ctx->context, enc);
  });
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
  sock->onError = error_callback(conf);
  return open_later(
      sock, [sock, ossia_conf, ctx = context, enc] {
    sock->open(ossia_conf, ctx->context, enc);
  });
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
  sock->onError = error_callback(conf);
  sock->onMessage = conf["onMessage"].value<QJSValue>();
  return open_later(
      sock, [sock, ossia_conf, ctx = context, enc] {
    sock->open(ossia_conf, ctx->context, enc);
  });
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
  sock->onError = error_callback(conf);
  sock->onMessage = conf["onMessage"].value<QJSValue>();
  sock->onBytes = conf["onBytes"].value<QJSValue>();
  return open_later(
      sock, [sock, ossia_conf, ctx = context, framing, delimiter, enc] {
    sock->open(ossia_conf, ctx->context, framing, delimiter, enc);
  });
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
  sock->onError = error_callback(conf);
  sock->onConnection = conf["onConnection"].value<QJSValue>();

  return open_later(
      sock, [sock, ossia_conf, ctx = context, framing, delimiter, enc] {
    sock->open(ossia_conf, ctx->context, framing, delimiter, enc);
  });
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
  sock->onError = error_callback(conf);
  sock->onMessage = conf["onMessage"].value<QJSValue>();
  sock->onBytes = conf["onBytes"].value<QJSValue>();
  return open_later(
      sock, [sock, ossia_conf, ctx = context, framing, delimiter, enc] {
    sock->open(ossia_conf, ctx->context, framing, delimiter, enc);
  });
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
  sock->onError = error_callback(conf);
  sock->onConnection = conf["onConnection"].value<QJSValue>();
  return open_later(
      sock, [sock, ossia_conf, ctx = context, framing, delimiter, enc] {
    sock->open(ossia_conf, ctx->context, framing, delimiter, enc);
  });
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
  sock->onError = error_callback(conf);
  sock->onTextMessage = conf["onTextMessage"].value<QJSValue>();
  sock->onBinaryMessage = conf["onBinaryMessage"].value<QJSValue>();
  return open_later(
      sock, [sock, ossia_conf, ctx = context] { sock->open(ossia_conf, ctx->context); });
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
  sock->onError = error_callback(conf);
  sock->onConnection = conf["onConnection"].value<QJSValue>();

  return open_later(
      sock, [sock, ossia_conf, ctx = context] { sock->open(ossia_conf, ctx); });
}

// Default port of the URL's scheme: it must not appear in the Host header.
static int http_default_port(const QUrl& url)
{
  return (url.scheme() == QLatin1String("https") || url.scheme() == QLatin1String("wss"))
             ? 443
             : 80;
}

// The host as it goes on the wire: an IPv6 literal keeps the brackets that
// separate it from the port, and an IDN host is punycode - a raw UTF-8 host is
// not something an HTTP server has to understand.
static QString http_wire_host(const QUrl& url)
{
  return url.host(QUrl::FullyEncoded);
}

// Host header value: the wire host, plus ":port" whenever the port is not the
// default of the scheme.
static QString http_authority(const QUrl& url)
{
  QString host = http_wire_host(url);
  if(host.contains(QLatin1Char(':')))
    host = QLatin1Char('[') + host + QLatin1Char(']');

  const int port = url.port(-1);
  if(port < 0 || port == http_default_port(url))
    return host;
  return host + QLatin1Char(':') + QString::number(port);
}

// CR and LF end a header field: a QML-supplied key or value carrying one
// injects headers - and with them a whole second request - into the stream.
static bool http_header_injects(const QString& field)
{
  return field.contains(QLatin1Char('\r')) || field.contains(QLatin1Char('\n'));
}

// Userinfo in an http URL is Basic credentials to every mainstream client
// (curl, browsers), so it is turned into the header it stands for rather than
// dropped, which would send the request unauthenticated.
static QString http_basic_authorization(const QUrl& url)
{
  if(url.userName().isEmpty() && url.password().isEmpty())
    return {};

  const auto credentials = url.userName() + QLatin1Char(':') + url.password();
  return QStringLiteral("Basic ") + QString::fromLatin1(credentials.toUtf8().toBase64());
}

// Request target: encoded path (never empty) followed by the encoded query.
static QString http_target(const QUrl& url)
{
  QString target = url.path(QUrl::FullyEncoded);
  if(target.isEmpty())
    target = QStringLiteral("/");
  if(url.hasQuery())
    target += QLatin1Char('?') + url.query(QUrl::FullyEncoded);
  return target;
}

// Legacy overload: Protocols.http(url, callback, verb). It shares the client
// implementation with the fetch overload below; only the callback shape
// differs, the response framing must not.
void qml_protocols::http(QUrl qurl, QJSValue val, QString verb)
{
  const auto target = http_target(qurl);
  const auto authority = http_authority(qurl);

  std::vector<std::pair<std::string, std::string>> headers;
  if(const auto authorization = http_basic_authorization(qurl); !authorization.isEmpty())
    headers.emplace_back("Authorization", authorization.toStdString());

  qml_protocols_http_answer a{this, std::move(val)};
  qml_protocols_http_error e;
  auto hrq = std::make_shared<request_type>(
      std::move(a), std::move(e), this->context->context, verb.toStdString(),
      authority.toStdString(), target.toStdString(), headers);
  try
  {
    hrq->resolve(
        http_wire_host(qurl).toStdString(),
        std::to_string(qurl.port(http_default_port(qurl))));
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

  const auto target = http_target(qurl);
  const auto authority = http_authority(qurl);

  qml_protocols_fetch_answer a{this, conf["onResponse"].value<QJSValue>()};
  qml_protocols_fetch_error e{this, conf["onError"].value<QJSValue>()};

  // Parse custom headers from QVariantMap
  std::vector<std::pair<std::string, std::string>> headers;
  bool hasAuthorization = false;
  auto headersMap = conf["headers"].toMap();
  for(auto it = headersMap.begin(); it != headersMap.end(); ++it)
  {
    const auto& key = it.key();
    const auto value = it.value().toString();
    if(http_header_injects(key) || http_header_injects(value))
    {
      // Sending it would be a request-splitting hole: nothing goes out.
      e(*this, "Invalid HTTP header \"" + key.toStdString() + "\": CR or LF");
      return;
    }

    if(key.compare(QLatin1String("Authorization"), Qt::CaseInsensitive) == 0)
      hasAuthorization = true;
    headers.emplace_back(key.toStdString(), value.toStdString());
  }

  // An explicit Authorization header wins over the URL's userinfo.
  if(!hasAuthorization)
    if(const auto authorization = http_basic_authorization(qurl);
       !authorization.isEmpty())
      headers.emplace_back("Authorization", authorization.toStdString());

  auto hrq = std::make_shared<fetch_request_type>(
      std::move(a), std::move(e), this->context->context, verb.toStdString(),
      authority.toStdString(), target.toStdString(), headers, body.toStdString());

  try
  {
    hrq->resolve(
        http_wire_host(qurl).toStdString(),
        std::to_string(qurl.port(http_default_port(qurl))));
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

//! Name of the libremidi backend an endpoint belongs to.
//! The backend is part of a port's identity: libremidi refuses to open a port
//! whose api does not match the one of the midi_in / midi_out it is given to.
//! Names rather than the numeric enum, as they survive JSON and QVariant
//! round-trips and stay readable in the QML debug output.
static QString midi_api_name(libremidi::API api)
{
  const auto name = libremidi::get_api_name(api);
  return QString::fromUtf8(name.data(), name.size());
}

//! How Protocols.*Devices() observes the machine's endpoints. Software ones
//! are part of it: the ports other applications expose - a synthesizer, a
//! sequencer, a virtual keyboard - are where most of a studio's MIDI lives.
static libremidi::observer_configuration enumeration_configuration()
{
  libremidi::observer_configuration conf;
  conf.track_virtual = true;
  return conf;
}

static void midi_port_information(
    QJSEngine* qjs, const libremidi::port_information& port, QJSValue& portInfo)
{
  portInfo.setProperty("Name", QString::fromStdString(port.port_name));
  portInfo.setProperty("DisplayName", QString::fromStdString(port.display_name));
  portInfo.setProperty("Manufacturer", QString::fromStdString(port.manufacturer));
  portInfo.setProperty("DeviceName", QString::fromStdString(port.device_name));
  portInfo.setProperty("API", midi_api_name(port.api));

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

static libremidi::port_information
qjs_to_midi_port_information(const QJSValue& portInfo, QString& error)
{
  libremidi::port_information port;

  // Basic string properties
  port.port_name = portInfo.property("Name").toString().toStdString();
  port.display_name = portInfo.property("DisplayName").toString().toStdString();
  port.manufacturer = portInfo.property("Manufacturer").toString().toStdString();
  port.device_name = portInfo.property("DeviceName").toString().toStdString();

  // Backend: an endpoint can only be reopened on the API it was enumerated
  // with, and there is nothing to fall back on if it is absent. An absent
  // property is told apart from a wrong one by asking the value, not its
  // string form: QJSValue::toString() renders `undefined` as the six-letter
  // word.
  const auto api_value = portInfo.property("API");
  const auto api = api_value.toString();
  const bool has_api
      = !api_value.isUndefined() && !api_value.isNull() && !api.isEmpty();
  port.api = has_api ? libremidi::get_compiled_api_by_name(api.toStdString())
                     : libremidi::API::UNSPECIFIED;
  if(port.api == libremidi::API::UNSPECIFIED)
  {
    error = has_api
                ? QStringLiteral("Unknown MIDI API: %1").arg(api)
                : QStringLiteral("Transport has no API: use one of the endpoints "
                                 "returned by Protocols.inbound/outboundMIDIDevices() "
                                 "or Protocols.inbound/outboundUMPDevices()");
    return port;
  }

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

//! Reports an endpoint that cannot be opened through the config's onError.
/**
 * Deferred like every other failure a factory reports: the tree the script
 * writes the error into does not exist yet while the factory runs.
 */
static bool midi_port_failed(
    QObject* owner, const QJSValue& config, const char* func, const QString& error)
{
  if(error.isEmpty())
    return false;

  qDebug() << func << error;
  call_later(owner, config.property("onError"), error);
  return true;
}

// for(let p of Protocols.inboundMIDIDevices()) { console.log(JSON.stringify(p)); }
QJSValue qml_protocols::inboundMIDIDevices()
{
  try
  {
    libremidi::observer observer{
        enumeration_configuration(), libremidi::midi1::default_api()};
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
        enumeration_configuration(), libremidi::midi2::default_api()};
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
        enumeration_configuration(), libremidi::midi1::default_api()};
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
        enumeration_configuration(), libremidi::midi2::default_api()};
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
  QString error;
  auto port = qjs_to_midi_port_information(config.property("Transport"), error);
  if(midi_port_failed(this, config, "Protocols.inboundMIDI:", error))
    return nullptr;

  auto sock = new qml_midi_inbound_socket{};
  qjsEngine(this)->newQObject(sock);

  // Set callbacks
  sock->onOpen = config.property("onOpen");
  sock->onClose = config.property("onClose");
  sock->onError = config.property("onError");
  sock->onMessage = config.property("onMessage");

  // Open the MIDI port
  return open_later(sock, [sock, port] { sock->open(port); });
}

QObject* qml_protocols::inboundUMP(QJSValue config)
{
  QString error;
  auto port = qjs_to_midi_port_information(config.property("Transport"), error);
  if(midi_port_failed(this, config, "Protocols.inboundUMP:", error))
    return nullptr;

  auto sock = new qml_ump_inbound_socket{};
  qjsEngine(this)->newQObject(sock);

  // Set callbacks
  sock->onOpen = config.property("onOpen");
  sock->onClose = config.property("onClose");
  sock->onError = config.property("onError");
  sock->onMessage = config.property("onMessage");

  // Open the UMP port
  return open_later(sock, [sock, port] { sock->open(port); });
}

QObject* qml_protocols::outboundMIDI(QJSValue config)
{
  QString error;
  auto port = qjs_to_midi_port_information(config.property("Transport"), error);
  if(midi_port_failed(this, config, "Protocols.outboundMIDI:", error))
    return nullptr;

  auto sock = new qml_midi_outbound_socket{};
  qjsEngine(this)->newQObject(sock);

  // Set callbacks
  sock->onOpen = config.property("onOpen");
  sock->onClose = config.property("onClose");
  sock->onError = config.property("onError");

  // Open the MIDI port
  return open_later(sock, [sock, port] { sock->open(port); });
}

QObject* qml_protocols::outboundUMP(QJSValue config)
{
  QString error;
  auto port = qjs_to_midi_port_information(config.property("Transport"), error);
  if(midi_port_failed(this, config, "Protocols.outboundUMP:", error))
    return nullptr;

  auto sock = new qml_ump_outbound_socket{};
  qjsEngine(this)->newQObject(sock);

  // Set callbacks
  sock->onOpen = config.property("onOpen");
  sock->onClose = config.property("onClose");
  sock->onError = config.property("onError");

  // Open the UMP port
  return open_later(sock, [sock, port] { sock->open(port); });
}

static ossia::net::serial_configuration parse_serial(const QVariantMap& transport)
{
  ossia::net::serial_configuration conf;
  conf.port = transport["Port"].toString().toStdString();

  if(auto baud = transport["Baud"]; baud.isValid())
    conf.baud_rate = baud.toInt();
  if(auto bits = transport["DataBits"]; bits.isValid())
    conf.character_size = bits.toInt();

  const auto flow = transport["FlowControl"].toString().toLower();
  if(flow == "software")
    conf.flow_control = ossia::net::serial_configuration::software;
  else if(flow == "hardware")
    conf.flow_control = ossia::net::serial_configuration::hardware;

  const auto parity = transport["Parity"].toString().toLower();
  if(parity == "odd")
    conf.parity = ossia::net::serial_configuration::odd;
  else if(parity == "even")
    conf.parity = ossia::net::serial_configuration::even;

  const auto stop = transport["StopBits"].toString().toLower();
  if(stop == "onepointfive" || stop == "1.5")
    conf.stop_bits = ossia::net::serial_configuration::onepointfive;
  else if(stop == "two" || stop == "2")
    conf.stop_bits = ossia::net::serial_configuration::two;

  return conf;
}

QObject* qml_protocols::serial(QVariant config)
{
  auto conf = config.toMap();
  auto onError = conf["onError"].value<QJSValue>();

  auto ossia_conf = parse_serial(conf["Transport"].toMap());
  if(ossia_conf.port.empty())
  {
    const QString err = "Transport.Port is required";
    ossia::logger().error("Protocols.serial: {}", err.toStdString());
    qDebug() << "Protocols.serial:" << err;
    call_later(this, onError, err);
    return nullptr;
  }

  auto [framing, delimiter] = parse_framing(conf);
  auto enc = parse_encoding(conf);

  // parse_framing packs the fixed_length frame size in the delimiter string
  std::size_t frame_size = 0;
  if(framing == ossia::net::framing::fixed_length && !delimiter.empty())
    frame_size = std::stoul(delimiter);

  auto sock = new qml_serial_socket{};
  qjsEngine(this)->newQObject(sock);
  sock->onOpen = conf["onOpen"].value<QJSValue>();
  sock->onClose = conf["onClose"].value<QJSValue>();
  sock->onError = onError;
  sock->onMessage = conf["onMessage"].value<QJSValue>();
  sock->onBytes = conf["onBytes"].value<QJSValue>();
  return open_later(
      sock,
      [sock, ossia_conf, ctx = context, framing, delimiter, frame_size, enc] {
    sock->open(ossia_conf, ctx->context, framing, delimiter, frame_size, enc);
  });
}

#if defined(__linux__)
static ossia::net::can_configuration
parse_can(const QVariantMap& transport, const QVariantList& filters)
{
  ossia::net::can_configuration conf;
  conf.interface_name = transport["Interface"].toString().toStdString();

  if(auto fd = transport["FD"]; fd.isValid())
    conf.fd = fd.toBool();
  if(auto lo = transport["Loopback"]; lo.isValid())
    conf.loopback = lo.toBool();
  if(auto own = transport["ReceiveOwnMessages"]; own.isValid())
    conf.receive_own_messages = own.toBool();
  if(auto err = transport["ErrorFrames"]; err.isValid())
    conf.error_frames = err.toBool();

  for(const auto& f : filters)
  {
    const auto filter = f.toMap();
    ossia::net::can_filter_configuration cf;
    cf.id = filter["id"].toUInt();
    if(filter["extended"].toBool())
      cf.id |= CAN_EFF_FLAG;
    if(filter["invert"].toBool())
      cf.id |= CAN_INV_FILTER;
    // Defaulting the mask to "all the identifier bits" makes the common
    // { id: 0x123 } filter mean "exactly 0x123" instead of "everything".
    if(auto mask = filter["mask"]; mask.isValid())
      cf.mask = mask.toUInt();
    else
      cf.mask = filter["extended"].toBool() ? CAN_EFF_MASK : CAN_SFF_MASK;
    conf.filters.push_back(cf);
  }

  return conf;
}

QObject* qml_protocols::can(QVariant config)
{
  auto conf = config.toMap();
  auto onError = conf["onError"].value<QJSValue>();

  auto ossia_conf = parse_can(conf["Transport"].toMap(), conf["Filters"].toList());
  if(ossia_conf.interface_name.empty())
  {
    const QString err = "Transport.Interface is required";
    ossia::logger().error("Protocols.can: {}", err.toStdString());
    qDebug() << "Protocols.can:" << err;
    call_later(this, onError, err);
    return nullptr;
  }

  auto sock = new qml_can_socket{};
  qjsEngine(this)->newQObject(sock);
  sock->onOpen = conf["onOpen"].value<QJSValue>();
  sock->onClose = conf["onClose"].value<QJSValue>();
  sock->onError = onError;
  sock->onMessage = conf["onMessage"].value<QJSValue>();
  return open_later(
      sock, [sock, ossia_conf, ctx = context] { sock->open(ossia_conf, ctx->context); });
}

QJSValue qml_protocols::canInterfaces()
{
  auto qjs = qjsEngine(this);
  if(!qjs)
    return {};

  auto result = qjs->newArray();
  int count = 0;

  // if_nameindex() lists every netdev; SocketCAN interfaces are the ones whose
  // link type is ARPHRD_CAN, which sysfs exposes as /sys/class/net/<if>/type.
  auto* list = ::if_nameindex();
  if(!list)
    return result;

  for(auto* it = list; it->if_index != 0 || it->if_name != nullptr; ++it)
  {
    const QString name = QString::fromUtf8(it->if_name);
    QFile type{QStringLiteral("/sys/class/net/%1/type").arg(name)};
    if(!type.open(QIODevice::ReadOnly))
      continue;
    if(type.readAll().trimmed().toInt() != ARPHRD_CAN)
      continue;

    auto obj = qjs->newObject();
    obj.setProperty("name", name);
    obj.setProperty("index", int(it->if_index));

    // An interface is CAN FD capable when its MTU is at least CANFD_MTU; on a
    // classic-only controller the MTU stays at CAN_MTU (16).
    QFile mtu{QStringLiteral("/sys/class/net/%1/mtu").arg(name)};
    if(mtu.open(QIODevice::ReadOnly))
    {
      const int m = mtu.readAll().trimmed().toInt();
      obj.setProperty("mtu", m);
      obj.setProperty("fd", m >= CANFD_MTU);
    }

    result.setProperty(count++, obj);
  }
  ::if_freenameindex(list);

  return result;
}
#else
// SocketCAN is Linux-only, but the slots are registered on the QML type
// everywhere, so they must still be defined. They throw rather than return null:
// a script calling Protocols.can() elsewhere deserves to be told why.
QObject* qml_protocols::can(QVariant config)
{
  const auto err = QStringLiteral(
      "CAN is only supported on Linux (SocketCAN); this build has no CAN backend");
  ossia::logger().error("Protocols.can: {}", err.toStdString());
  qDebug() << "Protocols.can:" << err;

  auto onError = config.toMap()["onError"].value<QJSValue>();
  if(onError.isCallable())
    onError.call({err});
  return nullptr;
}

QJSValue qml_protocols::canInterfaces()
{
  // An empty array, not undefined: enumerating the CAN interfaces of a machine
  // that has none is not an error, and callers can iterate the result without
  // having to special-case the platform.
  if(auto qjs = qjsEngine(this))
    return qjs->newArray();
  return {};
}
#endif

#if defined(OSSIA_HAS_BLUETOOTH)
QObject* qml_protocols::bluetoothScanner(QJSValue config)
{
  auto scanner = new qml_bluetooth_scanner{};
  qjsEngine(this)->newQObject(scanner);
  scanner->onDeviceDiscovered = config.property("onDeviceDiscovered");
  scanner->onFinished = config.property("onFinished");
  scanner->onError = config.property("onError");
  return scanner;
}

QObject* qml_protocols::bluetooth(QJSValue config)
{
  auto transport = config.property("Transport");
  QString address = transport.property("Address").toString();
  QString service = transport.property("Service").toString();

  if(address.isEmpty() || service.isEmpty())
  {
    qDebug() << "Bluetooth: Address and Service are required";
    return nullptr;
  }

  auto sock = new qml_bluetooth_socket{
      QBluetoothAddress{address}, bleUuidFromString(service)};
  qjsEngine(this)->newQObject(sock);
  sock->onOpen = config.property("onOpen");
  sock->onClose = config.property("onClose");
  sock->onError = config.property("onError");
  sock->onMessage = config.property("onMessage");

  try
  {
    sock->open();
    return sock;
  }
  catch(const std::exception& e)
  {
    qDebug() << e.what();
    delete sock;
    return nullptr;
  }
  catch(...)
  {
    qDebug() << "Error while creating Bluetooth socket";
    delete sock;
    return nullptr;
  }
}

QObject* qml_protocols::ble(QJSValue config)
{
  auto transport = config.property("Transport");
  QString address = transport.property("Address").toString();
  QString name = transport.property("Name").toString();
  QString deviceUuid = transport.property("DeviceUuid").toString();

  QBluetoothDeviceInfo info;
  if(!deviceUuid.isEmpty())
  {
    info = QBluetoothDeviceInfo{
        QBluetoothUuid{QUuid::fromString(deviceUuid)}, name, 0};
  }
  else if(!address.isEmpty())
  {
    info = QBluetoothDeviceInfo{QBluetoothAddress{address}, name, 0};
  }
  else
  {
    qDebug() << "BLE: Address or DeviceUuid is required";
    return nullptr;
  }
  info.setCoreConfigurations(
      QBluetoothDeviceInfo::LowEnergyCoreConfiguration);

  auto ctrl = new qml_ble_controller{info};
  qjsEngine(this)->newQObject(ctrl);
  ctrl->onConnected = config.property("onConnected");
  ctrl->onDisconnected = config.property("onDisconnected");
  ctrl->onServiceDiscovered = config.property("onServiceDiscovered");
  ctrl->onDiscoveryFinished = config.property("onDiscoveryFinished");
  ctrl->onMtuChanged = config.property("onMtuChanged");
  ctrl->onError = config.property("onError");

  try
  {
    ctrl->open();
    return ctrl;
  }
  catch(const std::exception& e)
  {
    qDebug() << e.what();
    delete ctrl;
    return nullptr;
  }
  catch(...)
  {
    qDebug() << "Error while creating BLE controller";
    delete ctrl;
    return nullptr;
  }
}
#else
QObject* qml_protocols::bluetoothScanner(QJSValue) { return nullptr; }
QObject* qml_protocols::bluetooth(QJSValue) { return nullptr; }
QObject* qml_protocols::ble(QJSValue) { return nullptr; }
#endif

#if defined(OSSIA_HAS_NFC)
QObject* qml_protocols::nfc(QJSValue config)
{
  auto scanner = new qml_nfc_scanner{};
  qjsEngine(this)->newQObject(scanner);
  scanner->onTargetDetected = config.property("onTargetDetected");
  scanner->onTargetLost = config.property("onTargetLost");
  scanner->onNdefMessage = config.property("onNdefMessage");
  scanner->onAdapterStateChanged = config.property("onAdapterStateChanged");
  scanner->onError = config.property("onError");

  try
  {
    scanner->start();
    return scanner;
  }
  catch(const std::exception& e)
  {
    qDebug() << e.what();
    delete scanner;
    return nullptr;
  }
  catch(...)
  {
    qDebug() << "Error while creating NFC scanner";
    delete scanner;
    return nullptr;
  }
}
#else
QObject* qml_protocols::nfc(QJSValue) { return nullptr; }
#endif

}
