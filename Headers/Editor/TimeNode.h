/*!
 * \file TimeNode.h
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#ifndef TIMENODE_H_
#define TIMENODE_H_

#include <memory>
#include <string>

namespace OSSIA {

class Expression;
class Scenario;
class State;
class TimeConstraint;
class TimeEvent;
class TimeValue;

class TimeNode {

public:

  // Factories, destructor
  static std::shared_ptr<TimeNode> create();
  virtual std::shared_ptr<TimeNode> clone() const = 0;
  virtual ~TimeNode() = default;

  // Lecture
  virtual void play(bool log = false, std::string name = "") const = 0;

  // Accessors
  virtual TimeValue getDate() const = 0;
  virtual TimeValue getSimultaneityMargin() const = 0;
  virtual void setSimultaneityMargin(TimeValue) = 0; //todo why not in constructor (only) ?

  // Std container //todo inherit Container ? because emplace is only here now
  class iterator;
  class const_iterator;
  class size_type;
  virtual iterator begin() = 0;
  virtual iterator end() = 0;
  virtual const_iterator cbegin() const = 0;
  virtual const_iterator cend() const = 0;
  virtual size_type size() const = 0;
  virtual bool empty() const = 0;
  virtual std::shared_ptr<TimeEvent> & front() = 0;
  virtual const std::shared_ptr<TimeEvent> & front() const = 0;
  virtual std::shared_ptr<TimeEvent> & back() = 0;
  virtual const std::shared_ptr<TimeEvent> & back() const = 0;
//  virtual iterator insert(const_iterator, std::shared_ptr<Event>) = 0;
  virtual iterator emplace(const_iterator,
                           std::shared_ptr<State>/*todo = NO_STATE*/,
                           std::shared_ptr<Expression>/*todo = NO_EXPRESSION*/) = 0;
  virtual iterator erase(const_iterator) = 0;
  virtual iterator erase(const_iterator first, const_iterator last) = 0;

};

}

#endif /* TIMENODE_H_ */
