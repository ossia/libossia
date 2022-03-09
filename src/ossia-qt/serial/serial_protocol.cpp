// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <ossia/detail/config.hpp>
#if defined(OSSIA_PROTOCOL_SERIAL)

#include "serial_protocol.hpp"
#include <QDebug>
#include <QQmlComponent>
#include <QQmlContext>
#include <QQmlEngine>
#include <iomanip>
#include <sstream>

#include <wobjectimpl.h>
W_OBJECT_IMPL(ossia::net::serial_wrapper)
namespace ossia
{
namespace net
{

static auto on_open = [] { fprintf(stderr, "connection open! \n"); };
static auto on_close = [] { fprintf(stderr, "connection on_close! \n"); };
static auto on_fail = [] { fprintf(stderr, "connection on_fail! \n"); };

struct serial_wrapper_read
{
  serial_wrapper& self;
  void operator()(const unsigned char* data, std::size_t sz) const noexcept {
    QByteArray arr{reinterpret_cast<const char*>(data), (int)sz};
    self.on_read(arr);
  };
};

struct serial_wrapper_init
{
  serial_wrapper& self;
  const serial_protocol_configuration& port;

  template<typename T>
  void common_init(T& m_socket)
  {
    m_socket.on_open.connect(on_open);
    m_socket.on_close.connect(on_close);
    m_socket.on_fail.connect(on_fail);

    m_socket.connect();
    m_socket.receive(serial_wrapper_read{self});
  }

  void operator()(auto& sock)
  {
    common_init(sock);
  }

