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
#include "TimeValue.h"
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
  virtual ~TimeNode();

# pragma mark -
# pragma mark Execution

  /*! setup status of each #TimeEvent's #TimeNode considering a date
   \param #TimeValue a date that will be compared to the #TimeNode's date */
  virtual void setup(const TimeValue& = OSSIA::Zero) = 0;

  /*! make all #TimeEvent's #TimeNode to happen */
  virtual void happen() = 0;

  /*! dispose all #TimeEvent's #TimeNode */
  virtual void dispose() = 0;

# pragma mark -
# pragma mark Accessors

  /*! get the date
   \details the date is the sum of its previous #TimeConstraint durations
   \details a #TimeNode with na previous #TimeConstraints have a date equals to 0.
   \return #TimeValue the date */
  virtual TimeValue getDate() const = 0;

  /*! get the expression of the #TimeNode
   \return std::shared_ptr<#Expression> */
  virtual const std::shared_ptr<Expression> & getExpression() const = 0;

  /*! set the expression of the #TimeNode
   \details setting the expression to ExpressionTrue will defer the evaluation on #TimeEvent's expression
   \details setting the expression to ExpressionFalse will mute TimeNode execution
   \param std::shared_ptr<#Expression>
   \return #TimeNode the time node */
  virtual TimeNode & setExpression(const std::shared_ptr<Expression> = ExpressionTrue) = 0;

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
