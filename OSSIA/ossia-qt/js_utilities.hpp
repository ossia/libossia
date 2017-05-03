#pragma once
#if defined(QT_CORE_LIB)
#include <ossia/editor/value/value.hpp>
#include <ossia/network/common/address_properties.hpp>
#include <ossia/network/domain/domain.hpp>
#include <ossia/network/base/node.hpp>
#include <ossia/network/generic/generic_address.hpp>
#include <ossia/editor/dataspace/dataspace_visitors.hpp>
#include <ossia-c/preset/preset.hpp>
#include <ossia-qt/metatypes.hpp>
#include <QStringBuilder>
#include <QHash>
#include <QString>
#include <QJSValue>
#include <QJSValueIterator>
#include <QJSEngine>
#include <QMetaEnum>
#include <QTime>
#include <QPoint>
#include <QLineF>
#include <QRectF>
#include <QVariantList>
#include <QQmlProperty>
#include <QQmlExtensionPlugin>
#include <QtGui/QColor>
#include <QtGui/QVector2D>
#include <QtGui/QVector3D>
#include <QtGui/QVector4D>
#include <QtGui/QQuaternion>
#include <ossia/detail/optional.hpp>
#include <ossia-qt/qml_context.hpp>

namespace ossia
{
namespace qt
{

template<std::size_t N>
struct QArray {};

template<> struct QArray<2> { using type = QVector2D; };
template<> struct QArray<3> { using type = QVector3D; };
template<> struct QArray<4> { using type = QVector4D; };

/**
 * @brief The matching_ossia_enum struct
 *
 * Allows converting between QML enums and Ossia enums
 */
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


/**
 * @brief The js_value_inbound_visitor struct
 *
 * Converts a QJSValue to an ossia::value according to the current value's type
 */
struct js_value_inbound_visitor
{
  const QJSValue& val;
public:
  ossia::value operator()(impulse) const;

  ossia::value operator()(int32_t v) const;
  ossia::value operator()(float v) const;
  ossia::value operator()(bool v) const;
  ossia::value operator()(char v) const;

  ossia::value operator()(const std::string& v) const;
  ossia::value operator()(const std::vector<ossia::value>& v) const;

  ossia::value operator()(vec2f v) const;
  ossia::value operator()(vec3f v) const;
  ossia::value operator()(vec4f v) const;

  ossia::value operator()() const;
};

struct qt_to_ossia
{
  ossia::value operator()() { return ossia::impulse{}; }
  ossia::value operator()(bool v) { return v; }
  ossia::value operator()(QTime v) { return v.msec(); }
  ossia::value operator()(qint32 v) { return v; }
  ossia::value operator()(quint32 v) { return (int)v; }
  ossia::value operator()(qint64 v) { return (int)v; }
  ossia::value operator()(quint64 v) { return (int)v; }
  ossia::value operator()(char v) { return v; }
  ossia::value operator()(QChar v) { return v.toLatin1(); }
  ossia::value operator()(const QString& v) { return v.toStdString(); }
  ossia::value operator()(const QByteArray& v) { return v.toStdString(); }
  ossia::value operator()(double v) { return v; }
  ossia::value operator()(QColor v) { return make_vec(v.alphaF(), v.redF(), v.greenF(), v.blueF()); }
  ossia::value operator()(QPoint v) { return make_vec(v.x(), v.y()); }
  ossia::value operator()(QPointF v) { return make_vec(v.x(), v.y()); }
  ossia::value operator()(QSize v) { return make_vec(v.width(), v.height()); }
  ossia::value operator()(QSizeF v) { return make_vec(v.width(), v.height()); }
  ossia::value operator()(QRect v) { return make_vec(v.x(), v.y(), v.width(), v.height()); }
  ossia::value operator()(QRectF v) { return make_vec(v.x(), v.y(), v.width(), v.height()); }
  ossia::value operator()(QLine v) { return make_vec(v.p1().x(), v.p1().y(), v.p2().x(), v.p2().y()); }
  ossia::value operator()(QLineF v) { return make_vec(v.p1().x(), v.p1().y(), v.p2().x(), v.p2().y()); }
  ossia::value operator()(QVector2D v) { return make_vec(v.x(), v.y()); }
  ossia::value operator()(QVector3D v) { return make_vec(v.x(), v.y(), v.z()); }
  ossia::value operator()(QVector4D v) { return make_vec(v.x(), v.y(), v.z(), v.w()); }
  ossia::value operator()(QQuaternion v) { return make_vec(v.scalar(), v.x(), v.y(), v.z()); }
  auto operator()(const QVariantList& v)
  {
    std::vector<ossia::value> tpl;
    tpl.reserve(v.size());
    for(auto& val : v)
    {
      tpl.push_back(qt_to_ossia{}(val));
    }
    return tpl;
  }
  ossia::value operator()(const QStringList& v)
  {
    std::vector<ossia::value> tpl;
    tpl.reserve(v.size());
    for(auto& val : v)
    {
      tpl.push_back(val.toStdString());
    }
    return tpl;
  }
  ossia::value operator()(const QDate& v) { return v.toString().toStdString(); }

