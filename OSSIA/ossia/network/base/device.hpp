#pragma once
#include <ossia/network/base/node.hpp>
#include <ossia/network/base/address_data.hpp>
#include <ossia/network/base/node_attributes.hpp>
#include <nano_signal_slot.hpp>
#include <ossia_export.h>

namespace ossia
{
namespace net
{
class protocol_base;

/**
 * @brief What a device is able to do
 */
struct device_capabilities
{
        /**
         * @brief change_tree : nodes can be added and removed externally.
         */
        bool change_tree = false;
};

/**
 * @brief Root of a device tree
 *
 * A device models a real hardware or software
 * with which communication should be established.
 *
 * It handles transformations of the node tree and allows
 * to add callbacks to be called :
 * - after a node has been created : device_base::onNodeCreated
 * - after a node has been renamed : device_base::onNodeRenamed
 * - before a node is removed : device_base::onNodeRemoving
 *
 * - after an address has been created : device_base::onAddressCreated
 * - before an address is being removed : device_base::onAddressRemoving
 *
 * The root node of a device maps to the "/" address.
 *
 * A device is necessarily constructed with a protocol.
 * If the device is meant to mirror a remote application, it should
 * be constructed directly with the remote protocol
 * (e.g. \ref minuit_protocol, \ref osc_protocol).
 *
 * if the device is meant to expose the current application to the network,
 * it should be constructed with the \ref local_protocol.
 *
 * \see generic_device
 * \see node_base
 * \see protocol_base
 */
class OSSIA_EXPORT device_base
{
public:
  device_base(std::unique_ptr<ossia::net::protocol_base> proto);

  device_base() = delete;
  device_base(const device_base&) = delete;
  device_base(device_base&&) = delete;
  device_base& operator=(const device_base&) = delete;
  device_base& operator=(device_base&&) = delete;

  ossia::net::protocol_base& getProtocol() const;

  virtual ~device_base();

  virtual const ossia::net::node_base& getRootNode() const = 0;
  virtual ossia::net::node_base& getRootNode() = 0;

  device_capabilities getCapabilities() const
  { return mCapabilities; }

  void setName(const std::string& str)
  {
    getRootNode().setName(str);
  }
  std::string getName() const
  {
    return getRootNode().getName();
  }

  Nano::Signal<void(const node_base&)> onNodeCreated; // The node being created
  Nano::Signal<void(const node_base&)>
      onNodeRemoving; // The node being removed
  Nano::Signal<void(const node_base&, std::string)>
      onNodeRenamed; // Node has the new name, second argument is the old name
  Nano::Signal<void(const node_base&, ossia::string_view)>
      onAttributeModified; // Second argument is an identifier
  Nano::Signal<void(const address_base&)>
      onAddressCreated; // The address being created
  Nano::Signal<void(const address_base&)>
      onAddressRemoving; // The node whose address was removed

  //! Called when a network client requests the creation of an instance.
  //!  First argument is the path to the parent.
  Nano::Signal<void(std::string, address_data)> onAddNodeRequested;

  //! Called when a network client requests the removal of an instance.
  //! Argument is the path of the parent and the node to remove.
  Nano::Signal<void(std::string, std::string)> onRemoveNodeRequested;
protected:
  std::unique_ptr<ossia::net::protocol_base> mProtocol;
  device_capabilities mCapabilities;
};

template<typename T>
void node_base::set(ossia::string_view str, const T& value)
{
  auto opt = ossia::get_optional_attribute<T>(*this, str);
  if(opt && *opt != value)
  {
    ossia::set_attribute(*this, str, value);
    getDevice().onAttributeModified(*this, str);
  }
}

template<typename T>
void node_base::set(ossia::string_view str, T&& value)
{
  auto opt = ossia::get_optional_attribute<T>(*this, str);
  if(opt && *opt != value)
  {
    ossia::set_attribute(*this, str, std::move(value));
    getDevice().onAttributeModified(*this, str);
  }
}

template<typename Attribute, typename T>
void node_base::set(Attribute a, const T& value)
{
  auto opt = a.getter(*this);

  // We make a copy here to prevent a double conversion
  // for instance from std::vector<> to value. TODO do the same in the other.
  typename Attribute::type val = value;
  if(compare_optional(opt, val))
  {
    a.setter(*this, val);
    getDevice().onAttributeModified(*this, a.text());
  }
}

template<typename Attribute, typename T>
void node_base::set(Attribute a, T& value)
{
  set(a, const_cast<const T&>(value));
}

template<typename Attribute, typename T>
void node_base::set(Attribute a, T&& value)
{
  auto opt = a.getter(*this);
  if(compare_optional(opt, value))
  {
    a.setter(*this, std::move(value));
    getDevice().onAttributeModified(*this, a.text());
  }
}
}
}
