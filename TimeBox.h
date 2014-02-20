/*!
 * \file TimeBox.h
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#ifndef TIMEBOX_H_
#define TIMEBOX_H_

#include <string>

#include "IObservable.h"

namespace OSSIA {

class Event;
class Scenario;
class State;
class TimeNode;
class TimeProcess;
class TimeValue;

class TimeBox : public IObservable {

public:

  // Constructors, destructor, assignment
  TimeBox();
  TimeBox(const TimeBox&);
  ~TimeBox();
  TimeBox & operator= (const TimeBox&);

  // Lecture
  void play(bool log = false, std::string name = "") const;

  // Navigation
  TimeNode & getPreviousNode() const;
  TimeNode & getNextNode();
  Event & getStartEvent() const;
  Event & getEndEvent() const;
  Scenario & getParentScenario() const;

  // Iterators
  class const_iterator; // bidirectional
  const_iterator begin() const;
  const_iterator end() const;
  const_iterator find(const TimeProcess&) const;

  // Managing TimeProcesses
  void addTimeProcess(const TimeProcess&);
  bool removeTimeProcess(const TimeProcess&);

  // Accessors
  TimeValue getLength() const;
  void setLength(TimeValue);
  TimeValue getMinimumLength() const;
  void setMinimumLength(TimeValue);
  TimeValue getMaximumLength() const;
  void setMaximumLength(TimeValue);
  bool isRigid();
  State & getStartState() const;
  void setStartState(const State&);
  State & getEndState() const;
  void setEndState(const State&);

  // Observation message types
  enum MessageType {
    LENGTH,
    MIN_LENGTH,
    MAX_LENGTH
  };

  // pimpl idiom
private:
  class Impl;
  Impl * pimpl;

};

}

#endif /* TIMEBOX_H_ */
