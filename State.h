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

#include "IObservable.h"

namespace OSSIA {

class State : public IObservable {

  // Constructors, destructor, assignment
  State();
  State(const State&);
  ~State();
  State & operator= (const State&);

  // Lecture
  void launch() const;

  // Navigation

  // Iterators
  class const_iterator; // bidirectional
  const_iterator begin() const;
  const_iterator end() const;
  const_iterator find(const State&) const;

  // Managing sub-States
  void addState(const State&);
  bool removeState(const State&);

  // Accessors

  // pimpl idiom
private:
  class Impl;
  Impl * pimpl;

};

}

#endif /* STATE_H_ */