  void operator()(line_framing_socket& sock)
  {
    int sz = std::max((int) 7, (int) port.line_framing_delimiter.size());
    std::copy_n(port.line_framing_delimiter.begin(), sz, sock.m_encoder.delimiter);
    std::copy_n(port.line_framing_delimiter.begin(), sz, sock.m_decoder.delimiter);
    common_init(sock);
  }
};

serial_wrapper::serial_wrapper(const network_context_ptr& ctx, const serial_protocol_configuration& port)
  : m_socket{make_socket(ctx, port)}
{
  std::visit(serial_wrapper_init{*this, port}, m_socket);
  m_open = true;
}

void serial_wrapper::on_write(const QByteArray& b) noexcept
{
  std::visit([&b] (auto& sock) { sock.write(b.data(), b.size()); }, m_socket);
}

void serial_wrapper::on_read(const QByteArray& arr)
{
  QString str = QString::fromLatin1(arr);
  read(str, arr);
}


framed_serial_socket serial_wrapper::make_socket(const network_context_ptr& ctx, const serial_protocol_configuration& port)
{
  switch(port.framing)
  {
    case ossia::net::framing::none:
      return framed_serial_socket{std::in_place_index<0>, port.transport, ctx->context};
    case ossia::net::framing::size_prefix:
      return framed_serial_socket{std::in_place_index<1>, port.transport, ctx->context};
    case ossia::net::framing::slip:
      return framed_serial_socket{std::in_place_index<2>, port.transport, ctx->context};
    default:
    case ossia::net::framing::line_delimiter:
      return framed_serial_socket{std::in_place_index<3>, port.transport, ctx->context};
  }
}

void serial_wrapper::close()
{
  if(m_open)
  {
    m_open = false;
    std::visit([] (auto& sock) { sock.close(); }, m_socket);
  }
}

serial_wrapper::~serial_wrapper() noexcept
{
}

serial_protocol::serial_protocol(
    const ossia::net::network_context_ptr& ctx
      , const QByteArray& code
      , const ossia::net::serial_configuration& cfg
      )
    : protocol_base{flags{}}
    , m_engine{new QQmlEngine}
    , m_component{new QQmlComponent{m_engine}}
    , m_code{code}
{
  QObject::connect(
      m_component, &QQmlComponent::statusChanged, this,
      [this, ctx, cfg](QQmlComponent::Status status) {
        qDebug() << status;
        qDebug() << m_component->errorString();
        if (!m_device)
          return;

        switch (status)
        {
          case QQmlComponent::Status::Ready:
            this->create(ctx, cfg);
            return;
          case QQmlComponent::Status::Loading:
            return;
          case QQmlComponent::Status::Null:
          case QQmlComponent::Status::Error:
            qDebug() << m_component->errorString();
            return;
        }
      });
}

void serial_protocol::create(const ossia::net::network_context_ptr& ctx, const ossia::net::serial_configuration& cfg)
{
  m_object = m_component->create();
  m_object->setParent(m_engine->rootContext());

  QVariant ret;
  QMetaObject::invokeMethod(
      m_object, "createTree", Q_RETURN_ARG(QVariant, ret));

  qt::create_device<ossia::net::device_base, serial_node, serial_protocol>(
      *m_device, ret.value<QJSValue>());

  m_jsObj = m_engine->newQObject(m_object);

  ossia::net::serial_protocol_configuration conf;
  conf.transport = cfg;

  auto delim = m_jsObj.property("delimiter").toString();
  auto frm = m_jsObj.property("framing").toString().toLower();

  conf.framing = ossia::net::framing::none;
  if(frm == "slip")
  {
    conf.framing = ossia::net::framing::slip;
  }
  else if(frm == "size")
  {
    conf.framing = ossia::net::framing::size_prefix;
  }
  else if(frm == "delimiter")
  {
    conf.framing = ossia::net::framing::line_delimiter;
    conf.line_framing_delimiter = delim.toStdString();
  }
  else
  {
    // Keep the same behaviour than before: split on \r\n
    conf.framing = ossia::net::framing::line_delimiter;
    if(!delim.isEmpty())
      conf.line_framing_delimiter = "\r\n";
    else
      conf.line_framing_delimiter = delim.toStdString();
  }

  m_onTextMessage = m_jsObj.property("onMessage");
  m_onBinaryMessage = m_jsObj.property("onBinary");
  m_onRead = m_jsObj.property("onRead");

  if(m_onTextMessage.isCallable())
  {
    // Argument: parsed as message, passed to JS as QString
  }
  else if(m_onBinaryMessage.isCallable())
  {
    // Argument: parsed as message, passed to JS as QByteArray
  }
  else if(m_onRead.isCallable())
  {
    // Argument: raw un-processed data, passed to JS as QByteArray
    conf.framing = ossia::net::framing::none;
  }

  m_port = std::make_shared<serial_wrapper>(ctx, conf);
  QObject::connect(m_port.get(), &serial_wrapper::read,
                   this, &serial_protocol::on_read, Qt::QueuedConnection);
  return;
}

bool serial_protocol::pull(parameter_base&)
{
  return false;
}

void serial_protocol::on_read(const QString& txt, const QByteArray& a)
{
  QJSValueList lst;
  QJSValue arr;
  if(m_onTextMessage.isCallable())
  {
    // Argument: parsed as message, passed to JS as QString
    lst.append(m_engine->toScriptValue(txt));
    arr = m_onTextMessage.callWithInstance(m_jsObj, lst);
  }
  else if(m_onBinaryMessage.isCallable())
  {
    // Argument: parsed as message, passed to JS as QByteArray
    lst.append(m_engine->toScriptValue(a));
    arr = m_onBinaryMessage.callWithInstance(m_jsObj, lst);
  }
  else if(m_onRead.isCallable())
  {
    // Argument: raw un-processed data, passed to JS as QByteArray
    lst.append(m_engine->toScriptValue(a));
    arr = m_onRead.callWithInstance(m_jsObj, lst);
  }


  // should be an array of { address, value } objects
  if (!arr.isArray())
    return;

  QJSValueIterator it(arr);
  while (it.hasNext())
  {
    it.next();
    auto val = it.value();
    auto addr = val.property("address");
    if (!addr.isString())
      continue;

    auto addr_txt = addr.toString().toStdString();
    auto n = find_node(m_device->get_root_node(), addr_txt);
    if (!n)
      continue;

    auto v = val.property("value");
    if (v.isNull())
      continue;

    if (auto addr = n->get_parameter())
    {
      // qDebug() << "Applied value"
      //          << QString::fromStdString(value_to_pretty_string(
      //                                      qt::value_from_js(addr->value(), v)));
      addr->set_value(qt::value_from_js(addr->value(), v));
    }
  }
}
bool serial_protocol::push(const ossia::net::parameter_base& addr, const ossia::value& v)
{
  // Prevent pushes when the device has not been set yet
  if(!m_port)
    return false;

  auto& ad = dynamic_cast<const serial_parameter&>(addr);
  auto str = ad.data().request;
  switch (addr.get_value_type())
  {
    case ossia::val_type::FLOAT:
      str.replace("$val", QString::number(v.get<float>(), 'g', 4));
      break;
    case ossia::val_type::INT:
      str.replace("$val", QString::number(v.get<int32_t>()));
      break;
    case ossia::val_type::STRING:
      str.replace("$val", QString::fromStdString(v.get<std::string>()));
      break;
    case ossia::val_type::BOOL:
      str.replace("$val", v.get<bool>() ? "1" : "0");
      break;
    case ossia::val_type::CHAR:
      str.replace("$val", QString{v.get<char>()});
      break;
    case ossia::val_type::VEC2F:
    {
      auto& vec = v.get<ossia::vec2f>();
      str.replace("$val", QString{"%1 %2"}.arg(vec[0]).arg(vec[1]));
      break;
    }
    case ossia::val_type::VEC3F:
    {
      auto& vec = v.get<ossia::vec3f>();
      str.replace("$val", QString{"%1 %2 %3"}.arg(vec[0]).arg(vec[1]).arg(vec[2]));
      break;
    }
    case ossia::val_type::VEC4F:
    {
      auto& vec = v.get<ossia::vec4f>();
      str.replace("$val", QString{"%1 %2 %3 %4"}.arg(vec[0]).arg(vec[1]).arg(vec[2]).arg(vec[3]));
      break;
    }
    case ossia::val_type::LIST:
      str.replace("$val", "TODO");
      break;
    case ossia::val_type::IMPULSE:
      str.replace("$val", "TODO");
      break;
    default:
      throw std::runtime_error("serial_protocol::push: bad type");
  }

  //qDebug() << str;
  m_port->on_write(str.toUtf8());
  return false;
}

bool serial_protocol::push_raw(const full_parameter_data& parameter_base)
{ return false; }

bool serial_protocol::observe(ossia::net::parameter_base&, bool)
{
  return false;
}

bool serial_protocol::update(ossia::net::node_base& node_base)
{
  return true;
}

void serial_protocol::set_device(device_base& dev)
{
  m_device = &dev;
  m_component->setData(m_code, QUrl{});
}

void serial_protocol::stop()
{
  this->m_port->close();
}

serial_protocol::~serial_protocol()
{
}

}
}
#endif
