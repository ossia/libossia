#include "qml_protocols.hpp"

#include <ossia-qt/protocols/qml_http_request.hpp>
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
W_OBJECT_IMPL(ossia::qt::qml_midi_inbound_socket)
W_OBJECT_IMPL(ossia::qt::qml_midi_outbound_socket)
W_OBJECT_IMPL(ossia::qt::qml_ump_inbound_socket)
W_OBJECT_IMPL(ossia::qt::qml_ump_outbound_socket)
W_OBJECT_IMPL(ossia::qt::qml_osc_processor)

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
      .bind = bind.toStdString(), .port = (uint16_t)port.toInt()};
  auto sock = new qml_udp_inbound_socket{ossia_conf, context->context};
  qjsEngine(this)->newQObject(sock);
  sock->onOpen = conf["onOpen"].value<QJSValue>();
  sock->onClose = conf["onClose"].value<QJSValue>();
  sock->onError = conf["onError"].value<QJSValue>();
  sock->onMessage = conf["onMessage"].value<QJSValue>();
  sock->open();
  return sock;
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

  ossia::net::fd_configuration ossia_conf{.fd = path.toStdString()};
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

  ossia::net::fd_configuration ossia_conf{.fd = path.toStdString()};
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

  ossia::net::fd_configuration ossia_conf{.fd = path.toStdString()};
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

  ossia::net::fd_configuration ossia_conf{.fd = path.toStdString()};
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
      .host = host.toStdString(), .port = (uint16_t)port.toInt()};
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
      .bind = bind.toStdString(), .port = (uint16_t)port.toInt()};
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
      .host = host.toStdString(), .port = (uint16_t)port.toInt()};
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
      .bind = bind.toStdString(), .port = (uint16_t)port.toInt()};
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

static void midi_port_information(
    QJSEngine* qjs, const libremidi::port_information& port, QJSValue& portInfo)
{
  portInfo.setProperty("Name", QString::fromStdString(port.port_name));
  portInfo.setProperty("DisplayName", QString::fromStdString(port.display_name));
  portInfo.setProperty("Manufacturer", QString::fromStdString(port.manufacturer));
  portInfo.setProperty("DeviceName", QString::fromStdString(port.device_name));

  // Port type flags
  auto typeObj = qjs->newArray();
  using pflags = libremidi::port_information;
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
  if(auto str = std::get_if<std::string>(&port.container))
  {
    portInfo.setProperty("ContainerID", QString::fromStdString(*str));
  }
  else if(auto num = std::get_if<std::uint64_t>(&port.container))
  {
    portInfo.setProperty("ContainerID", QString::number(*num));
  }
  else if(auto uid = std::get_if<libremidi::uuid>(&port.container))
  {
    auto uuidArr = qjs->newArray(16);
    for(int j = 0; j < 16; ++j)
    {
      uuidArr.setProperty(j, uid->bytes[j]);
    }
    portInfo.setProperty("ContainerID", uuidArr);
  }
  // Device identifier (convert variant to string if possible)
  if(auto str = std::get_if<std::string>(&port.device))
  {
    portInfo.setProperty("DeviceID", QString::fromStdString(*str));
  }
  else if(auto num = std::get_if<std::uint64_t>(&port.device))
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
        using pflags = libremidi::port_information;
        using pftype = libremidi::port_information::port_type;
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
  sock->open(port);

  return sock;
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
  sock->open(port);

  return sock;
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
  sock->open(port);

  return sock;
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
  sock->open(port);

  return sock;
}

QObject* qml_protocols::serial(QVariant config)
{
  return nullptr;
}
}
