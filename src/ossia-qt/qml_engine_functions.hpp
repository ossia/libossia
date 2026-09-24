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

#include <functional>
#include <mutex>
#include <span>

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
      const qml_device_cache& state, qml_device_push_function push, QQmlEngine& engine, QObject* parent)
      : QObject{parent}
      , on_push{std::move(push)}
      , m_engine{engine}
      , m_devices{state}
  {
  }

  ~qml_engine_functions() override;

  QVariant read(const QString& address);
  W_SLOT(read);
  void write(const QString& address, const QVariant& value);
  W_SLOT(write);
  void exec(const QString& code) E_SIGNAL(OSSIA_EXPORT, exec, code);
  void compute(const QString& code, const QString& cb)
      E_SIGNAL(OSSIA_EXPORT, compute, code, cb);
  void system(const QString& code) E_SIGNAL(OSSIA_EXPORT, system, code);

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

  QJSValue toValue(QJSValue) const noexcept;
  W_SLOT(toValue)

  static ossia::net::node_base*
  find_node(qml_device_cache& devices, std::string_view name);

  //! Resolves an address to the node it names, if any. Serialized like read()
  //! and write(): the returned node cannot be a device that just left the list.
  ossia::net::node_base* find(const QString& address);

  //! The device this script belongs to, if any. An unqualified address
  //! ('/foo/bar') names a node of *that* device and of no other: several
  //! devices of a document routinely expose the same leaf names, so searching
  //! the whole list would make '/leaf' land in whichever comes first. A
  //! qualified address ('othername:/foo/bar') still reaches any device of the
  //! list. Scripts owning no device leave this unset; for them an unqualified
  //! address means "the first match in any device of the document".
  void setDevice(ossia::net::device_base*);

  //! Device bookkeeping. Every mutation drops the resolved-address cache: that
  //! cache holds raw ossia::net::parameter_base pointers, and a device leaving
  //! the list takes its parameters with it. All of it is serialized against
  //! read() and write() so that a device cannot be removed while a script is
  //! resolving an address or pushing a value into it.
  void setDevices(qml_device_cache devices);
  void addDevice(ossia::net::device_base* device);
  void removeDevice(ossia::net::device_base* device);

  //! Definitive: read() and write() become no-ops, and the device list and the
  //! address cache are dropped. Used when the tree the scripts write into is
  //! about to be destroyed while queued scripts may still run.
  void disable();

private:
  using device_span = std::span<ossia::net::device_base* const>;

  //! A resolved address plus the devices it is allowed to expand into: a
  //! pattern address stays a path and is matched against those roots only.
  struct resolved_address
  {
    const ossia::destination_t& destination;
    device_span scope;
  };

  //! Requires m_mutex: the returned reference lives in m_address_cache, which
  //! any device-list mutation clears, and the span borrows either m_devices or
  //! m_own_device.
  resolved_address find_address(const QString&);

  qml_device_push_function on_push;
  QQmlEngine& m_engine;

  ossia::hash_map<QString, ossia::destination_t> m_address_cache;
  ossia::value_port m_port_cache;

protected:
  //! Requires m_mutex. For a change of the tree that the device list does not
  //! see: a removed node's parameter may be in the cache.
  void clear_address_cache() { m_address_cache.clear(); }

  //! Guards m_devices, m_own_device, m_address_cache and m_enabled. Recursive:
  //! a script write re-enters write() through the push callbacks.
  std::recursive_mutex m_mutex;
  qml_device_cache m_devices;
  ossia::net::device_base* m_own_device{};
  bool m_enabled{true};
};

class OSSIA_EXPORT qml_device_engine_functions : public qml_engine_functions
{
  W_OBJECT(qml_device_engine_functions)

public:
  using qml_engine_functions::qml_engine_functions;
  ~qml_device_engine_functions();

  //! addNode()/removeNode() edit the tree of the device set through
  //! setDevice(): a script only ever grows its own device.
  void addNode(QString address, QString type);
  W_SLOT(addNode)

  void removeNode(QString address, QString type);
  W_SLOT(removeNode)

  //! When set, addNode()/removeNode() hand their edit to this instead of
  //! performing it on the script's thread, e.g. to post it to the thread that
  //! owns the tree. The edit re-checks that the device is still enabled when
  //! it eventually runs. Set before the script runs.
  using tree_editor = std::function<void(std::function<void()>)>;
  void setTreeEditor(tree_editor);

private:
  void editTree(std::function<void(ossia::net::node_base& root)> edit);

  tree_editor m_tree_editor;
};
}
