/*!
 * \file Address.h
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#pragma once
#include "Network/AddressValue.h"

#include <memory>
#include <string>

#include "Editor/Expression.h"
#include "Editor/ExpressionValue.h"

namespace OSSIA {

class Device;

class Address : public ExpressionValue, public Expression {

public:

  // Enumerations
  enum class AccessMode {
    GET,
    SET,
    BI
  };
  enum class BoundingMode {
    FREE,
    CLIP,
    WRAP,
    FOLD
  };

  // Destructor
  virtual ~Address() = default;

  // Network
  virtual const std::shared_ptr<Device> & getDevice() const = 0; // théo : is this really needed ? I don't know how to provide it
  virtual bool updateValue() const = 0;
  virtual bool sendValue(const AddressValue*) const = 0;

  // Accessors
  virtual AddressValue * getValue() const = 0;
  virtual AddressValue::Type getValueType() const = 0;

  virtual AccessMode getAccessMode() const = 0;
  //TODO virtual Domain getDomain() const = 0;
  virtual BoundingMode getBoundingMode() const = 0; //TODO multiple ?
  virtual bool getRepetitionFilter() const = 0;
  //TODO virtual Destination getDestination() const = 0;
  virtual Address & setAccessMode(AccessMode) = 0;
  //TODO virtual Address & setDomain(Domain) = 0;
  virtual Address & setBoundingMode(BoundingMode) = 0; //TODO multiple ?
  virtual Address & setRepetitionFilter(bool = true) = 0;
  //TODO virtual Address & setDestination(Destination) = 0;

};

}

