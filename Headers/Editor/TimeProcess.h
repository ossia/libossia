/*!
 * \file TimeProcess.h
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#ifndef TIMEPROCESS_H_
#define TIMEPROCESS_H_

#include <string>
#include <memory>
namespace OSSIA {

class Scheduler;
class State;
class TimeValue;
class TimeEvent;

class TimeProcess {

public:

  virtual ~TimeProcess() = default;

  virtual void play(bool log = false, std::string name = "") const = 0;

  // Navigation
  virtual const std::shared_ptr<TimeEvent> & getStartEvent() const = 0;
  virtual const std::shared_ptr<TimeEvent> & getEndEvent() const = 0;

  // Accessors
  virtual TimeValue getLength() const = 0;
  virtual void setLength(TimeValue) = 0;
  virtual const std::shared_ptr<State> & getStartState() const = 0;
  virtual void setStartState(std::shared_ptr<State>) = 0;
  virtual const std::shared_ptr<State> & getEndState() const = 0;
  virtual void setEndState(std::shared_ptr<State>) = 0;
  virtual const std::shared_ptr<Scheduler> & getScheduler() const = 0;
  virtual void setScheduler(std::shared_ptr<Scheduler>) = 0;

};

}

#endif /* TIMEPROCESS_H_ */
