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
  State();
  State(const State&);
  virtual ~State();
  State & operator= (const State&);

  // Lecture
  virtual void launch() const override;

  // Navigation

  // Iterators
  class const_iterator; // bidirectional
  const_iterator begin() const;
  const_iterator end() const;
  const_iterator find(const StateElement&) const;

  // Managing StateElements
  void addStateElement(const StateElement&);
  bool removeStateElement(const StateElement&);

  // Accessors
  virtual StateElementType getType() const override final
      { return StateElementType::STATE_TYPE; };

  // pimpl idiom
private:
  class Impl;
  Impl * pimpl;

};

}

#endif /* STATE_H_ */