  ossia::value operator()(const QVariant& v);
};

struct ossia_to_qvariant
{
  QVariant operator()(QVariant::Type type, const ossia::value& ossia_val);


  QVariant operator()(impulse) const { return {}; }

  QVariant operator()(int32_t val) const { return val; }

  QVariant operator()(float val) const { return val; }
  QVariant operator()(bool val) const { return val; }
  QVariant operator()(char val) const { return val; }

  QVariant operator()(const std::string& val) const { return QString::fromStdString(val); }



  template<std::size_t N>
  typename QArray<N>::type make_array(const std::array<float, N>& arr) const
  {
    typename QArray<N>::type vec;

    for(std::size_t i = 0U; i < N; i++)
      vec[i] = arr[i];
    return vec;
  }

  QVariant operator()(vec2f val) const { return make_array(val); }
  QVariant operator()(vec3f val) const { return make_array(val); }
  QVariant operator()(vec4f val) const { return make_array(val); }

  QVariant operator()() const { return {}; }

  QVariant operator()(const std::vector<ossia::value>& val) const
  {
    QVariantList v;
    v.reserve(val.size());
    for(const ossia::value& e : val)
    {
      v.push_back(e.apply(*this));
    }
    return v;
  }

};


/**
 * @brief The js_value_outbound_visitor struct
 *
 * Creates a QJSValue from an ossia::value
 */
struct js_value_outbound_visitor
{
  QJSEngine& engine;

  QJSValue to_enum(qml_context::val_type t) const;

  QJSValue operator()(impulse) const;

  QJSValue operator()(int32_t val) const;
  QJSValue operator()(float val) const;
  QJSValue operator()(bool val) const;
  QJSValue operator()(char val) const;

  QJSValue operator()(const std::string& val) const;

  QJSValue make_tuple(const std::vector<ossia::value>& arr) const;

  QJSValue operator()(const std::vector<ossia::value>& val) const;

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

  QJSValue operator()(vec2f val) const;
  QJSValue operator()(vec3f val) const;
  QJSValue operator()(vec4f val) const;

  QJSValue operator()() const;
};


/**
 * @brief The js_value_outbound_visitor struct
 *
 * Creates a QString from an ossia::value.
 * This is used for replacing value parameters by the actual value
 * in JSON messages for instance.
 */
struct js_string_outbound_visitor
{
  QString operator()(impulse) const;

  QString operator()(int32_t val) const;

  QString operator()(float val) const;
  QString operator()(bool val) const;
  QString operator()(char val) const;

  QString operator()(const std::string& val) const;

  QString operator()(const std::vector<ossia::value>& val) const;

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

  QString operator()(vec2f val) const;
  QString operator()(vec3f val) const;
  QString operator()(vec4f val) const;

