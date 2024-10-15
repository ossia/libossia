// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <ossia/detail/config.hpp>
#if defined(OSSIA_PROTOCOL_SERIAL)

#include "serial_protocol.hpp"

#include <ossia/detail/lockfree_queue.hpp>

#include <ossia-qt/invoke.hpp>

#include <QDebug>
#include <QQmlComponent>
#include <QQmlContext>
#include <QQmlEngine>

#include <wobjectimpl.h>

#include <iomanip>
#include <sstream>
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
  void operator()(const unsigned char* data, std::size_t sz) const noexcept
  {
    QByteArray arr{reinterpret_cast<const char*>(data), (int)sz};
    self.on_read(arr);
  };
};

struct serial_wrapper_init
{
  serial_wrapper& self;
  const serial_protocol_configuration& port;

  template <typename T>
  void common_init(T& m_socket)
  {
    m_socket.on_open.connect(on_open);
    m_socket.on_close.connect(on_close);
    m_socket.on_fail.connect(on_fail);

    m_socket.connect();
    m_socket.receive(serial_wrapper_read{self});
  }

  void operator()(auto& sock) { common_init(sock); }

  void operator()(line_framing_socket& sock)
  {
    int sz = std::max((int)7, (int)port.line_framing_delimiter.size());
    std::copy_n(port.line_framing_delimiter.begin(), sz, sock.m_encoder.delimiter);
    std::copy_n(port.line_framing_delimiter.begin(), sz, sock.m_decoder.delimiter);
    common_init(sock);
  }
};

serial_wrapper::serial_wrapper(
    const network_context_ptr& ctx, const serial_protocol_configuration& port)
    : m_socket{make_socket(ctx, port)}
{
  ossia::visit(serial_wrapper_init{*this, port}, m_socket);
  m_open = true;
}

void serial_wrapper::on_write(const QByteArray& b) noexcept
{
  ossia::visit([&b](auto& sock) { sock.write(b.data(), b.size()); }, m_socket);
}

void serial_wrapper::on_read(const QByteArray& arr)
{
  read(QString::fromLatin1(arr), arr);
}

framed_serial_socket serial_wrapper::make_socket(
    const network_context_ptr& ctx, const serial_protocol_configuration& port)
{
  switch(port.framing)
  {
    case ossia::net::framing::none:
      return framed_serial_socket{
          ossia::in_place_index<0>, port.transport, ctx->context};
    case ossia::net::framing::size_prefix:
      return framed_serial_socket{
          ossia::in_place_index<1>, port.transport, ctx->context};
    case ossia::net::framing::slip:
      return framed_serial_socket{
          ossia::in_place_index<2>, port.transport, ctx->context};
    default:
    case ossia::net::framing::line_delimiter:
      return framed_serial_socket{
          ossia::in_place_index<3>, port.transport, ctx->context};
  }
}

void serial_wrapper::close()
{
  if(m_open)
  {
    m_open = false;
    ossia::visit([](auto& sock) { sock.close(); }, m_socket);
  }
}

serial_wrapper::~serial_wrapper() noexcept { }

struct serial_protocol_object
{
  QObject* m_object{};
  QJSValue m_jsObj{};

  QJSValue m_onTextMessage;
  QJSValue m_onBinaryMessage;
  QJSValue m_onRead;

  ossia::qt::deferred_js_node<serial_parameter_data> nodes;
  std::shared_ptr<serial_wrapper> m_port;

  double m_coalesce{};
};

