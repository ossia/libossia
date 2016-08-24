#pragma once
#include <ossia/editor/value/value.hpp>
#include <ossia/network/common/address_properties.hpp>
#include <ossia/network/domain/domain.hpp>
#include <ossia/network/base/node.hpp>
#include <ossia/network/generic/generic_address.hpp>
#include <QHash>
#include <QString>
#include <QJSValue>
#include <QJSValueIterator>
#include <QMetaEnum>
#include <boost/optional.hpp>

namespace ossia
{
namespace net
{

template<typename Device_T, typename Node_T, typename Protocol_T>
void create_device(Device_T& device, QJSValue root);
template<typename Device_T, typename Node_T, typename Protocol_T>
void create_node_rec(QJSValue js, Device_T& device, Node_T& parent);

template<typename Device_T, typename Node_T, typename Protocol_T>
void create_device(Device_T& device, QJSValue root)
{
  if(!root.isArray())
    return;

  QJSValueIterator it(root);
  while (it.hasNext()) {
    it.next();
    create_node_rec<Device_T, Node_T, Protocol_T>(it.value(), device, static_cast<Node_T&>(device));
  }
}


template<typename Device_T, typename Node_T, typename Protocol_T>
void create_node_rec(QJSValue js, Device_T& device, Node_T& parent)
{
  auto data = Protocol_T::read_data(js);
  if(data.node_name.empty())
    return;

  auto node = new Node_T{data, device, parent};
  parent.add_child(std::unique_ptr<ossia::net::node_base>(node));

  device.onNodeCreated(*node);

  QJSValue children = js.property("children");
  if(!children.isArray())
    return;

  QJSValueIterator it(children);
  while (it.hasNext()) {
    it.next();
    create_node_rec<Device_T, Node_T, Protocol_T>(it.value(), device, *node);
  }
}


struct js_value_visitor
{
  const QJSValue& val;
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

inline ossia::value value_from_jsvalue(ossia::value cur, const QJSValue& v)
{
  return cur.apply(js_value_visitor{v});
}

struct qml_context
{
private:
  Q_GADGET

public:
  // QML enums have to begin with a capital
  enum class val_type
  {
    Impulse, //! \see ossia::Impulse
    Bool, //! \see ossia::Bool
    Int, //! \see ossia::Int
    Float, //! \see ossia::Float
    Char, //! \see ossia::Char
    String, //! \see ossia::String
    Tuple, //! \see ossia::Tuple
    Vec2f, //! \see ossia::Vec2f
    Vec3f, //! \see ossia::Vec3f
    Vec4f, //! \see ossia::Vec4f
    Destination, //! \see ossia::Destination
    Behavior,    //! \see ossia::Behavior
  };

  enum class access_mode
  {
    Get,
    Set,
    Bi
  };

  enum class bounding_mode
  {
    Free,
    Clip,
    Wrap,
    Fold,
    Low,
    High
  };

  enum class repetition_filter
  {
    On,
    Off
  };

  Q_ENUM(val_type)
  Q_ENUM(access_mode)
  Q_ENUM(bounding_mode)
  Q_ENUM(repetition_filter)
};
template<typename T>
struct matching_ossia_enum
{

};
template<>
struct matching_ossia_enum<ossia::val_type>
{
  using type = qml_context::val_type;
};
template<>
struct matching_ossia_enum<ossia::access_mode>
{
  using type = qml_context::access_mode;
};
template<>
struct matching_ossia_enum<ossia::bounding_mode>
{
  using type = qml_context::bounding_mode;
};
template<>
struct matching_ossia_enum<ossia::repetition_filter>
{
  using type = qml_context::repetition_filter;
};

template<typename T>
boost::optional<T> get_enum(const QJSValue& val)
{
  if(val.isNumber())
  {
    int n = val.toInt();
    if(n >= 0 && n < QMetaEnum::fromType<typename matching_ossia_enum<T>::type>().keyCount())
    {
      return static_cast<T>(n);
    }
  }
  return {};
}
inline boost::optional<ossia::val_type> to_ossia_type(int t)
{
  if(t >= 0 && t < 10)
    return static_cast<ossia::val_type>(t);
  return {};
}

inline generic_address_data make_address_data(const QJSValue& js)
{
  generic_address_data dat;

  QJSValue name = js.property("name");
  if(name.isString())
  {
    dat.node_name = name.toString().toStdString();
  }
  else
  {
    return dat;
  }

  dat.type = get_enum<ossia::val_type>(js.property("type"));

  if(dat.type)
  {
    auto base_v = init_value(*dat.type);
    auto domain = init_domain(*dat.type);
    set_min(domain, value_from_jsvalue(base_v, js.property("min")));
    set_max(domain, value_from_jsvalue(base_v, js.property("max")));

    dat.access = get_enum<ossia::access_mode>(js.property("access"));
    dat.bounding = get_enum<ossia::bounding_mode>(js.property("bounding"));
    dat.repetition_filter = get_enum<ossia::repetition_filter>(js.property("repetition_filter"));
  }

  return dat;
}

}
}

Q_DECLARE_METATYPE(ossia::net::qml_context)
