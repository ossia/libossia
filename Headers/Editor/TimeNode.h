/*!
 * \file TimeNode.h
 *
 * \defgroup Editor
 *
 * \brief #TimeNode is use to describe temporal structure to synchronize each attached #TimeEvent evaluation.
 *
 * \details #TimeNode is also a #TimeEvent container.
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

#include "Expression.h"
#include "TimeEvent.h"
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
  
  /*! execute and optionnaly log the execution into a file
   \param bool to enable log
   \param string to give a log file name where to write */
  virtual void play(bool log = false, std::string name = "") const = 0;

# pragma mark -
# pragma mark Accessors
  
  /*! get the date
   \return #TimeValue the date */
  virtual TimeValue getDate() const = 0;
    
  /*! get the expression of the #TimeNode
   \return std::shared_ptr<#Expression> */
  virtual const std::shared_ptr<Expression> & getExpression() const = 0;
  
  /*! get the expression of the #TimeNode
   \param std::shared_ptr<#Expression>
   \return #TimeNode the time node */
  virtual TimeNode & setExpression(const std::shared_ptr<Expression>) = 0;
  
  /*! get the simultaneity margin
   \return #TimeValue the simultaneity margin */
  virtual TimeValue getSimultaneityMargin() const = 0;
  
  /*! set the simultaneity margin
   \todo remove setter and move the setting into constructor (?)
   \param #TimeValue the simultaneity margin 
   \return #TimeNode the time node */
  virtual TimeNode & setSimultaneityMargin(const TimeValue&) = 0;

# pragma mark -
# pragma mark #TimeEvents
  
  /*! create and store a #TimeEvent
   \param #Container<#TimeEvent>::const_iterator where to store the #TimeEvent
   \param #TimeEvent::ExecutionCallback to get #TimeEvent's status back,
   \param std::shared<#Expression> an optionnal #Expression to apply to the #TimeEvent
   \return std::shared_ptr<#TimeEvent> */
  virtual iterator emplace(const_iterator,
                           TimeEvent::ExecutionCallback,
                           std::shared_ptr<Expression> = ExpressionTrue) = 0;
  
  /*! get the #TimeEvents of the #TimeNode
   \return #Container<#TimeEvent> */
  Container<TimeEvent>& timeEvents()
  { return m_timeEvents; }
  
  /*! get the #TimeEvents of the #TimeNode
   \return #Container<#TimeEvent> */
  const Container<TimeEvent>& timeEvents() const
  { return m_timeEvents; }

private:
  Container<TimeEvent> m_timeEvents;
};

}
