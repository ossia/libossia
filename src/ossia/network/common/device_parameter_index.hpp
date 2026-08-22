#pragma once
#include <ossia/detail/hash.hpp>
#include <ossia/network/base/device.hpp>
#include <ossia/network/base/node.hpp>
#include <ossia/network/base/osc_address.hpp>
#include <ossia/network/base/parameter.hpp>

#include <boost/unordered/concurrent_flat_map.hpp>

#include <string>
#include <string_view>

namespace ossia::net
{

/**
 * @brief A thread-safe index of a device's parameters, by OSC address.
 *
 * Protocols that receive on a thread of their own - the serial and CAN ones run
 * their QML engine in a dedicated thread - have to turn an incoming address
 * into a parameter. ossia::net::find_node cannot do that from another thread:
 * it locks each node only while reading that node's children, so nothing keeps
 * the node it returns alive afterwards.
 *
 * The lifetime guarantee here is that apply() holds a lock on the entry for the
 * duration of the callback, and that a removal has to take that same lock; the
 * device signals a node's removal before destroying it, so a parameter cannot
 * be destroyed under a running callback.
 *
 * Locking is per bucket group, not per map, so a tree edit on the UI thread
 * only ever waits on a receive thread that is inside the same group.
 *
 * The callback must not itself add or remove nodes on the device: it runs with
 * the entry locked, and re-entering the index from it deadlocks.
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

    index_recursively(dev.get_root_node());
  }

  device_parameter_index(const device_parameter_index&) = delete;
  device_parameter_index(device_parameter_index&&) = delete;
  device_parameter_index& operator=(const device_parameter_index&) = delete;
  device_parameter_index& operator=(device_parameter_index&&) = delete;

  //! Run f on the parameter at `address`, if there is one. Returns whether
  //! there was.
  template <typename F>
  bool apply(std::string_view address, F&& f)
  {
    std::string buf;
    return m_index.visit(
               normalize(address, buf), [&](const auto& e) { f(*e.second); })
           > 0;
  }

  /**
   * @brief Drop every entry.
   *
   * The owning protocol must call this before the device tears its tree down: a
   * device destroying itself frees its children without signalling their
   * removal.
   */
  void clear() { m_index.clear(); }

  std::size_t size() const { return m_index.size(); }

private:
  /**
   * Addresses are stored the way osc_parameter_string() spells them: `/foo/bar`.
   *
   * Returns a view into `address` where it can, and only borrows `buf` when a
   * leading slash has to be added - so the common case of an already-normal
   * address costs no allocation.
   */
  static std::string_view normalize(std::string_view address, std::string& buf)
  {
    while(address.size() > 1 && address.back() == '/')
      address.remove_suffix(1);

    if(!address.empty() && address.front() == '/')
      return address;

    buf.reserve(address.size() + 1);
    buf = "/";
    buf += address;
    return buf;
  }

  /**
   * The address of a node, computed rather than read from node_base::
   * osc_address().
   *
   * That cache is filled by the node subclass, and only generic_node and
   * wrapped_node do it: midi_node and phidgets_node leave it empty, which would
   * file every parameter of such a device under "".
   */
  static std::string address_of(const ossia::net::node_base& node)
  {
    return ossia::net::osc_parameter_string(node);
  }

  void index_recursively(const ossia::net::node_base& node)
  {
    if(auto param = node.get_parameter())
      m_index.insert_or_assign(address_of(node), param);

    for(const auto& cld : node.children_copy())
      index_recursively(*cld);
  }

  void on_parameter_created(const ossia::net::parameter_base& param)
  {
    m_index.insert_or_assign(
        address_of(param.get_node()),
        const_cast<ossia::net::parameter_base*>(&param));
  }

  void on_parameter_removing(const ossia::net::parameter_base& param)
  {
    m_index.erase(address_of(param.get_node()));
  }

  void on_node_removing(ossia::net::node_base& node)
  {
    // wrapped_node has no remove_parameter(), so a node's own parameter is not
    // always signalled separately, and a node removed with children takes their
    // parameters with it: sweep the subtree.
    const auto prefix = address_of(node);

    m_index.erase_if([&prefix](const auto& e) {
      const auto& key = e.first;
      return key == prefix
             || (key.size() > prefix.size() && key.starts_with(prefix)
                 && key[prefix.size()] == '/');
    });
  }

  void on_node_renamed(ossia::net::node_base& node, const std::string& old_name)
  {
    // A rename changes the address of a whole subtree, and the node's own cache
    // has already been updated when this runs.
    m_index.clear();
    index_recursively(m_device.get_root_node());
  }

  ossia::net::device_base& m_device;
  boost::concurrent_flat_map<
      std::string, ossia::net::parameter_base*, ossia::string_hash, std::equal_to<>>
      m_index;
};

}
