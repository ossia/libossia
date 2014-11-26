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

  // Factory, destructor
  static State * create();
  virtual ~State() = default;

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

  // Accessors
  virtual StateElementType getType() const override final
      { return StateElementType::STATE_TYPE; };

};

}

#endif /* STATE_H_ */
