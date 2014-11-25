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

  // Constructors, destructor, assignment
  State() = 0;
  virtual ~State() = default;

  // Lecture
  virtual void launch() const override = 0;

  // Iterators
  class const_iterator; // bidirectional
  const_iterator begin() const = 0;
  const_iterator end() const = 0;
  const_iterator find(const StateElement&) const = 0;

  // Managing StateElements
  void addStateElement(const StateElement&) = 0;
  bool removeStateElement(const StateElement&) = 0;

  // Accessors
  virtual StateElementType getType() const override final
      { return StateElementType::STATE_TYPE; };

};

}

#endif /* STATE_H_ */
