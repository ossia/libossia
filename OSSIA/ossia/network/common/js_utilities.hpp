#pragma once
#if defined(QT_CORE_LIB)
#include <ossia/editor/value/value.hpp>
#include <ossia/network/common/address_properties.hpp>
#include <ossia/network/domain/domain.hpp>
#include <ossia/network/base/node.hpp>
#include <ossia/network/generic/generic_address.hpp>
#include <ossia/editor/dataspace/dataspace_visitors.hpp>
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

/**
 * @brief The qml_context struct
 *
 * This class is used to allow QML scripts to access common enums
 * in Ossia.
 */
struct qml_context
{
private:
  Q_GADGET

public:
  // QML enums have to begin with a capital
  enum class val_type
  {
    Impulse, //! \see ossia::Impulse
    Bool, //! \see bool
    Int, //! \see int32_t
    Float, //! \see float
    Char, //! \see char
    String, //! \see std::string
    Tuple, //! \see std::vector<ossia::value>
    Vec2f, //! \see ossia::Vec2f
    Vec3f, //! \see ossia::Vec3f
    Vec4f, //! \see ossia::Vec4f
    Destination //! \see ossia::Destination
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
  ossia::value operator()(Impulse) const;

  ossia::value operator()(int32_t v) const;
  ossia::value operator()(float v) const;
  ossia::value operator()(bool v) const;
  ossia::value operator()(char v) const;

  ossia::value operator()(const std::string& v) const;
  ossia::value operator()(const std::vector<ossia::value>& v) const;

  ossia::value operator()(Vec2f v) const;
  ossia::value operator()(Vec3f v) const;
  ossia::value operator()(Vec4f v) const;

  ossia::value operator()(const Destination& t);
  ossia::value operator()() const;
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

  QJSValue operator()(Impulse) const;

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

  QJSValue operator()(Vec2f val) const;
  QJSValue operator()(Vec3f val) const;
  QJSValue operator()(Vec4f val) const;

  QJSValue operator()(const Destination& t);
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
  QString operator()(Impulse) const;

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

  QString operator()(Vec2f val) const;
  QString operator()(Vec3f val) const;
  QString operator()(Vec4f val) const;

  QString operator()(const Destination& t);
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
boost::optional<T> get_enum(const QJSValue& val)
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
generic_address_data make_address_data(const QJSValue& js);


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

}
}

Q_DECLARE_METATYPE(ossia::net::qml_context)
#else
#error This file requires Qt.
#endif
