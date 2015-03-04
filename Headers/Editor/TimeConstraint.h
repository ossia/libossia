/*!
 * \file TimeConstraint.h
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#ifndef TIMECONSTRAINT_H_
#define TIMECONSTRAINT_H_

#include <memory>
#include <string>

#include "Misc/Container.h"

namespace OSSIA {

class State;
class TimeNode;
class TimeProcess;
class TimeValue;

class TimeConstraint : public Container<TimeProcess>{

public:

  // Factories, destructor
  static std::shared_ptr<TimeConstraint> create(TimeValue nominal,
                                            TimeValue min = nominal,
                                            TimeValue max = nominal);
  virtual std::shared_ptr<TimeConstraint> clone() const = 0;
  virtual ~TimeConstraint() = default;

  // Lecture
  void play(bool log = false, std::string name = "") const = 0;

  // Navigation
  const std::shared_ptr<TimeNode> & getStartNode() const = 0;
  const std::shared_ptr<TimeNode> & getEndNode() const = 0;

  // Accessors
  const std::shared_ptr<State> & getStartState() const = 0;
  void setStartState(std::shared_ptr<State>) = 0;
  const std::shared_ptr<State> & getEndState() const = 0;
  void setEndState(std::shared_ptr<State>) = 0;

};

}

#endif /* TIMECONSTRAINT_H_ */
