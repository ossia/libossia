// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "http_protocol.hpp"
#include <ossia-qt/js_utilities.hpp>

#include <QJSValueIterator>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QQmlComponent>
#include <QQmlContext>
#include <QQmlEngine>
#include <wobjectimpl.h>
W_OBJECT_IMPL(ossia::net::http_protocol)
namespace ossia
{
namespace net
{
http_protocol::http_protocol(QByteArray code)
    : m_engine{new QQmlEngine}
    , m_component{new QQmlComponent{m_engine}}
    , m_access{new QNetworkAccessManager}
    , m_code{code}
{
  connect(
      m_component, &QQmlComponent::statusChanged, this,
      [=](QQmlComponent::Status status) {
        if (!m_device)
          return;

        switch (status)
        {
          case QQmlComponent::Status::Ready:
          {
            auto item = m_component->create();
            item->setParent(m_engine->rootContext());

            QVariant ret;
            QMetaObject::invokeMethod(
                item, "createTree", Q_RETURN_ARG(QVariant, ret));
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

  connect(
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

bool http_protocol::push(const ossia::net::parameter_base& parameter_base)
{
  auto& addr = static_cast<const http_parameter&>(parameter_base);

  if (!addr.data().request.isEmpty())
  {
    sig_push(&addr);
    return true;
  }

  return false;
}

bool http_protocol::push_raw(const full_parameter_data& parameter_base)
{ return false; }

bool http_protocol::observe(parameter_base&, bool enable)
{
  return false;
}

void http_protocol::set_device(device_base& dev)
{
  m_device = &dev;
  m_component->setData(m_code, QUrl{});
}

void http_protocol::slot_push(const http_parameter* addr_p)
{
  auto& addr = *addr_p;
  auto dat = addr.data().request;
  auto rep = m_access->get(QNetworkRequest(
      dat.replace("$val", qt::value_to_js_string(addr.value()))));

  auto pair = std::make_pair(rep, &addr);

  m_replies.push_back(pair);

  connect(
      rep, &QNetworkReply::readyRead, this,
      [=]() {
        QNetworkReply& rep = *pair.first;
        const http_parameter& addr = *pair.second;

        auto ans = addr.data().answer;
        if (ans.isCallable())
        {
          apply_reply(ans.call({QString(rep.readAll())}));
        }

        m_replies.removeAll(pair);
      },
      Qt::QueuedConnection);
}

void http_protocol::apply_reply(QJSValue arr)
{
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
    auto n = ossia::net::find_node(m_device->get_root_node(), addr_txt);
    if (!n)
      continue;

    auto v = val.property("value");
    if (v.isNull())
      continue;

    if (auto addr = n->get_parameter())
    {
      addr->push_value(qt::value_from_js(addr->value(), v));
    }
  }
}
}
}
