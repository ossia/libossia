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

#include <ossia/editor/expression/expression.hpp>
#include <ossia/editor/scenario/time_event.hpp>
#include <ossia/editor/scenario/time_value.hpp>
#include <ossia/detail/ptr_container.hpp>
#include <ossia_export.h>

namespace OSSIA
{

class Expression;
class State;
class TimeEvent;
class TimeValue;

class OSSIA_EXPORT TimeNode
{

public:

  using iterator = Container<TimeEvent>::iterator;
  using const_iterator = Container<TimeEvent>::const_iterator;

  /*! to be notified when it is triggered */
  using ExecutionCallback = std::function<void()>;

#if 0
# pragma mark -
# pragma mark Life cycle
#endif

  /*! factory
   \param #TimeNode::ExecutionCallback to be be notified when the #TimeNode is triggered
   \return std::shared_ptr<#TimeNode> */
  static std::shared_ptr<TimeNode> create(TimeNode::ExecutionCallback = nullptr);
  /*! destructor */
  virtual ~TimeNode();

#if 0
# pragma mark -
# pragma mark Execution
#endif

  /*! changes the callback in the #TimeNode
   \param #TimeNode::ExecutionCallback to be be notified when the #TimeNode is triggered
   \details this may be unsafe to do during execution */
  virtual void setCallback(TimeNode::ExecutionCallback) = 0;

  /*! evaluate all #TimeEvent's to make them to happen or to dispose them
   \return boolean true if the operation succeeded */
  virtual bool trigger() = 0;

#if 0
# pragma mark -
# pragma mark Accessors
#endif

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
  virtual TimeNode & setExpression(const std::shared_ptr<Expression> = ExpressionTrue()) = 0;

  /*! get the simultaneity margin
   \return #TimeValue the simultaneity margin */
  virtual TimeValue getSimultaneityMargin() const = 0;

  /*! set the simultaneity margin
   \todo remove setter and move the setting into constructor (?)
   \param #TimeValue the simultaneity margin
   \return #TimeNode the time node */
  virtual TimeNode & setSimultaneityMargin(TimeValue) = 0;

#if 0
#pragma mark -
#pragma mark TimeEvents
#endif

  /*! create and store a #TimeEvent
   \param #Container<#TimeEvent>::const_iterator where to store the #TimeEvent
   \param #TimeEvent::ExecutionCallback to get #TimeEvent's status back
   \param std::shared<#Expression> an optionnal #Expression to apply to the #TimeEvent
   \return std::shared_ptr<#TimeEvent> */
  virtual iterator emplace(const_iterator,
                           TimeEvent::ExecutionCallback,
                           std::shared_ptr<Expression> = ExpressionTrue()) = 0;

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
