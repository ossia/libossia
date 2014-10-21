/*!
 * \file Device.h
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#ifndef DEVICE_H_
#define DEVICE_H_

namespace OSSIA {

template <typename T>
class Device {

public:

  typedef T protocol_type;

  // Constructors, destructor, assignment
  Device();
  Device(const Device&);
  Device(T * protocol);
  ~Device();
  Device & operator= (const Device&);

  // Factories
  Address addAddress(std::string) const;
  template <typename U>
  Address addAddress(std::string) const;
  template <typename U>
  Address addAddress(std::string, U min, U max) const;

  // Iterators
  class const_iterator; // bidirectionnal
  const_iterator begin() const;
  const_iterator end() const;
  const_iterator find(Address) const;

  // Saving
  bool save(std::string) const;
  bool load(std::string) const;

};

}

#endif /* DEVICE_H_ */
