/*!
 * \file Message.h
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#ifndef MESSAGE_H_
#define MESSAGE_H_

#include "StateElement.h"

namespace OSSIA {

class Address;
class AddressType;

class Message : public StateElement {

public:

  // Factory, destructor
  static Message * create(Address, AddressType = NO_VALUE);
  virtual ~Message() = default;

  // Lecture
  virtual void launch() const override = 0;

  // Accessors
  virtual Address & getAddress() const = 0;
  virtual AddressType getValue() const = 0;
  virtual StateElementType getType() const override final
      { return StateElementType::MESSAGE_TYPE; };

};

}

#endif /* MESSAGE_H_ */
