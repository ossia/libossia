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

class TimeNode{

    public:
        using iterator = Container<TimeEvent>::iterator;
        using const_iterator = Container<TimeEvent>::const_iterator;

        // Constructors, destructor, cloning
        static std::shared_ptr<TimeNode> create();
        virtual std::shared_ptr<TimeNode> clone() const = 0;
        virtual ~TimeNode() = default;

        // Execution
        virtual void play(bool log = false, std::string name = "") const = 0;

        // Accessors
        virtual TimeValue getDate() const = 0;
        virtual TimeValue getSimultaneityMargin() const = 0;
        virtual void setSimultaneityMargin(TimeValue) = 0; //TODO why not in constructor (only) ?

        // TimeEvent Factory
        virtual iterator emplace(const_iterator,
                                 std::shared_ptr<State>/*TODO = NO_STATE*/,
                                 std::shared_ptr<Expression>/*TODO = NO_EXPRESSION*/) = 0;

        Container<TimeEvent>& timeProcesses()
        { return m_timeEvents; }
        const Container<TimeEvent>& timeProcesses() const
        { return m_timeEvents; }

    private:
        Container<TimeEvent> m_timeEvents;

};

}
