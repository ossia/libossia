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

class Scenario;
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
  void play(bool log = false, std::string name = "") const = 0;

  // Accessors
  TimeValue getDate() const = 0;
  TimeValue getSimultaneityMargin() const = 0;
  void setSimultaneityMargin(TimeValue) = 0;

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
  virtual TimeEvent & front() = 0;
  virtual const TimeEvent & front() const = 0;
  virtual TimeEvent & back() = 0;
  virtual const TimeEvent & back() const = 0;
//  virtual iterator insert(const_iterator, std::shared_ptr<Event>) = 0;
  virtual iterator erase(const_iterator) = 0;
  virtual iterator erase(const_iterator first, const_iterator last) = 0;

};

}

#endif /* TIMENODE_H_ */
