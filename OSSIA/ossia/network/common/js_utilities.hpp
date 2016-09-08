#pragma once
#include <ossia/editor/value/value.hpp>
#include <ossia/network/common/address_properties.hpp>
#include <ossia/network/domain/domain.hpp>
#include <ossia/network/base/node.hpp>
#include <ossia/network/generic/generic_address.hpp>
#include <QStringBuilder>
#include <QHash>
#include <QString>
#include <QJSValue>
#include <QJSValueIterator>
#include <QJSEngine>
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


struct js_value_inbound_visitor
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

  ossia::value operator()(const Destination& t) { return t; }
  ossia::value operator()(const Behavior& t) { return t; }
  ossia::value operator()() const { return {}; }

};

inline ossia::value value_from_jsvalue(ossia::value cur, const QJSValue& v)
{
  return cur.apply(js_value_inbound_visitor{v});
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

    dat.domain = domain;
    dat.access = get_enum<ossia::access_mode>(js.property("access"));
    dat.bounding = get_enum<ossia::bounding_mode>(js.property("bounding"));
    dat.repetition_filter = get_enum<ossia::repetition_filter>(js.property("repetition_filter"));
  }

  return dat;
}



inline QJSValue value_to_js_value(const ossia::value& cur, QJSEngine& engine);

struct js_value_outbound_visitor
{
  QJSEngine& engine;

  QJSValue to_enum(qml_context::val_type t) const
  {
    return engine.toScriptValue(QVariant::fromValue(t));
  }

  QJSValue operator()(Impulse) const
  {
    QJSValue v;
    v.setProperty("type", to_enum(qml_context::val_type::Impulse));
    return v;
  }

  QJSValue operator()(Int val) const
  {
    QJSValue v;
    v.setProperty("type", to_enum(qml_context::val_type::Int));
    v.setProperty("value", val.value);
    return v;
  }
  QJSValue operator()(Float val) const
  {
    QJSValue v;
    v.setProperty("type", to_enum(qml_context::val_type::Float));
    v.setProperty("value", val.value);
    return v;
  }
  QJSValue operator()(Bool val) const
  {
    QJSValue v;
    v.setProperty("type", to_enum(qml_context::val_type::Bool));
    v.setProperty("value", val.value);
    return v;
  }
  QJSValue operator()(Char val) const
  {
    QJSValue v;
    v.setProperty("type", to_enum(qml_context::val_type::Char));
    v.setProperty("value", val.value);
    return v;
  }

  QJSValue operator()(const String& val) const
  {
    QJSValue v;
    v.setProperty("type", to_enum(qml_context::val_type::String));
    v.setProperty("value", QString::fromStdString(val.value));
    return v;
  }

  QJSValue make_tuple(const std::vector<ossia::value>& arr) const
  {
    auto array = engine.newArray(arr.size());
    int i = 0;

    for(const auto& child : arr)
    {
      array.setProperty(i++, value_to_js_value(child, engine));
    }

    return array;
  }

  QJSValue operator()(const Tuple& val) const
  {
    QJSValue v;
    v.setProperty("type", to_enum(qml_context::val_type::Tuple));
    v.setProperty("value", make_tuple(val.value));
    return v;
  }

  template<std::size_t N>
  QJSValue make_array(const std::array<float, N>& arr) const
  {
    auto array = engine.newArray(arr.size());
    int i = 0;

    for(auto child : arr)
    {
      array.setProperty(i++, child);
    }

    return array;
  }

  QJSValue operator()(Vec2f val) const
  {
    QJSValue v;
    v.setProperty("type", to_enum(qml_context::val_type::Vec2f));
    v.setProperty("value", make_array(val.value));
    return v;
  }
  QJSValue operator()(Vec3f val) const
  {
    QJSValue v;
    v.setProperty("type", to_enum(qml_context::val_type::Vec3f));
    v.setProperty("value", make_array(val.value));
    return v;
  }
  QJSValue operator()(Vec4f val) const
  {
    QJSValue v;
    v.setProperty("type", to_enum(qml_context::val_type::Vec4f));
    v.setProperty("value", make_array(val.value));
    return v;
  }

  QJSValue operator()(const Destination& t) { return {}; }
  QJSValue operator()(const Behavior& t) { return {}; }
  QJSValue operator()() const { return {}; }

};

inline QJSValue value_to_js_value(const ossia::value& cur, QJSEngine& engine)
{
  return cur.apply(js_value_outbound_visitor{engine});
}


inline QString value_to_js_string(const ossia::value& cur);
struct js_string_outbound_visitor
{
  QString operator()(Impulse) const
  {
    return QStringLiteral("\"\"");
  }

  QString operator()(Int val) const
  {
    return QString::number(val.value);
  }

  QString operator()(Float val) const
  {
    return QString::number(val.value);
  }
  QString operator()(Bool val) const
  {
    return val.value ? QStringLiteral("true") : QStringLiteral("false");
  }
  QString operator()(Char val) const
  {
    return "\"" % QString{val.value} % "\"";
  }

  QString operator()(const String& val) const
  {
    return "\"" % QString::fromStdString(val.value) % "\"";
  }

  QString operator()(const Tuple& val) const
  {
    QString s = "[";

    std::size_t n = val.value.size();
    if(n != 0)
    {
      s += value_to_js_string(val.value[0]);
      for(std::size_t i = 1; i < n; i++)
      {
        s += ", " % value_to_js_string(val.value[i]);
      }
    }

    s += "]";
    return s;
  }

  template<std::size_t N>
  QString make_array(const std::array<float, N>& arr) const
  {
    static_assert(N > 0, "N <= 0");
    QString s = "[";

    s += QString::number(arr[0]);
    for(std::size_t i = 1; i < N; i++)
    {
      s += ", " % QString::number(arr[i]);
    }
    s += "]";
    return s;
  }

  QString operator()(Vec2f val) const
  {
    return make_array(val.value);
  }
  QString operator()(Vec3f val) const
  {
    return make_array(val.value);
  }
  QString operator()(Vec4f val) const
  {
    return make_array(val.value);
  }

  QString operator()(const Destination& t) { return (*this)(Impulse{}); }
  QString operator()(const Behavior& t) { return (*this)(Impulse{}); }
  QString operator()() const { return (*this)(Impulse{}); }

};

inline QString value_to_js_string(const ossia::value& cur)
{
  return cur.apply(js_string_outbound_visitor{});
}

}
}

Q_DECLARE_METATYPE(ossia::net::qml_context)
