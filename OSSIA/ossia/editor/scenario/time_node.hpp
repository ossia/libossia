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

namespace ossia
{

class expression_base;
class State;
class time_event;
class time_value;

class OSSIA_EXPORT time_node
{

public:

  using iterator = ptr_container<time_event>::iterator;
  using const_iterator = ptr_container<time_event>::const_iterator;

  /*! to be notified when it is triggered */
  using execution_callback = std::function<void()>;

#if 0
# pragma mark -
# pragma mark Life cycle
#endif

  /*! factory
   \param #TimeNode::ExecutionCallback to be be notified when the #TimeNode is triggered
   \return std::shared_ptr<#TimeNode> */
  static std::shared_ptr<time_node> create(time_node::execution_callback = {});
  /*! destructor */
  virtual ~time_node();

#if 0
# pragma mark -
# pragma mark Execution
#endif

  /*! changes the callback in the #TimeNode
   \param #TimeNode::ExecutionCallback to be be notified when the #TimeNode is triggered
   \details this may be unsafe to do during execution */
  virtual void setCallback(time_node::execution_callback) = 0;

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
  virtual time_value getDate() const = 0;

  /*! get the expression of the #TimeNode
   \return std::shared_ptr<#Expression> */
  virtual const expression& getExpression() const = 0;

  /*! set the expression of the #TimeNode
   \details setting the expression to ExpressionTrue will defer the evaluation on #TimeEvent's expression
   \details setting the expression to ExpressionFalse will mute TimeNode execution
   \param std::shared_ptr<#Expression>
   \return #TimeNode the time node */
  virtual time_node & setExpression(expression_ptr) = 0;

  /*! get the simultaneity margin
   \return #TimeValue the simultaneity margin */
  virtual time_value getSimultaneityMargin() const = 0;

  /*! set the simultaneity margin
   \todo remove setter and move the setting into constructor (?)
   \param #TimeValue the simultaneity margin
   \return #TimeNode the time node */
  virtual time_node & setSimultaneityMargin(time_value) = 0;

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
                            time_event::ExecutionCallback,
                            expression_ptr/* = ExpressionTrue()*/) = 0;

  /*! get the #TimeEvents of the #TimeNode
   \return #Container<#TimeEvent> */
  ptr_container<time_event>& timeEvents()
  { return m_timeEvents; }

  /*! get the #TimeEvents of the #TimeNode
   \return #Container<#TimeEvent> */
  const ptr_container<time_event>& timeEvents() const
  { return m_timeEvents; }

private:
  ptr_container<time_event> m_timeEvents;
};

}
