/*!
 * \file TimeEvent.h
 *
 * \brief
 *
 * \details
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * \copyright This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#pragma once

#include <memory>
#include <string>
#include "TimeProcessList.h"

namespace OSSIA {

class Expression;
class State;
class TimeEvent {

    public:

      virtual ~TimeEvent() = default;

    # pragma mark -
# pragma mark Execution
      virtual void play(bool log = false, std::string name = "") const = 0;

    # pragma mark -
# pragma mark Accessors //TODO is it necessary ?
      virtual const std::shared_ptr<State> & getState() const = 0;
      virtual const std::shared_ptr<Expression> & getExpression() const = 0;
      virtual TimeProcessList & getPreviousProcesses() = 0;
      virtual TimeProcessList & getNextProcesses() = 0;

    };

}

