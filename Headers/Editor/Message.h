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
  virtual Message & operator= (const Message&) = 0;
  virtual operator State*() const override {return nullptr;}
  virtual operator Message*() const override = 0;

  // Lecture
  virtual void launch() const override = 0;

  // Accessors
  virtual Address & getAddress() const = 0;
  virtual AddressType getValue() const = 0;

};

}

#endif /* MESSAGE_H_ */
