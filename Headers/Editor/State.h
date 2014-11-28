/*!
 * \file State.h
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#ifndef STATE_H_
#define STATE_H_

#include "Editor/StateElement.h"

namespace OSSIA {

class State : public StateElement {

public:

  // Factory, destructor, assignment, conversion
  static State * create();
  virtual ~State() = default;
  virtual State & operator= (const State&) = 0;
  virtual operator State*() const override = 0;
  virtual operator Message*() const override {return nullptr;}

  // Lecture
  virtual void launch() const override = 0;

  // Iterators
  class const_iterator; // bidirectional
  virtual const_iterator begin() const = 0;
  virtual const_iterator end() const = 0;
  virtual const_iterator find(const StateElement&) const = 0;

  // Managing StateElements
  virtual void addStateElement(const StateElement&) = 0;
  virtual bool removeStateElement(const StateElement&) = 0;

};

}

#endif /* STATE_H_ */
