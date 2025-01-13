// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "ws_generic_client_protocol.hpp"

#include <ossia-qt/js_utilities.hpp>
#include <ossia-qt/qml_engine_functions.hpp>

#include <QJSValueIterator>
#include <QQmlComponent>
#include <QQmlContext>
#include <QQmlEngine>
#include <QWebSocket>

#include <wobjectimpl.h>
W_OBJECT_IMPL(ossia::net::ws_generic_client_protocol)
W_OBJECT_IMPL(ossia::net::WS)
namespace ossia
{
namespace net
{
ws_generic_client_protocol::ws_generic_client_protocol(
    const QString& addr, QByteArray code)
    : protocol_base{flags{}}
    , m_engine{new QQmlEngine}
    , m_component{new QQmlComponent{m_engine}}
    , m_websocket{new QWebSocket{"ossia-api"}}
    , m_code{std::move(code)}
{
  auto obj = new ossia::qt::qml_device_engine_functions{
      {}, [](ossia::net::parameter_base& param, const ossia::value_port& v) {
    if(v.get_data().empty())
      return;
    auto& last = v.get_data().back().value;
    param.push_value(last);
  }, m_engine};
  m_engine->rootContext()->setContextProperty("Device", obj);

  QObject::connect(
      m_component, &QQmlComponent::statusChanged, this,
      [this, addr](QQmlComponent::Status status) {
    if(!m_device)
      return;

    switch(status)
    {
      case QQmlComponent::Status::Ready:
        on_ready(addr);
        return;
      case QQmlComponent::Status::Loading:
        return;
      case QQmlComponent::Status::Null:
      case QQmlComponent::Status::Error:
        qDebug() << m_component->errorString();
        return;
    }
      });

  QObject::connect(
      this, &ws_generic_client_protocol::sig_push, this,
      &ws_generic_client_protocol::slot_push, Qt::QueuedConnection);
}

ws_generic_client_protocol::~ws_generic_client_protocol()
{
  // TODO also delete it in others.
  delete m_engine;
  delete m_websocket;
}

bool ws_generic_client_protocol::update(ossia::net::node_base& node_base)
{
  return true;
}

bool ws_generic_client_protocol::pull(ossia::net::parameter_base& parameter_base)
{
  return true;
}

bool ws_generic_client_protocol::push(
    const ossia::net::parameter_base& parameter_base, const ossia::value& v)
{
  assert(dynamic_cast<const ws_generic_client_parameter*>(&parameter_base));
  auto& addr = static_cast<const ws_generic_client_parameter&>(parameter_base);

  if(!addr.data().request.isNull())
  {
    sig_push(&addr, v);
    return true;
  }

  return false;
}

bool ws_generic_client_protocol::push_raw(const full_parameter_data& parameter_base)
{
  return false;
}

bool ws_generic_client_protocol::observe(
    ossia::net::parameter_base& addr_base, bool enable)
{
  assert(dynamic_cast<const ws_generic_client_parameter*>(&addr_base));
  auto& addr = static_cast<ws_generic_client_parameter&>(addr_base);
  if(enable)
  {
    auto& fun = addr.data().openListening;
    if(fun.isCallable())
      fun.call();
  }
  else
  {
    auto& fun = addr.data().closeListening;
    if(fun.isCallable())
      fun.call();
  }
  return true;
}

void ws_generic_client_protocol::set_device(device_base& dev)
{
  m_device = &dev;
  m_engine->findChild<ossia::qt::qml_device_engine_functions*>()->setDevice(&dev);
  m_component->setData(m_code, QUrl{});
}

void ws_generic_client_protocol::slot_push(
    const ws_generic_client_parameter* addr_p, const ossia::value& v)
{
  auto& addr = *addr_p;
  auto dat = addr.data().request;
  if(dat.isCallable())
  {
    auto res = dat.call({qt::value_to_js_value(v, *m_engine)});
    m_websocket->sendBinaryMessage(res.toVariant().toByteArray());
  }
  else
  {
    if(dat.isString())
    {
      //      qDebug() << "sending"
      //               << dat.toString().replace(
      //                      "$val", qt::value_to_js_string(addr.value()));
      m_websocket->sendTextMessage(
          dat.toString().replace("$val", qt::value_to_js_string(addr.value())));
    }
  }
}

void ws_generic_client_protocol::on_ready(const QString& host)
{
  auto obj = m_component->create();
  m_object = qobject_cast<WS*>(obj);
  if(!m_object)
  {
    qDebug() << "Invalid object type for WebSocket: " << obj;
    return;
  }

  m_object->setParent(m_engine->rootContext());

  // Create the ossia tree from the spec given in QML
  QVariant ret;
  QMetaObject::invokeMethod(m_object, "createTree", Q_RETURN_ARG(QVariant, ret));
  qt::create_device<
      ossia::net::device_base, ws_generic_client_node, ws_generic_client_protocol>(
      *m_device, ret.value<QJSValue>());

  // Websocket management
  m_websocket->open(host);

  QObject::connect(m_websocket, &QWebSocket::connected, this, [this] {
    QVariant ret;
    QMetaObject::invokeMethod(m_object, "onConnected", Q_RETURN_ARG(QVariant, ret));
    apply_reply(ret.value<QJSValue>());
  });
  QObject::connect(m_websocket, &QWebSocket::disconnected, this, [this] {
    QVariant ret;
    QMetaObject::invokeMethod(m_object, "onDisonnected", Q_RETURN_ARG(QVariant, ret));
    apply_reply(ret.value<QJSValue>());
  });

  // Messages from the server
  QObject::connect(
      m_websocket, &QWebSocket::binaryMessageReceived, this,
      [this](const QByteArray& arr) {
    if(m_object->processFromJson())
    {
      auto str = arr.toStdString();
      ossia::presets::apply_json(str, this->m_device->get_root_node());
    }
    else
    {
      QVariant ret;
      QMetaObject::invokeMethod(
          m_object, "onMessage", Q_RETURN_ARG(QVariant, ret),
          Q_ARG(QVariant, QString(arr)));
      apply_reply(ret.value<QJSValue>());
    }
      });

  QObject::connect(
      m_websocket, &QWebSocket::textMessageReceived, this, [this](const QString& mess) {
        if(m_object->processFromJson())
        {
          auto str = mess.toStdString();
          ossia::presets::apply_json(str, this->m_device->get_root_node());
        }
        else
        {
          QVariant ret;
          QMetaObject::invokeMethod(
              m_object, "onMessage", Q_RETURN_ARG(QVariant, ret), Q_ARG(QVariant, mess));
          apply_reply(ret.value<QJSValue>());
        }
      });

  return;
}

void ws_generic_client_protocol::apply_reply(QJSValue arr)
{
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

    auto addr_txt = addr.toString().toStdString();
    auto n = ossia::net::find_node(m_device->get_root_node(), addr_txt);
    if(!n)
      continue;

    auto v = val.property("value");
    if(v.isNull())
      continue;

    if(auto addr = n->get_parameter())
    {
      //      qDebug() << "Applied value"
      //               << QString::fromStdString(value_to_pretty_string(
      //                      qt::value_from_js(addr->value(), v)));
      addr->push_value(qt::value_from_js(addr->value(), v));
    }
  }
}
}
}
