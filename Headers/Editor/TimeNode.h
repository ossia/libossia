/*!
 * \file TimeNode.h
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#pragma once

#include <memory>
#include <string>

#include "Misc/Container.h"

namespace OSSIA {

class Expression;
class Scenario;
class State;
class TimeConstraint;
class TimeEvent;
class TimeValue;

class TimeNode : public virtual Container<TimeEvent> {

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

      // TimeEvent Factory
      virtual iterator emplace(const_iterator,
                               std::shared_ptr<State>/*todo = NO_STATE*/,
                               std::shared_ptr<Expression>/*todo = NO_EXPRESSION*/) = 0;

    };

}
