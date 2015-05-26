/*!
 * \file Node.h
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#pragma once

#include "Network/AddressValue.h"

#include "Misc/Container.h"

namespace OSSIA {

class Address;

class Node : public virtual Container<Node> {

public:

  // Destructor
  virtual ~Node() = default;

  // Navigation
  virtual Node & getParent() const = 0;

  // Accessors
  virtual std::string getName() const = 0;
  virtual const std::shared_ptr<Address> & getAddress() const = 0;
  virtual bool removeAddress() = 0;

  // Address Factory
  virtual std::shared_ptr<Address> createAddress(
      AddressValue::Type = AddressValue::Type::NONE) = 0;

  // Child Node Factory
  virtual iterator emplace(const_iterator, std::string) = 0;

};

}
