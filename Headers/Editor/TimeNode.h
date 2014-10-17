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

#include <set>

#include "Misc/IObservable.h"

namespace OSSIA {

class Expression;
class Scenario;
class TimeBox;
class TimeValue;

class TimeNode : public IObservable {

public:

  // Constructors, destructor, assignment
  TimeNode();
  TimeNode(const TimeNode&);
  ~TimeNode();
  TimeNode & operator= (const TimeNode&);

  // Lecture
  void play(bool log = false, std::string name = "") const;

  // Navigation
  std::set<TimeBox*> getPreviousTimeBoxes() const;
  std::set<TimeBox*> getNextTimeBoxes() const;
  Scenario & getParentScenario() const;

  // Iterators
  class const_iterator;
  // bidirectional, upon pair<State, pair<Expression, set<TimeBox*> > >
  const_iterator begin() const;
  const_iterator end() const;
  const_iterator find(const State&) const;

  // Managing states
  void addState(const State&, const Expression, std::set<TimeBox*>);
  bool removeState(const State&);

  // Accessors
  TimeValue getDate() const;
  TimeValue getPreListenningDuration() const;
  void setPreListenningDuration(TimeValue);
  TimeValue getSimultaneityMargin();
  void setSimultaneityMargin(TimeValue);

  // pimpl idiom
private:
  class Impl;
  Impl * pimpl;

};

}

#endif /* TIMENODE_H_ */
