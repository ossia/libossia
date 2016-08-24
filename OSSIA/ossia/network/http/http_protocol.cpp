#include "http_protocol.hpp"
namespace ossia
{
namespace net
{

http_node::http_node(http_address_data data, http_device& aDevice, http_node& aParent):
  mName{data.name},
  mDevice{aDevice},
  mParent{&aParent}
{
  if(!data.request.isEmpty() || data.t)
    mAddress = std::make_unique<http_address>(data, *this);
}

http_node::http_node(http_address_data data, http_device& aDevice):
  mName{data.name},
  mDevice{aDevice}
{
  if(!data.request.isEmpty() || data.t)
    mAddress = std::make_unique<http_address>(data, *this);
}

http_node::~http_node()
{

}

device_base&http_node::getDevice() const
{ return mDevice; }

node_base*http_node::getParent() const
{ return mParent; }

std::string http_node::getName() const
{ return mName.toStdString(); }

node_base&http_node::setName(std::string)
{ return *this; }

address_base* http_node::getAddress() const
{ return mAddress.get(); }

address_base*http_node::createAddress(val_type)
{ return nullptr; }

bool http_node::removeAddress()
{ return false; }

std::unique_ptr<node_base> http_node::makeChild(const std::string& name)
{ return nullptr; }

void http_node::removingChild(node_base& node_base)
{ }

http_address::http_address(http_address_data data, const node_base& node_base):
  generic_address{node_base},
  mData{data}
{
  if(mData.t)
  {
    mValueType = *mData.t;
    mValue = init_value(mValueType);
    this->setAccessMode(ossia::access_mode::BI);
  }
}

http_address::~http_address()
{
  callback_container<value_callback>::callbacks_clear();
}

http_device::http_device(std::unique_ptr<http_protocol> protocol, std::string name):
  device_base(std::move(protocol)),
  http_node(http_address_data{QString::fromStdString(name), {}, {}, {}}, *this)
{
  mProtocol->setDevice(*this);
}

http_device::~http_device()
{

}

http_protocol::http_protocol(QByteArray code):
  mEngine{new QQmlEngine},
  mComponent{new QQmlComponent{mEngine}},
  mAccessManager{new QNetworkAccessManager},
  mCode{code}
{
  connect(mComponent, &QQmlComponent::statusChanged,
          this, [=] (QQmlComponent::Status status) {
    switch(status)
    {
      case QQmlComponent::Status::Null:
      case QQmlComponent::Status::Ready:
      {
        auto item = mComponent->create();
        item->setParent(mEngine->rootContext());

        QVariant ret;
        QMetaObject::invokeMethod(item, "createTree", Q_RETURN_ARG(QVariant, ret));
        create_device(ret.value<QJSValue>());

        return;
      }
      case QQmlComponent::Status::Loading:
        return;
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

      auto ans = addr.data().answerer;
      if(ans.isCallable())
      {
        apply_reply(ans.call({QString(rep.readAll())}));
      }

      mReplies.removeAll(pair);
    }, Qt::QueuedConnection);
}

const QHash<QString, ossia::val_type> js_type_map
{
  { "impulse", ossia::val_type::IMPULSE },
  { "bool", ossia::val_type::BOOL },
  { "int", ossia::val_type::INT },
  { "float", ossia::val_type::FLOAT },
  { "string", ossia::val_type::STRING },
  { "char", ossia::val_type::CHAR },
  { "tuple", ossia::val_type::TUPLE },
  { "vec2f", ossia::val_type::VEC2F },
  { "vec3f", ossia::val_type::VEC3F },
  { "vec4f", ossia::val_type::VEC4F }
};
boost::optional<ossia::val_type> to_ossia_type(QString s)
{
  auto it = js_type_map.find(s);
  if(it != js_type_map.end())
    return *it;
  return {};
}

http_address_data make_data(QJSValue& js)
{
  http_address_data dat;

  QJSValue name = js.property("name");
  if(name.isString())
  {
    dat.name = name.toString();

    QJSValue request = js.property("request");
    if(request.isString())
    {
      dat.request = request.toString();

      QJSValue answer = js.property("answer");
      if(answer.isCallable())
      {
        dat.answerer = answer;
      }
    }

    QJSValue type = js.property("type");
    if(type.isString())
    {
      dat.t = to_ossia_type(type.toString());
    }
  }

  return dat;
}


void http_protocol::create_device(QJSValue root)
{
  if(!root.isArray())
    return;

  QJSValueIterator it(root);
  while (it.hasNext()) {
      it.next();
      create_node_rec(it.value(), *mDevice);
  }
}

void http_protocol::create_node_rec(QJSValue js, http_node& parent)
{
  auto data = make_data(js);
  if(data.name.isEmpty())
    return;

  auto node = new http_node{data, *mDevice, parent};
  parent.mChildren.push_back(std::unique_ptr<ossia::net::node_base>(node));

  mDevice->onNodeCreated(*node);

  QJSValue children = js.property("children");
  if(!children.isArray())
    return;

  QJSValueIterator it(children);
  while (it.hasNext()) {
    it.next();
    create_node_rec(it.value(), *node);
  }
}

struct js_value_visitor
{
  QJSValue& val;
public:
  ossia::value operator()(Impulse) const
  {
    return Impulse{};
  }

  ossia::value operator()(Int v) const
  {
    return ossia::Int(val.toInt());
  }
  ossia::value operator()(Float v) const
  {
    return ossia::Float(val.toNumber());
  }
  ossia::value operator()(Bool v) const
  {
    return ossia::Bool(val.toBool());
  }
  ossia::value operator()(Char v) const
  {
    auto str = val.toString();
    if(str.size() > 0)
      return ossia::Char(str[0].toLatin1());
    return v;
  }

  ossia::value operator()(const String& v) const
  {
    return ossia::String(val.toString().toStdString());
  }

  ossia::value operator()(const Tuple& v) const
  {
    return v; // TODO
  }

  ossia::value operator()(Vec2f v) const
  {
    return v; // TODO
  }
  ossia::value operator()(Vec3f v) const
  {
    return v; // TODO
  }
  ossia::value operator()(Vec4f v) const
  {
    return v; // TODO
  }

  template<typename T>
  ossia::value operator()(T&& t) { return ossia::value(std::forward<T>(t)); }
  ossia::value operator()() const { return {}; }

};

ossia::value from_json(ossia::value cur, QJSValue v)
{
  return cur.apply(js_value_visitor{v});
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
      addr->pushValue(from_json(addr->cloneValue(), v));
    }
  }
}

}
}
