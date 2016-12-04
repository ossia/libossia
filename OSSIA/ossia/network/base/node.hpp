#pragma once
#include <ossia/detail/callback_container.hpp>
#include <ossia/detail/ptr_container.hpp>
#include <ossia/network/common/address_properties.hpp>
#include <ossia/network/base/name_validation.hpp>
#include <functional>
#include <memory>
#include <string>

#include <nano_signal_slot.hpp>
#include <ossia_export.h>

namespace ossia
{
namespace net
{
class device_base;
class address_base;
class node_base;

/**
 * @brief The node_base class
 *
 * Base class for nodes.
 * A \ref node_base is part of the hierarchy of a \ref device_base.
 *
 * The \ref node_base has ownership of its children.
 *
 * If a node is meant to send and receive data, an \ref address_base
 * should be created with node_base::createAddress.
 *
 * Some device trees may provide immutable node hierarchies :
 * child nodes cannot be added nor removed.
 * This is the case for the \ref midi_device.
 * Other device trees allow the user to create nodes as he sees fit :
 * \ref generic_node is used for this common case.
 *
 * \see generic_node
 *
 * \see device_base
 * \see address_base
 */
class OSSIA_EXPORT node_base
{
public:
  node_base() = default;
  node_base(const node_base&) = delete;
  node_base(node_base&&) = delete;
  node_base& operator=(const node_base&) = delete;
  node_base& operator=(node_base&&) = delete;

  virtual ~node_base();

  virtual device_base& getDevice() const = 0;
  virtual node_base* getParent() const = 0;

  virtual std::string getName() const = 0;
  virtual node_base& setName(std::string) = 0;

  virtual address_base* getAddress() const = 0;
  virtual address_base* createAddress(val_type = val_type::IMPULSE) = 0;
  virtual bool removeAddress() = 0;

  // The parent has ownership
  node_base* createChild(const std::string& name);
  node_base* findChild(const std::string& name);
  bool removeChild(const std::string& name);
  bool removeChild(const node_base& name);
  void clearChildren();

  const std::vector<std::unique_ptr<node_base>>& children() const
  {
    return mChildren;
  }


  mutable Nano::Signal<void(const node_base&)> aboutToBeDeleted;

protected:
  /** Should return nullptr if no child is to be added */
  virtual std::unique_ptr<node_base> makeChild(const std::string& name) = 0;

  /** Should return false if the child must not be removed */
  virtual void removingChild(node_base& node_base) = 0;

  std::vector<std::unique_ptr<node_base>> mChildren;
};

}
}
