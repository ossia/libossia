/*!
 * \file Node.h
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#ifndef NODE_H
#define NODE_H

#include "Network/AddressValue.h"

#include "Misc/Container.h"

namespace OSSIA {

class Node : public Container<Node> {

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

#endif // NODE_H

