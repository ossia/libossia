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
class Curve;
class TimeValue;

class Automation : public TimeProcess {

public:

  // Constructors, destructor, assignement
  Automation();
  Automation(const Automation&);
  ~Automation();
  Automation & operator= (const Automation&);

  // Lecture
  virtual void play() const override;

  // Edition

  // Iterators
  class const_iterator; // bidirectional
  const_iterator begin() const;
  const_iterator end() const;
  const_iterator find(const Address&) const;

  // Managing Addresses
  void addAddress(const Address&);
  bool removeAddress(const Address&);

  // Accessors
  TimeValue getPeriod() const;
  void setPeriod(const TimeValue);
  Curve & getCurve() const;
  void setCurve(const Curve&);

  // pimpl idiom
private:
  class Impl;
  Impl * pimpl;

};

}

#endif /* AUTOMATION_H_ */
