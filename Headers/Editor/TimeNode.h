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
class State;
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
  
  /*! get the date
   \return #TimeValue the date */
  virtual TimeValue getDate() const = 0;
  
  /*! get the simultaneity margin
   \return #TimeValue the simultaneity margin */
  virtual TimeValue getSimultaneityMargin() const = 0;
  
  /*! set the simultaneity margin
   \todo remove setter and move the setting into constructor (?)
   \param #TimeValue the simultaneity margin */
  virtual void setSimultaneityMargin(TimeValue) = 0;

# pragma mark -
# pragma mark TimeEvents
  
  /*! create and store a time event
   \param #Container<#TimeEvent>::const_iterator where to store the time event
   \param std::shared_ptr<#State> state (\todo NO_STATE)
   \return std::shared_ptr<#Expression> expression (\todo NO_EXPRESSION) */
  virtual iterator emplace(const_iterator,
                           std::shared_ptr<State> = nullptr,
                           std::shared_ptr<Expression> = nullptr) = 0;
  
  /*! get the time events of the time node
   \return #Container<#TimeEvent> container */
  Container<TimeEvent>& timeEvents()
  { return m_timeEvents; }
  
  /*! get the time events of the time node
   \return #Container<#TimeEvent> container */
  const Container<TimeEvent>& timeEvents() const
  { return m_timeEvents; }

private:
  Container<TimeEvent> m_timeEvents;
};

}
