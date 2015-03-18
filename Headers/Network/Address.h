/*!
 * \file Address.h
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#ifndef ADDRESS_H_
#define ADDRESS_H_

#include <string>

#include "Editor/Expression.h"
#include "Editor/ExpressionValue.h"

namespace OSSIA {

class Address : public ExpressionValue, public Expression {

public:

  // Destructor
  virtual ~Address() = default;

  // Network
  virtual const std::shared_ptr<Device> & getDevice() const = 0;
  virtual bool updateValue() const = 0;
  virtual bool sendValue(AddressValue) const = 0;

  // Accessors
  virtual AddressValue getValue() const = 0;
  virtual AddressValue::Type getValueType() const = 0;
  virtual AccessMode getAccessMode() const = 0;
  virtual Domain getDomain() const = 0;
  virtual BoundingModes getBoundingMode() const = 0;
  virtual bool getRepetitionFilter() const = 0;
  virtual Destination getDestination() const = 0;
  virtual Address & setAccessMode(AccessMode) = 0;
  virtual Address & setDomain(Domain) = 0;
  virtual Address & setBoundingMode(BoundingModes) = 0;
  virtual Address & setRepetitionFilter(bool = true) = 0;
  virtual Address & setDestination(Destination) = 0;

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

};

}

#endif /* ADDRESS_H_ */
