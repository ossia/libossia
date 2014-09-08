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

class Scenario;
class TimeBox;
class TimeValue;

class TimeNode : public IObservable {

public:

  // Constructors, destructor, assignment
  virtual ~TimeNode() = default;

  // Lecture
  virtual void play(bool log = false, std::string name = "") const = 0; // ??

  // Navigation
  virtual std::set<TimeBox*> getPreviousTimeBoxes() const = 0;
  virtual std::set<TimeBox*> getNextTimeBoxes() const = 0;
  Scenario & getParentScenario() const;

  // Accessors
  TimeValue getDate() const;

  // pimpl idiom
private:
  class Impl;
  Impl * pimpl;

};

}

#endif /* TIMENODE_H_ */
