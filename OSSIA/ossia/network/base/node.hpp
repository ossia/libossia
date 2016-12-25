#pragma once
#include <ossia/detail/callback_container.hpp>
#include <ossia/detail/ptr_container.hpp>
#include <ossia/network/common/address_properties.hpp>
#include <ossia/network/base/name_validation.hpp>
#include <ossia/network/base/node_attributes.hpp>
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
using any_map = tsl::hopscotch_map<std::string, boost::any>;
using extended_attributes = any_map;

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

  //! The device in which this node is.
  virtual device_base& getDevice() const = 0;

  //! Parent of this node. May be null if it is the device (i.e. root).
  virtual node_base* getParent() const = 0;

  /**
   * \brief The name of this node, e.g. "foo".
   *
   * \see ossia::net::address_string_from_node
   */
  virtual std::string getName() const = 0;
  virtual node_base& setName(std::string) = 0;

  //! Allows a node to carry a value
  virtual address_base* createAddress(val_type = val_type::IMPULSE) = 0;
  virtual bool removeAddress() = 0;
  virtual address_base* getAddress() const = 0;

  //! Allows to add arbitrary key-value metadata to nodes.
  const extended_attributes& getExtendedAttributes() const;
  void setExtendedAttributes(const extended_attributes&);

  //! Get a specific attribute.
  boost::any getAttribute(const std::string& str) const;

  template<typename T>
  void setAttribute(const std::string& str, const T& val)
  { mExtended[str] = val; }
  template<typename T>
  void setAttribute(const std::string& str, T&& val)
  { mExtended[str] = std::move(val); }

  /**
   * @brief createChild Adds a sub-child of the given name.
   *
   * @note The name of the child may be modified, so it should be checked after creation.
   *
   * @return A pointer to the child if it could be created, else nullptr.
   */
  node_base* createChild(const std::string& name);
  node_base* findChild(const std::string& name);

  bool removeChild(const std::string& name);
  bool removeChild(const node_base& name);

  void clearChildren();

  operator const extended_attributes&() const { return mExtended; }
  operator extended_attributes&() { return mExtended; }

  const std::vector<std::unique_ptr<node_base>>& children() const
  {
    return mChildren;
  }

  //! The node subclasses must call this in their destructor.
  mutable Nano::Signal<void(const node_base&)> aboutToBeDeleted;

protected:
  //! Should return nullptr if no child is to be added.
  virtual std::unique_ptr<node_base> makeChild(const std::string& name) = 0;

  //! Reimplement for a specific removal action.
  virtual void removingChild(node_base& node_base) = 0;

  std::vector<std::unique_ptr<node_base>> mChildren;
  extended_attributes mExtended{0};
};

}
}
