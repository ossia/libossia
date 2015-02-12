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

#include <memory>
#include <string>

namespace OSSIA {

class Scenario;
class State;
class TimeNode;
class TimeProcess;
class TimeValue;

class Constraint {

public:

  // Factories, destructor
  static std::shared_ptr<Constraint> create(TimeValue nominal,
                                            TimeValue min = nominal,
                                            TimeValue max = nominal);
  virtual std::shared_ptr<Constraint> clone() const = 0;
  virtual ~Constraint() = default;

  // Lecture
  void play(bool log = false, std::string name = "") const = 0;

  // Navigation
  const std::shared_ptr<TimeNode> & getStartEvent() const = 0;
  const std::shared_ptr<TimeNode> & getEndEvent() const = 0;
  const std::shared_ptr<Scenario> & getParentScenario() const = 0;

  // Accessors
  const std::shared_ptr<State> & getStartState() const = 0;
  void setStartState(std::shared_ptr<State>) = 0;
  const std::shared_ptr<State> & getEndState() const = 0;
  void setEndState(std::shared_ptr<State>) = 0;

  // Std container
  class iterator;
  class const_iterator;
  class reverse_iterator;
  class const_reverse_iterator;
  class size_type;
  virtual iterator begin() = 0;
  virtual iterator end() = 0;
  virtual reverse_iterator rbegin() = 0;
  virtual reverse_iterator rend() = 0;
  virtual const_iterator begin() const = 0;
  virtual const_iterator end() const = 0;
  virtual const_reverse_iterator rbegin() const = 0;
  virtual const_reverse_iterator rend() const = 0;
  virtual const_iterator cbegin() const = 0;
  virtual const_iterator cend() const = 0;
  virtual const_reverse_iterator crbegin() const = 0;
  virtual const_reverse_iterator crend() const = 0;
  virtual size_type size() const = 0;
  virtual bool empty() const = 0;
  virtual std::shared_ptr<TimeProcess> & front() = 0;
  virtual const std::shared_ptr<TimeProcess> & front() const = 0;
  virtual std::shared_ptr<TimeProcess> & back() = 0;
  virtual const std::shared_ptr<TimeProcess> & back() const = 0;
  virtual iterator insert(const_iterator, std::shared_ptr<TimeProcess>) = 0;
  virtual iterator erase(const_iterator) = 0;
  virtual iterator erase(const_iterator first, const_iterator last) = 0;

};

}

#endif /* CONSTRAINT_H_ */
