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

namespace OSSIA {

class Address;

template <typename value_type>
class Message {

  // Constructors, destructor, asssignment
  Message();

  // Accessors
  Address getAddress() const;
  void setAddress(Address);
  value_type getValue() const;
  void setValue(value_type);
  bool hasValue();

  // pimpl idiom
private:
  class Impl;
  Impl * pimpl;

};

}

#endif /* MESSAGE_H_ */
