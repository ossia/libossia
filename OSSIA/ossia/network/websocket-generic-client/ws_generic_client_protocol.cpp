#include "ws_generic_client_protocol.hpp"
#include <ossia/network/websocket-generic-client/ws_generic_client_address_data.hpp>
#include <ossia/network/websocket-generic-client/ws_generic_client_device.hpp>
#include <ossia/network/websocket-generic-client/ws_generic_client_address.hpp>
#include <ossia/network/websocket-generic-client/ws_generic_client_node.hpp>
#include <ossia/network/common/js_utilities.hpp>


#include <QQmlEngine>
#include <QQmlComponent>
#include <QQmlContext>
#include <QWebSocket>
#include <QJSValueIterator>
namespace ossia
{
namespace net
{
ws_generic_client_protocol::ws_generic_client_protocol(const QString& addr, QByteArray code):
  mEngine{new QQmlEngine},
  mComponent{new QQmlComponent{mEngine}},
  mWebsocket{new QWebSocket{"ossia-api"}},
  mCode{code}
{
  connect(mComponent, &QQmlComponent::statusChanged,
          this, [=] (QQmlComponent::Status status)
  {
    if(!mDevice)
      return;

    switch(status)
    {
      case QQmlComponent::Status::Ready:
      {
        mItem = mComponent->create();
        mItem->setParent(mEngine->rootContext());

        QVariant ret;
        QMetaObject::invokeMethod(mItem, "createTree", Q_RETURN_ARG(QVariant, ret));
        create_device<ws_generic_client_device, ws_generic_client_node, ws_generic_client_protocol>(*mDevice, ret.value<QJSValue>());

        mWebsocket->open(addr);
        connect(mWebsocket, &QWebSocket::binaryMessageReceived,
                this, [=] (const QByteArray& arr) {
          qDebug() << "array" << arr;
          QVariant ret;
          QMetaObject::invokeMethod(mItem, "onMessage", Q_RETURN_ARG(QVariant, ret), Q_ARG(QVariant, QString(arr)));
          apply_reply(ret.value<QJSValue>());
        });
        connect(mWebsocket, &QWebSocket::textMessageReceived,
                this, [=] (const QString& mess) {
          qDebug() << "text" << mess;
          QVariant ret;
          QMetaObject::invokeMethod(mItem, "onMessage", Q_RETURN_ARG(QVariant, ret), Q_ARG(QVariant, mess));
          apply_reply(ret.value<QJSValue>());
        });
        return;
      }
      case QQmlComponent::Status::Loading:
        return;
      case QQmlComponent::Status::Null:
      case QQmlComponent::Status::Error:
        qDebug() << mComponent->errorString();
        return;
    }
  }
  );

  connect(this, &ws_generic_client_protocol::sig_push, this, &ws_generic_client_protocol::slot_push, Qt::QueuedConnection);

}

ws_generic_client_protocol::~ws_generic_client_protocol()
{
    // TODO also delete it in others.
    delete mEngine;
    delete mWebsocket;
}

bool ws_generic_client_protocol::update(
    ossia::net::node_base& node_base)
{
  return true;
}

bool ws_generic_client_protocol::pull(ossia::net::address_base& address_base)
{
  return true;
}

bool ws_generic_client_protocol::push(const ossia::net::address_base& address_base)
{
  auto& addr = static_cast<const ws_generic_client_address&>(address_base);

  if(!addr.data().request.isNull())
  {
    emit sig_push(&addr);
    return true;
  }

  return false;
}

bool ws_generic_client_protocol::observe(ossia::net::address_base& addr, bool enable)
{
  auto a = QString::fromStdString(address_string_from_node(addr));
  if(enable)
  {
    QMetaObject::invokeMethod(mItem,
                              "openListening",
                              Q_ARG(QVariant, a));
  }
  else
  {
    QMetaObject::invokeMethod(mItem,
                              "closeListening",
                              Q_ARG(QVariant, a));
  }
  return true;
}

void ws_generic_client_protocol::setDevice(device_base& dev)
{
  if(auto htdev = dynamic_cast<ws_generic_client_device*>(&dev))
  {
    mDevice = htdev;
    mComponent->setData(mCode, QUrl{});
  }
}

void ws_generic_client_protocol::slot_push(const ws_generic_client_address* addr_p)
{
  auto& addr = *addr_p;
  auto dat = addr.data().request;
  if(dat.isCallable())
  {
      auto res = dat.call({value_to_js_value(addr.cloneValue(), *mEngine)});
      mWebsocket->sendBinaryMessage(res.toVariant().toByteArray());
  }
  else
  {
      if(dat.isString())
      {
        qDebug() << "senidng" << dat.toString().replace("$val", value_to_js_string(addr.cloneValue()));
        mWebsocket->sendTextMessage(
              dat.toString().replace("$val", value_to_js_string(addr.cloneValue())));
      }
  }
}

void ws_generic_client_protocol::apply_reply(QJSValue arr)
{
  // should be an array of { address, value } objects
  if(!arr.isArray())
    return;

  QJSValueIterator it(arr);
  while (it.hasNext()) {
    it.next();
    auto val = it.value();
    auto addr = val.property("address");
    if(!addr.isString())
      continue;

    auto addr_txt = addr.toString().toStdString();
    auto n = find_node(*mDevice, addr_txt);
    if(!n)
      continue;

    auto v = val.property("value");
    if(v.isNull())
      continue;

    if(auto addr = n->getAddress())
    {
      qDebug() << "Applied value" << QString::fromStdString(value_to_pretty_string(value_from_jsvalue(addr->cloneValue(), v)));
      addr->pushValue(value_from_jsvalue(addr->cloneValue(), v));
    }
  }
}

}
}
