/*!
 * \file Node.h
 *
 * \defgroup Network
 *
 * \brief
 *
 * \details
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * \copyright This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#pragma once

#include "Editor/Value.h"
#include "Network/Address.h"
#include "Misc/Container.h"
#include "Misc/CallbackContainer.h"
#include "Network/AddressProperties.h"

namespace OSSIA
{

class Address;
class Device;

/*! type of of change on a #Node */
enum class NodeChange
{
  RENAMED,
  EMPLACED,
  ERASED,
  ADDRESS_CREATED,
  ADDRESS_REMOVED
};

/*! to track any modifications done on a node or its children
 \param the node that have changed
 \param the name of the node that have changed (when it has been renamed it is the former name)
 \param the change type */
using NodeChangeCallback = std::function<void(const Node&, const std::string&, NodeChange)>;

class Node : public CallbackContainer<NodeChangeCallback>
{

public:

# pragma mark -
# pragma mark Definitions

  using iterator = typename CallbackContainer<NodeChangeCallback>::iterator;

# pragma mark -
# pragma mark Life cycle

  /*! destructor */
  virtual ~Node();

# pragma mark -
# pragma mark Network

  /*! get node's device
   \return std::shared_ptr<Device> the device */
  virtual std::shared_ptr<Device> getDevice() const = 0;

  /*! get node's parent
   \return std::shared_ptr<Node> the parent */
  virtual std::shared_ptr<Node> getParent() const = 0;

  /*! get a shared_ptr to this node
   \return std::shared_ptr<Node> a shared_ptr to this node */
  virtual std::shared_ptr<Node> getThis() = 0;

# pragma mark -
# pragma mark Accessors

  /*! get node's name
   \return std::string the name */
  virtual std::string getName() const = 0;

  /*! set node's name
   \details as each node name is unique (among its brothers), call getName() afterward to have the effective name of the node
   \param std::string a new name
   \return #Node the node */
  virtual Node & setName(std::string) = 0;

  /*! get node's address
   \return std::shared_ptr<#Address> the address */
  virtual std::shared_ptr<OSSIA::Address> getAddress() const = 0;

# pragma mark -
# pragma mark Address

  /*! create node's address
   \param #Value::Type the type of the address to create
   \return std::shared_ptr<#Address> the new address */
  virtual std::shared_ptr<OSSIA::Address> createAddress(Value::Type = Value::Type::IMPULSE) = 0;

  /*! remove node's address
   \return bool true if the address is correctly removed */
  virtual bool removeAddress() = 0;

# pragma mark -
# pragma mark Children

  /*! create and store a child node
   \param #Container<#Node>::const_iterator where to create the child
   \param std::string child name
   \return #Container<#Node>::iterator */
  virtual Container<Node>::iterator emplace(Container<Node>::const_iterator, std::string) = 0;

  /*! create and store a child node and create an address
   \param #Container<#Node>::const_iterator where to create the child
   \param std::string child name
   \param
   \param
   \param
   \param
   \param
   \return #Container<#Node>::iterator */
  virtual Container<Node>::iterator emplace(Container<Node>::const_iterator,
                                            const std::string&,
                                            Value::Type,
                                            AccessMode = {},
                                            const std::shared_ptr<Domain>& = {},
                                            BoundingMode = {},
                                            bool repetitionFilter = {}) = 0;

  /*! store an existing node to create an alias
   \param #Container<#Node>::const_iterator where to store the child
   \param shared_ptr<Node> the #Node to store
   \param std::string child name
   \return #Container<#Node>::iterator */
  virtual Container<Node>::iterator insert(Container<Node>::const_iterator,
                                           std::shared_ptr<Node>,
                                           std::string) = 0;

  /*! erased a node
   \param #Container<#Node>::const_iterator the position of the node to erase
   \return #Container<#Node>::iterator */
  virtual Container<Node>::iterator erase(Container<Node>::const_iterator) = 0;

  /*! get children of the node
   \return #Container<#Node> */
  const Container<Node>& children() const
  { return m_children; }

protected:
  Container<Node> m_children;
};

# pragma mark -
# pragma mark Destination

/*! \details Destination to an Address value and optionnally to several index of this value */
struct Destination final : public Value
{
  std::shared_ptr<Node> value;
  std::vector<char> index;

  /*! constructor for a node and optionnal index values
   \param std::shared_ptr<Node>
   \param char
   \param char
   \param ... */
  Destination(std::shared_ptr<Node> v, std::initializer_list<char> = {});

  /*! constructor for a node and an index vector
   \param std::shared_ptr<Node>
   \param std::vector<const #Value> value */
  Destination(std::shared_ptr<Node> v, std::vector<char>);

  /*! clone */
  Value * clone() const override;

  /*! equal operator */
  bool operator== (const Value&) const override;

  /*! different operator */
  bool operator!= (const Value&) const override;

  /*! greater than operator */
  bool operator> (const Value&) const override;

  /*! greater than and equal operator */
  bool operator>= (const Value&) const override;

  /*! less than operator */
  bool operator< (const Value&) const override;

  /*! less than and equal operator */
  bool operator<= (const Value&) const override;
};

}