serial_protocol_object serial_protocol::load_serial_object_from_qml(
    serial_protocol& proto, const ossia::net::network_context_ptr& ctx,
    const ossia::net::serial_configuration& cfg)
{
  serial_protocol_object r;

  auto m_engine = proto.m_engine;
  auto m_component = proto.m_component;

  r.m_object = m_component->create();
  r.m_object->setParent(m_engine->rootContext());

  QVariant createTree_ret;
  QMetaObject::invokeMethod(
      r.m_object, "createTree", Q_RETURN_ARG(QVariant, createTree_ret));

  // We have to lock the main thread while this happens
  // or.... create the object in the other thread and just add it to the device in the main one.
  r.nodes = ossia::qt::create_device_nodes_deferred<serial_protocol>(
      createTree_ret.value<QJSValue>());

  r.m_jsObj = m_engine->newQObject(r.m_object);

  // Read object properties
  r.m_coalesce = r.m_jsObj.property("coalesce").toNumber();

  ossia::net::serial_protocol_configuration conf;
  conf.transport = cfg;

  auto delim = r.m_jsObj.property("delimiter").toString();
  auto frm = r.m_jsObj.property("framing").toString().toLower();

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

  r.m_onTextMessage = r.m_jsObj.property("onMessage");
  r.m_onBinaryMessage = r.m_jsObj.property("onBinary");
  r.m_onRead = r.m_jsObj.property("onRead");

  if(r.m_onTextMessage.isCallable())
  {
    // Argument: parsed as message, passed to JS as QString
  }
  else if(r.m_onBinaryMessage.isCallable())
  {
    // Argument: parsed as message, passed to JS as QByteArray
  }
  else if(r.m_onRead.isCallable())
  {
    // Argument: raw un-processed data, passed to JS as QByteArray
    conf.framing = ossia::net::framing::none;
  }

  try
  {
    r.m_port = std::make_shared<serial_wrapper>(ctx, conf);

    // on_read has to execute in the serial thread as it calls QQmlEngine methods
    QObject::connect(
        r.m_port.get(), &serial_wrapper::read, proto.m_threadWorker,
        [&proto](const QString& l, const QByteArray& b) { proto.on_read(l, b); },
        Qt::QueuedConnection);
  }
  catch(...)
  {
  }

  return r;
}

struct serial_worker : public QObject
{
  coalescing_queue& m_queue;

  explicit serial_worker(coalescing_queue& q)
      : m_queue{q}
  {
  }

  void timerEvent(QTimerEvent* ev) { m_queue.process_messages(); }
};

serial_protocol::serial_protocol(
    const ossia::net::network_context_ptr& ctx, const QByteArray& code,
    const ossia::net::serial_configuration& cfg)
    : protocol_base{flags{}}
    , m_code{code}
{
  m_thread.setObjectName("Serial Thread");
  m_thread.start();

  // Only needed for the sake of invoking a method on the QThread
  m_threadWorker = new serial_worker{m_queue};
  m_threadWorker->moveToThread(&m_thread);

  ossia::qt::run_async(m_threadWorker, [this, ctx, cfg] {
    // Serial thread

    m_engine = new QQmlEngine;
    m_component = new QQmlComponent{m_engine};
    m_engine->moveToThread(&m_thread);
    m_component->moveToThread(&m_thread);

    // Serial thread to main thread
    QObject::connect(
        m_component, &QQmlComponent::statusChanged, m_component,
        [this, ctx, cfg](QQmlComponent::Status status) {
      // Serial thread
      qDebug() << "Serial error:" << m_component->errorString();
      if(!m_device)
        return;

      switch(status)
      {
        case QQmlComponent::Status::Ready: {
          // Any call to the QQmlEngine needs to be done in its thread.
          auto res = load_serial_object_from_qml(*this, ctx, cfg);

          // Move the objects back to the main thread in one go
          ossia::qt::run_async(this, [this, res = std::move(res)]() mutable {
            // Main thread - just copying is safe as it's just a classic Qt dptr
            m_object = res.m_object;
            m_jsObj = res.m_jsObj;
            m_onTextMessage = res.m_onTextMessage;
            m_onBinaryMessage = res.m_onBinaryMessage;
            m_onRead = res.m_onRead;
            m_port = res.m_port;
            if(res.m_coalesce > 0.)
            {
              m_coalesce = res.m_coalesce;
              m_queue.callback
                  = [this](ossia::net::parameter_base& p, const ossia::value& v) {
                do_write(p, v);
              };

              ossia::qt::run_async(m_threadWorker, [this] {
                m_threadWorker->startTimer(m_coalesce.value(), Qt::PreciseTimer);
              });
            }

            // Now we can finally create our tree :-)
            ossia::qt::apply_deferred_device<
                ossia::net::device_base, serial_node, serial_parameter_data>(
                *this->m_device, res.nodes);
          });
          return;
        }
        case QQmlComponent::Status::Loading:
          return;
        case QQmlComponent::Status::Null:
        case QQmlComponent::Status::Error:
          qDebug() << m_component->errorString();
          return;
      }
        },
        Qt::DirectConnection);
  });
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
  if(!arr.isArray())
    return;

  QJSValueIterator it(arr);
  while(it.hasNext())
  {
    it.next();
    auto val = it.value();
    auto addr = val.property("address");
    if(!addr.isString())
      continue;

    // FIXME since this executes in a separate thread we have to be super
    // confident that the user cannot remove nodes while read operations are
    // taking place
    auto addr_txt = addr.toString().toStdString();
    auto n = find_node(m_device->get_root_node(), addr_txt);
    if(!n)
      continue;

    auto v = val.property("value");
    if(v.isNull())
      continue;

    if(auto addr = n->get_parameter())
    {
      // qDebug() << "Applied value"
      //          << QString::fromStdString(value_to_pretty_string(
      //                                      qt::value_from_js(addr->value(), v)));
      addr->set_value(qt::value_from_js(addr->value(), v));
    }
  }
}

