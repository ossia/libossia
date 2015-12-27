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

namespace OSSIA
{

class Address;
class Device;

class Node
{

public:

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
  virtual std::shared_ptr<OSSIA::Address> createAddress(
                Value::Type = Value::Type::IMPULSE) = 0;

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

  /*! store an existing node to create an alias
   \param #Container<#Node>::const_iterator where to store the child
   \param shared_ptr<Node> the #Node to store
   \param std::string child name
   \return #Container<#Node>::iterator
   */
  virtual Container<Node>::iterator insert(Container<Node>::const_iterator, std::shared_ptr<Node>, std::string) = 0;

  Container<Node>::iterator emplaceAndNotify(
          Container<Node>::const_iterator requested_it,
          std::string str);

  Container<Node>::iterator erase(
          Container<Node>::const_iterator);

  Container<Node>::iterator eraseAndNotify(
          Container<Node>::const_iterator);

  /*! get children of the node
   \return #Container<#Node> */
  const Container<Node>& children() const
  { return m_children; }

protected:
  Container<Node> m_children;
};

# pragma mark -
# pragma mark Destination

/*! \details Destination value */
struct Destination final : public Value
{
  std::shared_ptr<Node> value;

  /*! constructor */
  Destination(std::shared_ptr<Node> v);

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
