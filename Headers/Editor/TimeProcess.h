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

namespace OSSIA {

class State;
class Constraint;

class TimeProcess {

public:

  // Constructors, destructor, assignment
  TimeProcess();
  virtual ~TimeProcess();

  // Lecture
  virtual void play() const;

  // Navigation
  Constraint & getParentTimeBox();

  // Accessors
  State & getStartState() const;
  void setStartState(const State&);
  State & getEndState() const;
  void setEndState(const State&);

  // pimpl idiom
private:
  class Impl;
  Impl * pimpl{};

};

}

#endif /* TIMEPROCESS_H_ */
