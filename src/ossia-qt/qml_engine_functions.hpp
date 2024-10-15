#pragma once

#include <ossia/dataflow/dataflow_fwd.hpp>
#include <ossia/dataflow/value_port.hpp>
#include <ossia/detail/hash_map.hpp>
#include <ossia/network/base/node_functions.hpp>
#include <ossia/network/common/path.hpp>

#include <ossia-qt/js_utilities.hpp>

#include <QObject>
#include <QQmlContext>
#include <QQmlEngine>

#include <verdigris>

namespace ossia
{
struct value_port;
namespace net
{
class device_base;
}
}
namespace ossia::qt
{
using qml_device_cache = ossia::small_vector<ossia::net::device_base*, 4>;
using qml_device_push_function
    = smallfun::function<void(ossia::net::parameter_base&, const ossia::value_port&)>;

class OSSIA_EXPORT qml_engine_functions : public QObject
{
  W_OBJECT(qml_engine_functions)
public:
  qml_engine_functions(
      const qml_device_cache& state, qml_device_push_function push, QObject* parent)
      : QObject{parent}
      , devices{state}
      , on_push{std::move(push)}
  {
  }

  ~qml_engine_functions() override;

  QVariant read(const QString& address);
  W_SLOT(read);
  void write(const QString& address, const QVariant& value);
  W_SLOT(write);
  void exec(const QString& code) W_SIGNAL(exec, code);
  void compute(const QString& code, const QString& cb) W_SIGNAL(compute, code, cb);
  void system(const QString& code) W_SIGNAL(system, code);

  /// Conversions ///
  QVariant asArray(QVariant) const noexcept;
  W_SLOT(asArray)
  QVariant asColor(QVariant) const noexcept;
  W_SLOT(asColor)
  QVariant asVec2(QVariant) const noexcept;
  W_SLOT(asVec2)
  QVariant asVec3(QVariant) const noexcept;
  W_SLOT(asVec3)
  QVariant asVec4(QVariant) const noexcept;
  W_SLOT(asVec4)

  static ossia::net::node_base*
  find_node(qml_device_cache& devices, std::string_view name);
  const ossia::destination_t& find_address(const QString&);

  qml_device_cache devices;

private:
  qml_device_push_function on_push;

  ossia::hash_map<QString, ossia::destination_t> m_address_cache;
  ossia::value_port m_port_cache;
  // TODO share cache
};

class OSSIA_EXPORT qml_device_engine_functions : public qml_engine_functions
{
  W_OBJECT(qml_device_engine_functions)
  ossia::net::device_base* m_dev{};

public:
  using qml_engine_functions::qml_engine_functions;
  ~qml_device_engine_functions();

  void addNode(QString address, QString type);
  W_SLOT(addNode)

  void removeNode(QString address, QString type);
  W_SLOT(removeNode)

  void setDevice(ossia::net::device_base*);
};
}
