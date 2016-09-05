#include "ws_generic_client_protocol.hpp"
#include <ossia/network/websocket-generic-client/ws_generic_client_address_data.hpp>
#include <ossia/network/websocket-generic-client/ws_generic_client_device.hpp>
#include <ossia/network/websocket-generic-client/ws_generic_client_address.hpp>
#include <ossia/network/websocket-generic-client/ws_generic_client_node.hpp>
#include <ossia/network/common/js_utilities.hpp>


#include <QQmlEngine>
#include <QQmlComponent>
#include <QQmlContext>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJSValueIterator>
namespace ossia
{
namespace net
{
ws_generic_client_protocol::ws_generic_client_protocol(QByteArray code):
  mEngine{new QQmlEngine},
  mComponent{new QQmlComponent{mEngine}},
  mWebsocket{new QWebSocket},
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
        auto item = mComponent->create();
        item->setParent(mEngine->rootContext());

        QVariant ret;
        QMetaObject::invokeMethod(item, "createTree", Q_RETURN_ARG(QVariant, ret));
        create_device<ws_generic_client_device, ws_generic_client_node, ws_generic_client_protocol>(*mDevice, ret.value<QJSValue>());

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

}

bool ws_generic_client_protocol::update(ossia::net::node_base& node_base)
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

  if(!addr.data().request.isEmpty())
  {
    emit sig_push(&addr);
    return true;
  }

  return false;
}

bool ws_generic_client_protocol::observe(address_base& address_base, bool enable)
{
  return false;

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
  auto rep = mWebsocket->get(QNetworkRequest(addr.data().request));

  auto pair = std::make_pair(rep, &addr);

  mReplies.push_back(pair);

  connect(rep, &QNetworkReply::readyRead,
          this, [=] ()
  {
    QNetworkReply& rep = *pair.first;
    const ws_generic_client_address& addr = *pair.second;

    auto ans = addr.data().answer;
    if(ans.isCallable())
    {
      apply_reply(ans.call({QString(rep.readAll())}));
    }

    mReplies.removeAll(pair);
  }, Qt::QueuedConnection);
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
      addr->pushValue(value_from_jsvalue(addr->cloneValue(), v));
    }
  }
}

}
}
