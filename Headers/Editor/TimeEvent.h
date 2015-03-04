/*!
 * \file TimeEvent.h
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#ifndef TIMEEVENT_H_
#define TIMEEVENT_H_

#include <memory>
#include <string>

namespace OSSIA {

class Expression;

class TimeEvent {

public:

  virtual ~TimeEvent() = default;

  // Lecture
  virtual void play(bool log = false, std::string name = "") const = 0;

  // Accessors //todo is it necessary ?
  virtual const std::shared_ptr<State> & getState() const = 0;
  virtual const std::shared_ptr<Expression> & getExpression() const = 0;
  virtual TimeProcessList & getPreviousProcesses() = 0;
  virtual TimeProcessList & getNextProcesses() = 0;

};

}



#endif /* TIMEEVENT_H_ */
