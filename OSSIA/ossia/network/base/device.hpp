#pragma once
#include <ossia/network/base/node.hpp>
#include <nano_signal_slot.hpp>
#include <ossia_export.h>

namespace ossia
{
namespace net
{
class protocol_base;

/**
 * @brief The device_base class
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

struct device_capabilities
{
        /**
         * @brief change_tree : nodes can be added and removed externally.
         */
        bool change_tree = false;
};

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
  Nano::Signal<void(const address_base&)>
      onAddressCreated; // The address being created
  Nano::Signal<void(const address_base&)>
      onAddressModified; // The address being modified
  Nano::Signal<void(const address_base&)>
      onAddressRemoving; // The node whose address was removed

protected:
  std::unique_ptr<ossia::net::protocol_base> mProtocol;
  device_capabilities mCapabilities;
};
}
}
