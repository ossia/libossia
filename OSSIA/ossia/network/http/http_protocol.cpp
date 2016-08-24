#include "http_protocol.hpp"
#include <ossia/network/http/http_address_data.hpp>
#include <ossia/network/http/http_device.hpp>
#include <ossia/network/http/http_address.hpp>
#include <ossia/network/http/http_node.hpp>
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
http_protocol::http_protocol(QByteArray code):
  mEngine{new QQmlEngine},
  mComponent{new QQmlComponent{mEngine}},
  mAccessManager{new QNetworkAccessManager},
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
        create_device<http_device, http_node, http_protocol>(*mDevice, ret.value<QJSValue>());

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

  connect(this, &http_protocol::sig_push, this, &http_protocol::slot_push, Qt::QueuedConnection);

}

http_protocol::~http_protocol()
{

}

bool http_protocol::update(ossia::net::node_base& node_base)
{
  return true;

}

bool http_protocol::pull(ossia::net::address_base& address_base)
{
  return true;

}

bool http_protocol::push(const ossia::net::address_base& address_base)
{
  auto& addr = static_cast<const http_address&>(address_base);

  if(!addr.data().request.isEmpty())
  {
    emit sig_push(&addr);
    return true;
  }

  return false;
}

bool http_protocol::observe(address_base& address_base, bool enable)
{
  return false;

}

void http_protocol::setDevice(device_base& dev)
{
  if(auto htdev = dynamic_cast<http_device*>(&dev))
  {
    mDevice = htdev;
    mComponent->setData(mCode, QUrl{});
  }
}

void http_protocol::slot_push(const http_address* addr_p)
{
  auto& addr = *addr_p;
  auto rep = mAccessManager->get(QNetworkRequest(addr.data().request));

  auto pair = std::make_pair(rep, &addr);

  mReplies.push_back(pair);

  connect(rep, &QNetworkReply::readyRead,
          this, [=] ()
  {
    QNetworkReply& rep = *pair.first;
    const http_address& addr = *pair.second;

    auto ans = addr.data().answer;
    if(ans.isCallable())
    {
      apply_reply(ans.call({QString(rep.readAll())}));
    }

    mReplies.removeAll(pair);
  }, Qt::QueuedConnection);
}

void http_protocol::apply_reply(QJSValue arr)
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
