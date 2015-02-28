/*!
 * \file Automation.h
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#ifndef AUTOMATION_H_
#define AUTOMATION_H_

#include <list>

#include "TimeProcess.h"

namespace OSSIA {

class Address;
class AddressValue;
class Curve;
class TimeValue;

class Automation : public TimeProcess {

public:

  // Factories, destructor
  static std::shared_ptr<Automation> create();
  virtual std::shared_ptr<Automation> clone() const = 0;
  virtual ~Automation() = default;

  // Lecture
  virtual void play(bool log = false, std::string name = "") const override = 0;

  // Accessors
  virtual AddressValue getStartValue() const = 0; //todo doublon avec Curve ?
  virtual void setStartValue(AddressValue) = 0;
  virtual AddressValue getEndValue() const = 0;
  virtual void setSEndValue(AddressValue) = 0;
  virtual const std::shared_ptr<Curve> & getCurve() const = 0;
  virtual void setCurve(std::shared_ptr<Curve>) = 0;
  virtual const std::shared_ptr<Address> & getInputAdress() const = 0;
  virtual void setInputAddress(std::shared_ptr<Address>) = 0;

  // Std container
  // Output Addresses
  class iterator;
  class const_iterator;
  class reverse_iterator;
  class const_reverse_iterator;
  class size_type;
  virtual iterator begin() = 0;
  virtual iterator end() = 0;
  virtual reverse_iterator rbegin() = 0;
  virtual reverse_iterator rend() = 0;
  virtual const_iterator begin() const = 0;
  virtual const_iterator end() const = 0;
  virtual const_reverse_iterator rbegin() const = 0;
  virtual const_reverse_iterator rend() const = 0;
  virtual const_iterator cbegin() const = 0;
  virtual const_iterator cend() const = 0;
  virtual const_reverse_iterator crbegin() const = 0;
  virtual const_reverse_iterator crend() const = 0;
  virtual size_type size() const = 0;
  virtual bool empty() const = 0;
  virtual std::shared_ptr<Address> & front() = 0;
  virtual const std::shared_ptr<Address> & front() const = 0;
  virtual std::shared_ptr<Address> & back() = 0;
  virtual const std::shared_ptr<Address> & back() const = 0;
  virtual iterator insert(const_iterator, std::shared_ptr<Address>) = 0;
  virtual iterator erase(const_iterator) = 0;
  virtual iterator erase(const_iterator first, const_iterator last) = 0;

};

}

#endif /* AUTOMATION_H_ */