void serial_protocol::do_write(
    const ossia::net::parameter_base& addr, const ossia::value& v)
{
  auto& engine = *m_engine;
  auto& port = *m_port;

  auto& ad = const_cast<serial_parameter&>(static_cast<const serial_parameter&>(addr));

  QJSValue& req = ad.data().request;

  QString str;
  if(req.isString())
  {
    str = req.toString();
  }
  else if(req.isCallable())
  {
    auto r1 = req.call({qt::value_to_js_value(v, engine)});
    if(!r1.isError())
    {
      auto var = r1.toVariant();
      if(var.typeId() == QMetaType::QByteArray)
      {
        auto ba = var.toByteArray();
        port.on_write(ba);
      }
      else
      {
        auto ba = var.toString().toUtf8();
        port.on_write(ba);
      }
      return;
    }

    auto res = req.call();
    if(res.isError())
      return;

    str = res.toString();
  }
  else
  {
    return;
  }

  // FIXME harmonize with qt::value_to_js_string_unquoted in HTTP
  switch(ad.get_value_type())
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
    case ossia::val_type::VEC2F: {
      auto& vec = v.get<ossia::vec2f>();
      str.replace("$val", QString{"%1 %2"}.arg(vec[0]).arg(vec[1]));
      break;
    }
    case ossia::val_type::VEC3F: {
      auto& vec = v.get<ossia::vec3f>();
      str.replace("$val", QString{"%1 %2 %3"}.arg(vec[0]).arg(vec[1]).arg(vec[2]));
      break;
    }
    case ossia::val_type::VEC4F: {
      auto& vec = v.get<ossia::vec4f>();
      str.replace(
          "$val",
          QString{"%1 %2 %3 %4"}.arg(vec[0]).arg(vec[1]).arg(vec[2]).arg(vec[3]));
      break;
    }
    case ossia::val_type::LIST:
      str.replace("$val", "TODO");
      break;
    case ossia::val_type::MAP:
      str.replace("$val", "TODO");
      break;
    case ossia::val_type::IMPULSE:
      str.replace("$val", "TODO");
      break;
    default:
      qDebug() << "serial_protocol::push: bad type";
  }

  port.on_write(str.toUtf8());
  return;
}

bool serial_protocol::push(const ossia::net::parameter_base& addr, const ossia::value& v)
{
  // Prevent pushes when the device has not been set yet
  if(!m_port)
    return false;

  // we have to rate limit: if the thread is busy writing messages
  // we must not overload it
  // if not critical, we just send an event to write
  // find a way to collapse events in Qt
  // -> new request -> write it down -> use the last value

  if(!this->m_coalesce)
  {
    ossia::qt::run_async(m_threadWorker, [this, &addr, v] { do_write(addr, v); });
  }
  else
  {
    auto critical = ossia::net::get_critical(addr.get_node());
    auto& q = critical ? this->m_queue.critical : this->m_queue.noncritical;
    q.enqueue(ossia::received_value{(ossia::net::parameter_base*)&addr, v});
  }

  return false;
}

bool serial_protocol::push_raw(const full_parameter_data& parameter_base)
{
  return false;
}

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
  // run in the engine thread
  ossia::qt::run_async(m_threadWorker, [this] { m_component->setData(m_code, QUrl{}); });
}

void serial_protocol::stop()
{
  // run in the engine thread
  ossia::qt::run_async(m_threadWorker, [port = m_port] {
    if(port)
      port->close();
  });
}

serial_protocol::~serial_protocol()
{
  m_thread.quit();
  m_thread.wait();

  delete m_threadWorker;
}

}
}
#endif
