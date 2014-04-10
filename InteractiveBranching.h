/*!
 * \file InteractiveBranching.h
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#ifndef INTERACTIVEBRANCHING_H_
#define INTERACTIVEBRANCHING_H_

#include <set>

#include "TimeNode.h"

namespace OSSIA {

class Event;
class Expression;
class TimeBox;
class TimeValue;

class InteractiveBranching : public TimeNode {

public:

  // Constructors, destructor, assignment
  InteractiveBranching();
  InteractiveBranching(const InteractiveBranching&);
  virtual ~InteractiveBranching();
  InteractiveBranching & operator= (const InteractiveBranching&);

  // Lecture
  virtual void play(bool log = false, std::string name = "") const override;

  // Navigation
  virtual std::set<TimeBox*> getPreviousTimeBoxes() const override;
  virtual std::set<TimeBox*> getNextTimeBoxes() const override;

  // Iterators
  class const_iterator; // bidirectional, upon std::pair<Event,Expression>
  const_iterator begin() const;
  const_iterator end() const;
  const_iterator find(const Event&) const;

  // Managing branches
  void addBranch(const Event&, const Expression); // ?? StartEvent ?
  bool removeBranch(const Event&);

  // Accessors
  Event & getEvent() const;
  void setEvent(const Event&);
  TimeValue getPreListenningDuration() const;
  void setPreListenningDuration(TimeValue);
  TimeValue getSimultaneityMargin();
  void setSimultaneityMargin(TimeValue);
  bool isMultiple();

  // pimpl idiom
private:
  class Impl;
  Impl * pimpl;

};

}

#endif /* INTERACTIVEBRANCHING_H_ */