  QString operator()() const;
};

ossia::value value_from_jsvalue(const QJSValue& v);

inline ossia::value value_from_jsvalue(ossia::value cur, const QJSValue& v)
{
  return cur.apply(js_value_inbound_visitor{v});
}

inline QJSValue value_to_js_value(const ossia::value& cur, QJSEngine& engine)
{
  return cur.apply(js_value_outbound_visitor{engine});
}

inline QString value_to_js_string(const ossia::value& cur)
{
  return cur.apply(js_string_outbound_visitor{});
}

/**
 * @brief get_enum Gets an ossia enum from a QJSValue
 *
 * This function tries to convert a weakly-typed JS enum to a strongly-typed C++ enum.
 */
template<typename T>
ossia::optional<T> get_enum(const QJSValue& val)
{
  if(val.isNumber())
  {
    const int n = val.toInt();
    if(n >= 0 && n < QMetaEnum::fromType<typename matching_ossia_enum<T>::type>().keyCount())
    {
      return static_cast<T>(n);
    }
  }
  return {};
}

/**
 * @brief make_address_data Extracts all the data required to build an address from a QJSValue
 */
ossia::net::address_data make_address_data(const QJSValue& js);


void set_address_type(QVariant::Type type, ossia::net::address_base& addr);


/**
 * @defgroup JSTreeCreation
 *
 * The following functions iterate over a tree given in Javscript
 * in order to create corresponding Ossia structures
 */

template<typename Device_T, typename Node_T, typename Protocol_T>
/** @ingroup JSTreeCreation **/
void create_device(Device_T& device, QJSValue root);

template<typename Device_T, typename Node_T, typename Protocol_T>
/** @ingroup JSTreeCreation **/
void create_node_rec(QJSValue js, Device_T& device, Node_T& parent);

template<typename Device_T, typename Node_T, typename Protocol_T>
/** @ingroup JSTreeCreation **/
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
/** @ingroup JSTreeCreation **/
void create_node_rec(QJSValue js, Device_T& device, Node_T& parent)
{
  auto data = Protocol_T::read_data(js);
  if(data.name.empty())
    return;

  auto node = new Node_T{data, device, parent};
  parent.add_child(std::unique_ptr<ossia::net::node_base>(node));

  device.on_node_created(*node);

  QJSValue children = js.property("children");
  if(!children.isArray())
    return;

  QJSValueIterator it(children);
  while (it.hasNext()) {
    it.next();
    create_node_rec<Device_T, Node_T, Protocol_T>(it.value(), device, *node);
  }
}

template<typename Methods>
QMetaObject::Connection connectSignalToMatchingMethod(
    const QMetaMethod& sig,
    Methods& meth,
    QObject* source,
    QObject* target)
{
  switch(sig.parameterCount())
  {
    case 0:
    {
      return QObject::connect(source, sig,
                       target, meth[QVariant::Invalid]);
    }
    case 1:
    {
      auto t = sig.parameterType(0);

      auto method_it = meth.find((QVariant::Type)t);
      if(method_it != meth.end())
      {
        return QObject::connect(source, sig,
                         target, method_it->second);
      }
      break;
    }
  }
  return {};
}

template<typename Methods>
QMetaObject::Connection connectSignalToMatchingMethod(
    const QQmlProperty& prop,
    Methods& methods,
    QMetaMethod variantMethod,
    QObject* source,
    QObject* target)
{
  auto meth = prop.method();

  switch(meth.parameterCount())
  {
    case 0:
    {
      return QObject::connect(
            source, prop.method(),
            target, variantMethod);
    }
    case 1:
    {
      auto t = meth.parameterType(0);

      auto method_it = methods.find((QVariant::Type)t);
      if(method_it != methods.end())
      {
        return QObject::connect(
              source, prop.method(),
              target, method_it->second);
      }
      break;
    }
  }
  return {};
}
}

namespace net {
OSSIA_EXPORT void sanitize_name(QString& str);
OSSIA_EXPORT QString sanitize_name(QString name_base, const std::vector<QString>& brethren);
}

}

Q_DECLARE_METATYPE(ossia::qt::qml_context*)
#else
#error This file requires Qt.
#endif
