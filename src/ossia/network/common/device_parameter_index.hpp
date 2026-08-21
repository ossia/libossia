#pragma once
#include <ossia/detail/hash_map.hpp>
#include <ossia/network/base/device.hpp>
#include <ossia/network/base/node.hpp>
#include <ossia/network/base/osc_address.hpp>
#include <ossia/network/base/parameter.hpp>

#include <nano_observer.hpp>

#include <mutex>
#include <string>

namespace ossia::net
{

/**
 * @brief A thread-safe index of the parameters of a device, by OSC address.
 *
 * Some protocols receive their data on a thread of their own - the serial
 * protocol runs its QML engine in a dedicated thread, for instance - and have
 * to turn an incoming address into the parameter it refers to.
 *
 * ossia::net::find_node cannot be used for that from another thread: it locks
 * each node only while it reads that node's children, so nothing keeps the node
 * it returns alive afterwards. Whichever thread owns the tree can remove and
 * destroy that node in between, and the caller is then left dereferencing freed
 * memory.
 *
 * This class keeps the addresses of the device's parameters in a map that is
 * maintained from the device's own notifications, and only ever hands a
 * parameter to its user from inside apply(), which holds the index lock for the
 * duration of the callback. Removals go through that same lock, and the device
 * signals a node's removal before destroying it, so a parameter cannot be
 * destroyed while apply() is running on it.
 *
 * The callback passed to apply() therefore runs with the lock held: it must not
 * itself add or remove nodes on the device.
 */
class device_parameter_index : public Nano::Observer
{
public:
  explicit device_parameter_index(ossia::net::device_base& dev)
      : m_device{dev}
  {
    dev.on_parameter_created.connect<&device_parameter_index::on_parameter_created>(
        *this);
    dev.on_parameter_removing.connect<&device_parameter_index::on_parameter_removing>(
        *this);
    dev.on_node_removing.connect<&device_parameter_index::on_node_removing>(*this);
    dev.on_node_renamed.connect<&device_parameter_index::on_node_renamed>(*this);

    std::lock_guard lock{m_mutex};
    index_recursively(dev.get_root_node());
  }

  device_parameter_index(const device_parameter_index&) = delete;
  device_parameter_index(device_parameter_index&&) = delete;
  device_parameter_index& operator=(const device_parameter_index&) = delete;
  device_parameter_index& operator=(device_parameter_index&&) = delete;

  //! Look up an OSC address and run f on the matching parameter, if any.
  //! Returns whether the parameter was found.
  template <typename F>
  bool apply(std::string_view address, F&& f)
  {
    std::lock_guard lock{m_mutex};
    auto it = m_index.find(normalize(address));
    if(it == m_index.end())
      return false;

    f(*it->second);
    return true;
  }

  /**
   * @brief Drop every entry.
   *
   * To be called by the owning protocol before the device tears its tree down:
   * a device destroying itself frees its children without signalling their
   * removal, so this is the only chance to stop other threads from reaching
   * them.
   */
  void clear()
  {
    std::lock_guard lock{m_mutex};
    m_index.clear();
  }

  std::size_t size() const
  {
    std::lock_guard lock{m_mutex};
    return m_index.size();
  }

private:
  //! Addresses are stored the way node_base caches them, e.g. `/foo/bar`.
  static std::string normalize(std::string_view address)
  {
    while(address.size() > 1 && address.back() == '/')
      address.remove_suffix(1);

    if(!address.empty() && address.front() == '/')
      return std::string(address);
    return "/" + std::string(address);
  }

  void index_recursively(const ossia::net::node_base& node)
  {
    if(auto param = node.get_parameter())
      m_index[node.osc_address()] = param;

    for(const auto& cld : node.children_copy())
      index_recursively(*cld);
  }

  void on_parameter_created(const ossia::net::parameter_base& param)
  {
    std::lock_guard lock{m_mutex};
    m_index[param.get_node().osc_address()]
        = const_cast<ossia::net::parameter_base*>(&param);
  }

  void on_parameter_removing(const ossia::net::parameter_base& param)
  {
    std::lock_guard lock{m_mutex};
    m_index.erase(param.get_node().osc_address());
  }

  void on_node_removing(ossia::net::node_base& node)
  {
    // The node's own parameter is not always signalled separately - wrapped_node
    // has no remove_parameter() - and a node removed while it still has children
    // takes their parameters with it, so sweep the whole subtree.
    const auto& prefix = node.osc_address();

    std::lock_guard lock{m_mutex};
    for(auto it = m_index.begin(); it != m_index.end();)
    {
      const auto& key = it->first;
      if(key == prefix
         || (key.size() > prefix.size() && key.starts_with(prefix)
             && key[prefix.size()] == '/'))
        it = m_index.erase(it);
      else
        ++it;
    }
  }

  void on_node_renamed(ossia::net::node_base& node, const std::string& old_name)
  {
    // Renaming changes the address of a whole subtree; the cached addresses of
    // the nodes have already been updated when this runs.
    std::lock_guard lock{m_mutex};
    m_index.clear();
    index_recursively(m_device.get_root_node());
  }

  ossia::net::device_base& m_device;
  mutable std::mutex m_mutex;
  ossia::hash_map<std::string, ossia::net::parameter_base*> m_index;
};

}
