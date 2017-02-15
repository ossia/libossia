#pragma once
#include <ossia/network/common/address_properties.hpp>
#include <ossia/network/base/name_validation.hpp>

#include <ossia/detail/callback_container.hpp>
#include <ossia/detail/any_map.hpp>
#include <ossia/detail/ptr_container.hpp>
#include <ossia/detail/string_view.hpp>

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

  /** Allows to add arbitrary key-value metadata to nodes.
   * There is a list of pre-defined attributes available in \ref node_attributes.hpp
   */
  const extended_attributes& getExtendedAttributes() const;
  void setExtendedAttributes(const extended_attributes&);

  /** Get a specific attribute.
   * Usage :
   *
   * \code
   * node.setAttribute("my_int_attribute", 1234);
   * auto attr = node.getAttribute("my_int_attribute");
   * if(int* int_attr = boost::any_cast<int>(&attr))
   * {
   *   // The attribute exists and is an int.
   * }
   * \endcode
   */
  boost::any getAttribute(ossia::string_view str) const;

  template<typename T>
  void set(ossia::string_view str, const T& val);
  template<typename T>
  void set(ossia::string_view str, T&& val);

  template<typename Attribute, typename T>
  void set(Attribute a, const T& value);
  template<typename Attribute, typename T>
  void set(Attribute a, T&& value);

  /**
   * @brief createChild Adds a sub-child of the given name.
   *
   * @note The name of the child may be modified, so it should be checked after creation.
   *
   * If you need to add multiple childs in one go (for instance `/foo/bar/baz/blop` if this node
   * is `foo`), see ossia::net::find_or_create_node.
   *
   * @return A pointer to the child if it could be created, else nullptr.
   */
  node_base* createChild(std::string name);

  /**
   * @brief Adds a new child if it can be added.
   *
   * For instance if the name is already taken, it won't be added
   * and the returned pointer will be null.
   */
  node_base* addChild(std::unique_ptr<node_base>);

  /**
   * @brief Find a direct child of this node.
   *
   * e.g. `foo.findChild("bar")` will find `/foo/bar`, but not `/foo/blop/bar`.
   *
   * If you need to find a child recursively, see ossia::net::find_node.
   *
   */
  node_base* findChild(ossia::string_view name);

  bool removeChild(const std::string& name);
  bool removeChild(const node_base& name);

  //! Remove all the children.
  void clearChildren();

  operator const extended_attributes&() const { return mExtended; }
  operator extended_attributes&() { return mExtended; }

  const std::vector<std::unique_ptr<node_base>>& children() const
  {
    return mChildren;
  }

  //! A vector with all the names of the children.
  std::vector<std::string> childrenNames() const;

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

// address : format /a/b/c
OSSIA_EXPORT node_base*
find_node(node_base& dev, ossia::string_view address_base);
OSSIA_EXPORT node_base&
find_or_create_node(node_base& dev, ossia::string_view address_base);

}
}
