/*!
 * \file Scenario.h
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#ifndef SCENARIO_H_
#define SCENARIO_H_

#include <set>

#include "TimeProcess.h"
#include "Event.h"

namespace OSSIA {

class TimeBox;
class TimeNode;

class Scenario : public TimeProcess {

public:

  // Constructors, destructor, assignment
  Scenario();
  Scenario(const Scenario&);
  ~Scenario();
  Scenario & operator= (const Scenario&);

  // Lecture
  virtual void play() const override;

  // Navigation
  std::set<TimeBox*> getTimeBoxes() const;
  std::set<TimeNode*> getTimeNodes() const;

  // Edition
  void addTimeBox(const TimeBox&, const Event & startEvent);
  void addTimeBox(
      const TimeBox&,
      const Event & startEvent,
      const Event & endEvent);

  // Accessors
  Event & getStartEvent() const;
  void setStartEvent(const Event&);
  Event & getEndEvent() const;
  void setEndEvent(const Event&);

  // pimpl idiom
private:
  class Impl;
  Impl * pimpl;

};

}

#endif /* SCENARIO_H_ */
