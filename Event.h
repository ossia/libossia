/*!
 * \file Event.h
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#ifndef EVENT_H_
#define EVENT_H_

#include "IObservable.h"

namespace OSSIA {

class Scenario;
class State;

class Event : public IObservable {

  // Constructors, destructor, assignment
  Event();
  virtual ~Event();

  // Navigation
  Scenario & getParentScenario() const;

  // Lecture
  virtual void play() const;

  // Managing States
  virtual void addState(State*);
  virtual bool removeState(State*);

  // Accessors
  float getDate() const;

  // pimpl idiom
private:
  class Impl;
  Impl * pimpl;

};

}

#endif /* EVENT_H_ */
