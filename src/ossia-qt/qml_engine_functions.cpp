#include "qml_engine_functions.hpp"

#include <ossia/dataflow/dataflow.hpp>
#include <ossia/detail/algorithms.hpp>
#include <ossia/detail/apply.hpp>
#include <ossia/network/common/debug.hpp>
#include <ossia/network/value/value.hpp>

#include <ossia-qt/js_utilities.hpp>

#include <wobjectimpl.h>
W_OBJECT_IMPL(ossia::qt::qml_engine_functions)
W_OBJECT_IMPL(ossia::qt::qml_device_engine_functions)
namespace ossia::qt
{

qml_engine_functions::~qml_engine_functions() { }

qml_engine_functions::resolved_address
qml_engine_functions::find_address(const QString& str)
{
  // OPTIMIZEME this function can be optimized a lot
  // c.f. MapperDevice.cpp:find_parameter
  const auto d = str.indexOf(':');

  // An address without a ':' cannot name a device, so it names a node of the
  // device this script belongs to - and only of that one. Scripts with no own
  // device search the whole document instead: nothing else is available to
  // them. This is also the scope a pattern address gets expanded against.
  const bool own_scope = m_own_device && d == -1;
  const device_span scope = own_scope
                                ? device_span{&m_own_device, 1}
                                : device_span{m_devices.data(), m_devices.size()};

  if(auto it = m_address_cache.find(str); it != m_address_cache.end())
  {
    return {it->second, scope};
  }

  if(d == -1)
  {
    // Address looks like '/foo/bar'
    auto node = own_scope ? ossia::net::find_node(
                                m_own_device->get_root_node(), str.toStdString())
                          : find_node(m_devices, str.toStdString());
    if(node)
    {
      if(auto addr = node->get_parameter())
      {
        auto [it, b] = m_address_cache.insert({str, addr});
        return {it->second, scope};
      }
    }
  }
  else
  {
    // Address looks like 'devname:/foo/bar': any device of the list may be
    // named, including this script's own one.
    auto dev = ossia::find_if(
        m_devices, [devname = str.mid(0, d).toStdString()](const auto& dev) {
      return dev->get_name() == devname;
    });

    if(dev != m_devices.end())
    {
      if(d == str.size() - 1)
      {
        if(auto addr = (*dev)->get_root_node().get_parameter())
        {
          auto [it, b] = m_address_cache.insert({str, addr});
          return {it->second, scope};
        }
      }

      auto node
          = ossia::net::find_node((*dev)->get_root_node(), str.mid(d + 1).toStdString());
      if(node)
      {
        if(auto addr = node->get_parameter())
        {
          auto [it, b] = m_address_cache.insert({str, addr});
          return {it->second, scope};
        }
      }
    }
  }

  if(auto p = ossia::traversal::make_path(str.toStdString()))
  {
    auto [it, b] = m_address_cache.insert({str, *p});
    return {it->second, scope};
  }

  static const ossia::destination_t bad_dest;
  return {bad_dest, scope};
}

void qml_engine_functions::setDevices(qml_device_cache devices)
{
  std::lock_guard g{m_mutex};
  if(!m_enabled)
    return;
  m_devices = std::move(devices);
  m_address_cache.clear();
}

void qml_engine_functions::addDevice(ossia::net::device_base* device)
{
  std::lock_guard g{m_mutex};
  if(!m_enabled || !device)
    return;
  m_devices.push_back(device);
  ossia::remove_duplicates(m_devices);
  m_address_cache.clear();
}

void qml_engine_functions::removeDevice(ossia::net::device_base* device)
{
  std::lock_guard g{m_mutex};
  ossia::remove_erase(m_devices, device);
  if(m_own_device == device)
    m_own_device = nullptr;
  // The cache stores raw parameter pointers: every one of them may have
  // belonged to that device.
  m_address_cache.clear();
}

void qml_engine_functions::setDevice(ossia::net::device_base* device)
{
  {
    std::lock_guard g{m_mutex};
    if(!m_enabled)
      return;
    m_own_device = device;
    m_address_cache.clear();
  }
  // A script also reaches its own device by name: it has to be in the list.
  addDevice(device);
}

void qml_engine_functions::disable()
{
  std::lock_guard g{m_mutex};
  m_enabled = false;
  m_devices.clear();
  m_own_device = nullptr;
  m_address_cache.clear();
}

ossia::net::node_base* qml_engine_functions::find(const QString& address)
{
  std::lock_guard g{m_mutex};
  if(!m_enabled)
    return nullptr;

  const auto& res = find_address(address).destination;
  if(auto p = res.target<ossia::net::parameter_base*>())
    return *p ? &(*p)->get_node() : nullptr;
  else if(auto n = res.target<ossia::net::node_base*>())
    return *n;
  return nullptr;
}

QVariant qml_engine_functions::read(const QString& address)
{
  std::lock_guard g{m_mutex};
  if(!m_enabled)
    return {};

  const auto res = find_address(address);
  if(res.destination)
  {
    QVariant var;
    QVariantMap mv;

    bool unique = ossia::apply_to_destination(
        res.destination, res.scope, [&](ossia::net::parameter_base* addr, bool unique) {
      if(unique)
      {
        var = addr->value().apply(ossia::qt::ossia_to_qvariant{});
      }
      else
      {
        mv[QString::fromStdString(addr->get_node().osc_address())]
            = addr->value().apply(ossia::qt::ossia_to_qvariant{});
      }
    }, ossia::do_nothing_for_nodes{});
    if(unique)
    {
      return var;
    }
    else
    {
      return mv;
    }
  }
  return {};
}

void qml_engine_functions::write(const QString& address, const QVariant& value)
{
  std::lock_guard g{m_mutex};
  if(!m_enabled)
    return;

  const auto res = find_address(address);
  if(res.destination)
  {
    auto& cache = m_port_cache.get_data();
    cache.clear();
    auto converter = ossia::qt::qt_to_ossia{};
    cache.emplace_back(converter(value));

    ossia::apply_to_destination(
        res.destination, res.scope, [&](ossia::net::parameter_base* addr, bool unique) {
      if(addr)
        on_push(*addr, m_port_cache);
    }, ossia::do_nothing_for_nodes{});
  }
}

ossia::net::node_base*
qml_engine_functions::find_node(qml_device_cache& devices, std::string_view name)
{
  for(auto dev : devices)
  {
    if(auto res = ossia::net::find_node(dev->get_root_node(), name))
      return res;
  }
  return nullptr;
}

QVariant qml_engine_functions::asColor(QVariant v) const noexcept
{
  switch(v.userType())
  {
    case QMetaType::QVector3D: {
      auto val = v.value<QVector3D>();
      return QVariant::fromValue(QColor::fromRgbF(val.x(), val.y(), val.z()));
    }
    case QMetaType::QVector4D: {
      auto val = v.value<QVector4D>();
      return QVariant::fromValue(QColor::fromRgbF(val.x(), val.y(), val.z(), val.w()));
    }
    default:
      return v;
  }
}

QVariant qml_engine_functions::asVec2(QVariant v) const noexcept
{
  switch(v.userType())
  {
    default:
      return v;
  }
}

QVariant qml_engine_functions::asVec3(QVariant v) const noexcept
{
  switch(v.userType())
  {
    default:
      return v;
  }
}

QVariant qml_engine_functions::asVec4(QVariant v) const noexcept
{
  switch(v.userType())
  {
    default:
      return v;
  }
}

QJSValue qml_engine_functions::toValue(QJSValue v) const noexcept
{
  if (!v.isObject())
    return v;

  if (v.hasProperty("type") && v.hasProperty("value"))
  {
    auto type = (ossia::val_type)v.property("type").toInt();
    QJSValue value = v.property("value");

    switch (type)
    {
      case ossia::val_type::FLOAT:
      case ossia::val_type::INT:
      case ossia::val_type::VEC2F:
      case ossia::val_type::VEC3F:
      case ossia::val_type::VEC4F:
      case ossia::val_type::BOOL:
      case ossia::val_type::STRING:
      {
        return value;
      }
      case ossia::val_type::IMPULSE:
      {
        return QJSValue::UndefinedValue;
      }
      case ossia::val_type::LIST:
      {
        int length = value.property("length").toInt();
        QJSValue unpackedList = m_engine.newArray(length); // Assuming m_engine is available

        for (int i = 0; i < length; ++i)
        {
          unpackedList.setProperty(i, toValue(value.property(i)));
        }
        return unpackedList;
        break;
      }
      case ossia::val_type::MAP:
      {
        QJSValue unpackedMap = m_engine.newObject();
        QJSValueIterator it(value);
        while (it.next())
        {
          unpackedMap.setProperty(it.name(), toValue(it.value()));
        }
        return unpackedMap;
        break;
      }
      default:
        break;
    }
  }

  return v;
}

QVariant qml_engine_functions::asArray(QVariant v) const noexcept
{
  switch(v.userType())
  {
    case QMetaType::Bool:
    case QMetaType::QTime:
    case QMetaType::Int:
    case QMetaType::UInt:
    case QMetaType::ULongLong:
    case QMetaType::Char:
    case QMetaType::QString:
    case QMetaType::QByteArray:
    case QMetaType::Double:
    case QMetaType::QColor: {
      auto val = v.value<QColor>();
      return QVariant::fromValue(
          QList<double>{val.redF(), val.greenF(), val.blueF(), val.alphaF()});
    }
    case QMetaType::QPoint: {
      auto val = v.value<QPoint>();
      return QVariant::fromValue(QList<int>{val.x(), val.y()});
    }
    case QMetaType::QPointF: {
      auto val = v.value<QPointF>();
      return QVariant::fromValue(QList<double>{val.x(), val.y()});
    }
    case QMetaType::QVector2D: {
      auto val = v.value<QVector2D>();
      return QVariant::fromValue(QList<double>{val.x(), val.y()});
    }
    case QMetaType::QSize: {
      auto val = v.value<QSize>();
      return QVariant::fromValue(QList<int>{val.width(), val.height()});
    }
    case QMetaType::QSizeF: {
      auto val = v.value<QSizeF>();
      return QVariant::fromValue(QList<double>{val.width(), val.height()});
    }
    case QMetaType::QVector3D: {
      auto val = v.value<QVector3D>();
      return QVariant::fromValue(QList<double>{val.x(), val.y(), val.z()});
    }
    case QMetaType::QVector4D: {
      auto val = v.value<QVector4D>();
      return QVariant::fromValue(QList<double>{val.x(), val.y(), val.z(), val.w()});
    }
    case QMetaType::QQuaternion: {
      auto val = v.value<QQuaternion>();
      return QVariant::fromValue(QList<double>{val.x(), val.y(), val.z(), val.scalar()});
    }
    case QMetaType::QLine: {
      auto val = v.value<QLine>();
      return QVariant::fromValue(
          QList<int>{val.p1().x(), val.p1().y(), val.p2().x(), val.p2().y()});
    }
    case QMetaType::QLineF: {
      auto val = v.value<QLineF>();
      return QVariant::fromValue(
          QList<double>{val.p1().x(), val.p1().y(), val.p2().x(), val.p2().y()});
    }
    case QMetaType::QRect: {
      auto val = v.value<QRect>();
      return QVariant::fromValue(
          QList<int>{val.x(), val.y(), val.width(), val.height()});
    }
    case QMetaType::QRectF: {
      auto val = v.value<QRectF>();
      return QVariant::fromValue(
          QList<double>{val.x(), val.y(), val.width(), val.height()});
    }
    case QMetaType::QVariantList:
    case QMetaType::QStringList:
    case QMetaType::QDate:
    default:
      break;
  }

  return v;
}

qml_device_engine_functions::~qml_device_engine_functions() = default;
void qml_device_engine_functions::addNode(QString address, QString type)
{
  std::lock_guard g{m_mutex};
  if(!m_enabled || !m_own_device)
    return;

  ossia::net::find_or_create_parameter(
      m_own_device->get_root_node(), address.toStdString(), type.toStdString());
}

void qml_device_engine_functions::removeNode(QString address, QString type)
{
  std::lock_guard g{m_mutex};
  if(!m_enabled || !m_own_device)
    return;

  if(auto res
     = ossia::net::find_node(m_own_device->get_root_node(), address.toStdString()))
    if(auto p = res->get_parent())
      p->remove_child(*res);
}
}
