/*!
 * \file #TimeEvent.h
 *
 * \defgroup Editor
 *
 * \brief #TimeEvent is use to describe temporal structure to launch the start or the end of each attached #TimeConstraint.
 *
 * \details #TimeEvent has a #State and can also be submitted to an #Expression.
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
#include <functional>

#include "Misc/Container.h"

namespace OSSIA
{

class Expression;
class State;
class TimeConstraint;
class TimeNode;
  
class TimeEvent
{

public:
  
# pragma mark -
# pragma mark Enumerations
  
  /*! event status */
  enum class Status
  {
    NONE,
    PENDING,
    HAPPENED,
    DISPOSED
  };
  
# pragma mark -
# pragma mark Life cycle
  
  /*! destructor */
  virtual ~TimeEvent() = default;

# pragma mark -
# pragma mark Execution
  
  /*! to get the event status back
   \param #Status new status */
  using ExecutionCallback = std::function<void(Status)>;
  
  /*! make the event happen to propagate the execution to next TimeConstraints
   \details the event have to be in WAITING or PENDING status to call this method otherwise it will raise a runtime_error
   \details turn the event' status into HAPPENED calling the callback to notify its owner */
  virtual void happen() = 0;
  
  /*! dispose the event to not propagate the execution to next TimeConstraints
   \details the event have to be in WAITING or PENDING status to call this method otherwise it will raise a runtime_error
   \details turn the event' status into DISPOSED  calling the callback to notify its owner */
  virtual void dispose() = 0;
  
# pragma mark -
# pragma mark Edition
  
  /*! add a sub state into the state of the event
   \param std::shared_ptr<#State> to add */
  virtual void addState(const std::shared_ptr<State>) = 0;
  
  /*! remove a sub state from the state of the event
   \param std::shared_ptr<#State> to remove */
  virtual void removeState(const std::shared_ptr<State>) = 0;

# pragma mark -
# pragma mark Accessors
  
  /*! get the #TimeNode where the event is
   \return std::shared_ptr<#TimeNode> */
  virtual const std::shared_ptr<TimeNode> & getTimeNode() const = 0;
  
  /*! get the state of the event
  \return std::shared_ptr<#State> */
  virtual const std::shared_ptr<State> & getState() const = 0;
  
  /*! get the expression of the event
  \return std::shared_ptr<#Expression> */
  virtual const std::shared_ptr<Expression> & getExpression() const = 0;
  
  /*! get the expression of the event
   \param std::shared_ptr<#Expression> 
   \return #TimeEvent the event */
  virtual TimeEvent & setExpression(const std::shared_ptr<Expression>) = 0;
  
  /*! get the status of the event
   \return #Status */
  virtual Status getStatus() const = 0;
  
# pragma mark -
# pragma mark Time Constraints
  
  /*! get previous time contraints attached to the event
   \return #Container<#TimeConstraint> */
  Container<TimeConstraint>& previousTimeConstraints()
  { return m_previousTimeConstraints; }
  
  /*! get previous time contraints attached to the event
   \return #Container<#TimeProcess> */
  const Container<TimeConstraint>& previousTimeConstraints() const
  { return m_previousTimeConstraints; }
  
  /*! get next time contraints attached to the event
   \return #Container<#TimeConstraint> */
  Container<TimeConstraint>& nextTimeConstraints()
  { return m_nextTimeConstraints; }
  
  /*! get next time contraints attached to the event
   \return #Container<#TimeProcess> */
  const Container<TimeConstraint>& nextTimeConstraints() const
  { return m_nextTimeConstraints; }
  
private:
  Container<TimeConstraint> m_previousTimeConstraints;
  Container<TimeConstraint> m_nextTimeConstraints;
};
}

