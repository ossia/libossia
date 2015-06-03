/*!
 * \file TimeNode.h
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

#include "Misc/Container.h"

namespace OSSIA
{

class Expression;
class Scenario;
class State;
class TimeConstraint;
class TimeEvent;
class TimeValue;

class TimeNode
{

public:

  using iterator = Container<TimeEvent>::iterator;
  using const_iterator = Container<TimeEvent>::const_iterator;

# pragma mark -
# pragma mark Life cycle
  
  /*! factory 
   \return std::shared_ptr<#TimeNode> */
  static std::shared_ptr<TimeNode> create();
  
  /*! clone */
  virtual std::shared_ptr<TimeNode> clone() const = 0;
  
  /*! destructor */
  virtual ~TimeNode() = default;

# pragma mark -
# pragma mark Execution
  
  /*! execute the scenario and optionnaly log the execution into a file
   \param bool to enable log
   \param string to give a log file name where to write */
  virtual void play(bool log = false, std::string name = "") const = 0;

# pragma mark -
# pragma mark Accessors
  
  virtual TimeValue getDate() const = 0;
  virtual TimeValue getSimultaneityMargin() const = 0;
  virtual void setSimultaneityMargin(TimeValue) = 0; //TODO why not in constructor (only) ?

# pragma mark -
# pragma mark TimeEvent Factory
  
  virtual iterator emplace(const_iterator,
                           std::shared_ptr<State>/*TODO = NO_STATE*/,
                           std::shared_ptr<Expression>/*TODO = NO_EXPRESSION*/) = 0;

# pragma mark -
# pragma mark Internals
  
  Container<TimeEvent>& timeProcesses()
  { return m_timeEvents; }
  
  const Container<TimeEvent>& timeProcesses() const
  { return m_timeEvents; }

private:
  Container<TimeEvent> m_timeEvents;
};

}
