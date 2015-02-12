/*!
 * \file Constraint.h
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#ifndef CONSTRAINT_H_
#define CONSTRAINT_H_

#include <string>

namespace OSSIA {

class Scenario;
class State;
class TimeNode;
class TimeProcess;
class TimeValue;

class Constraint {

public:

  // Constructors, destructor, assignment
  Constraint();
  Constraint(const Constraint&);
  ~Constraint();
  Constraint & operator= (const Constraint&);

  // Lecture
  void play(bool log = false, std::string name = "") const;

  // Navigation
  TimeNode & getPreviousNode() const;
  TimeNode & getNextNode() const;
  State & getStartState() const;
  void setStartState(const State&);
  State & getEndState() const;
  void setEndState(const State&);
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

  // pimpl idiom
private:
  class Impl;
  Impl * pimpl{};

};

}

#endif /* CONSTRAINT_H_ */
