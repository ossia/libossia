// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "http_protocol.hpp"

#include <ossia-qt/js_utilities.hpp>
#include <ossia-qt/qml_engine_functions.hpp>

#include <QJSValueIterator>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QQmlComponent>
#include <QQmlContext>
#include <QQmlEngine>

#include <wobjectimpl.h>

W_OBJECT_IMPL(ossia::net::http_protocol)
namespace ossia::net
{
http_protocol::http_protocol(QByteArray code)
    : protocol_base{flags{}}
    , m_engine{new QQmlEngine}
    , m_component{new QQmlComponent{m_engine}}
    , m_access{new QNetworkAccessManager}
    , m_code{code}
{
  auto obj = new ossia::qt::qml_device_engine_functions{
      {}, [](ossia::net::parameter_base& param, const ossia::value_port& v) {
    if(v.get_data().empty())
      return;
    auto& last = v.get_data().back().value;
    param.push_value(last);
  }, m_engine};
  m_engine->rootContext()->setContextProperty("Device", obj);

  QObject::connect(m_access, &QNetworkAccessManager::finished, this, [this](auto reply) {
    QNetworkReply& rep = *reply;
    auto it = m_replies.find(&rep);
    http_parameter& addr = *it->second;

    if(rep.error() != QNetworkReply::NoError)
      return;

    auto ans = addr.data().answer;
    if(ans.isCallable())
    {
      apply_reply(ans.call(
          {QString(rep.readAll()), qt::value_to_js_value(addr.value(), *m_engine)}));
    }
    // else
    // {
    //   // If parameter is string we just apply it
    //   addr.set_value(rep.readAll().toStdString());
    // }

    m_replies.erase(it);
  }, Qt::QueuedConnection);

  QObject::connect(
      m_component, &QQmlComponent::statusChanged, this,
      [this](QQmlComponent::Status status) {
    if(!m_device)
      return;

    switch(status)
    {
      case QQmlComponent::Status::Ready: {
        auto item = m_component->create();
        item->setParent(m_engine->rootContext());

        QVariant ret;
        QMetaObject::invokeMethod(item, "createTree", Q_RETURN_ARG(QVariant, ret));
        qt::create_device<ossia::net::device_base, http_node, http_protocol>(
            *m_device, ret.value<QJSValue>());

        return;
      }
      case QQmlComponent::Status::Loading:
        return;
      case QQmlComponent::Status::Null:
      case QQmlComponent::Status::Error:
        qDebug() << m_component->errorString();
        return;
    }
      });

  QObject::connect(
      this, &http_protocol::sig_push, this, &http_protocol::slot_push,
      Qt::QueuedConnection);
}

http_protocol::~http_protocol()
{
  delete m_access;
  delete m_component;
  delete m_engine;
}

bool http_protocol::update(ossia::net::node_base& node_base)
{
  return true;
}

bool http_protocol::pull(ossia::net::parameter_base& parameter_base)
{
  return true;
}

bool http_protocol::push(
    const ossia::net::parameter_base& parameter_base, const ossia::value& v)
{
  // TODO dynamic_cast or whatever
  // TODO put the http_parameters in a hash map instead?
  if(auto addr = dynamic_cast<const http_parameter*>(&parameter_base))
  {
    if(addr->data().requestIsValid())
    {
      sig_push(const_cast<http_parameter*>(addr), v);
      return true;
    }
  }
  return false;
}

bool http_protocol::push_raw(const full_parameter_data& parameter_base)
{
  return false;
}

bool http_protocol::observe(parameter_base&, bool enable)
{
  return false;
}

void http_protocol::set_device(device_base& dev)
{
  m_device = &dev;
  m_engine->findChild<ossia::qt::qml_device_engine_functions*>()->setDevice(&dev);
  m_component->setData(m_code, QUrl{});
}

void http_protocol::slot_push(http_parameter* addr_p, const ossia::value& v)
{
  auto& addr = *addr_p;
  auto& dat = addr.data();

  auto setupReply = [this, addr_p](QNetworkReply* rep) {
    auto& dat = addr_p->data();
    if(dat.onRead.isCallable() || dat.onReadString.isCallable())
    {
      QObject::connect(rep, &QNetworkReply::readyRead, this, [this, rep, addr_p] {
        const auto ba = rep->readAll();
        auto& dat = addr_p->data();
        if(dat.onRead.isCallable())
          dat.onRead.call({m_engine->toScriptValue(ba)});
        if(dat.onReadString.isCallable())
          dat.onReadString.call({m_engine->toScriptValue(QString::fromUtf8(ba))});
      });
    }
    m_replies[rep] = addr_p;
  };

  if(auto url = this->requestUrl(addr_p, v); url.isValid())
  {
    auto request_data = this->requestData(addr_p, v);
    if(dat.is_post)
    {
      setupReply(m_access->post(QNetworkRequest(url), request_data));
    }
    else
    {
#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
      if(!request_data.isEmpty())
      {
        setupReply(m_access->get(QNetworkRequest(url), request_data));
      }
      else
#endif
      {
        setupReply(m_access->get(QNetworkRequest(url)));
      }
    }
  }
}

QUrl http_protocol::requestUrl(const http_parameter* addr_p, const ossia::value& v)
{
  auto& req = addr_p->data().request;
  if(req.isString())
  {
    return QUrl{req.toString().replace("$val", qt::value_to_js_string_unquoted(v))};
  }
  else if(req.isCallable())
  {
    auto& engine = *m_engine;
    auto r1 = req.call({qt::value_to_js_value(v, engine)});
    if(!r1.isError())
    {
      auto var = r1.toVariant();
      if(var.typeId() == QMetaType::QByteArray)
      {
        return QUrl{QString{var.toByteArray()}};
      }
      else
      {
        return QUrl{QString{var.toString()}};
      }
    }

    auto res = req.call();
    if(res.isError())
      return QUrl{};

    return QUrl{res.toString().replace("$val", qt::value_to_js_string_unquoted(v))};
  }
  else
  {
    return QUrl{};
  }
}

QByteArray
http_protocol::requestData(const http_parameter* addr_p, const ossia::value& v)
{
  auto& req = addr_p->data().requestData;
  if(req.isString())
  {
    return req.toString().replace("$val", qt::value_to_js_string_unquoted(v)).toUtf8();
  }
  else if(req.isCallable())
  {
    auto& engine = *m_engine;
    auto r1 = req.call({qt::value_to_js_value(v, engine)});
    if(!r1.isError())
    {
      auto var = r1.toVariant();
      if(var.typeId() == QMetaType::QByteArray)
      {
        return var.toByteArray();
      }
      else
      {
        return var.toString().toUtf8();
      }
    }

    auto res = req.call();
    if(res.isError())
      return QByteArray{};

    return res.toString().replace("$val", qt::value_to_js_string_unquoted(v)).toUtf8();
  }
  else
  {
    return QByteArray{};
  }
}

void http_protocol::apply_reply(QJSValue arr)
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
      addr->push_value(qt::value_from_js(addr->value(), v));
    }
  }
}
}
