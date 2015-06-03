/*!
 * \file Node.h
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

#include "Network/AddressValue.h"

#include "Misc/Container.h"

namespace OSSIA
{

class Address;

class Node
{

public:
  
  using iterator = Container<Node>::iterator;
  using const_iterator = Container<Node>::const_iterator;

# pragma mark -
# pragma mark Life cycle
  virtual ~Node() = default;

# pragma mark -
# pragma mark Navigation
  virtual std::shared_ptr<Node> getParent() const = 0;

# pragma mark -
# pragma mark Accessors
  virtual std::string getName() const = 0;
  virtual const std::shared_ptr<Address> & getAddress() const = 0;
  virtual bool removeAddress() = 0;

# pragma mark -
# pragma mark Factory
  virtual std::shared_ptr<Address> createAddress(
                AddressValue::Type = AddressValue::Type::NONE) = 0;

  virtual iterator emplace(const_iterator, std::string) = 0;

# pragma mark -
# pragma mark Internals
  Container<Node>& children()
  { return m_nodes; }
  
  const Container<Node>& children() const
  { return m_nodes; }

private:
  Container<Node> m_nodes;
};

}
