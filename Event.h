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

#include <set>

#include "TimeNode.h"

namespace OSSIA {

class Scenario;
class State;
class TimeBox;

class Event : public TimeNode {

public:

  // Constructors, destructor, assignment
  Event();
  Event(const Event&);
  virtual ~Event();
  Event & operator= (const Event&);

  // Lecture
  virtual void play(bool log = false, std::string name = "") const override;

  // Navigation
  virtual std::set<TimeBox*> getPreviousTimeBoxes() const override;
  virtual std::set<TimeBox*> getNextTimeBoxes() const override;

  // Accessors
  State & getState() const;
  void setState(const State&);

  // pimpl idiom
private:
  class Impl;
  Impl * pimpl;

};

}

#endif /* EVENT_H_ */
