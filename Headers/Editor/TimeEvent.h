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
# pragma mark Life cycle
  
  /*! destructor */
  virtual ~TimeEvent() = default;

# pragma mark -
# pragma mark Execution
  
  /*! execute the scenario and optionnaly log the execution into a file
  \param bool to enable log
  \param string to give a log file name where to write */
  virtual void play(bool log = false, std::string name = "") const = 0;

# pragma mark -
# pragma mark Accessors
  
  /*! get the time node where the event is
   \return std::shared_ptr<#TimeNode> time node */
  virtual const std::shared_ptr<TimeNode> & getTimeNode() const = 0;
  
  /*! get the state of the event
  \return std::shared_ptr<#State> state */
  virtual const std::shared_ptr<State> & getState() const = 0;
  
  /*! get the expression of the event
  \return std::shared_ptr<#Expression> expression */
  virtual const std::shared_ptr<Expression> & getExpression() const = 0;
  
# pragma mark -
# pragma mark Time Constraints
  
  /*! get previous time contraints attached to the event
   \return #Container<#TimeConstraint> container */
  Container<TimeConstraint>& previousTimeConstraints()
  { return m_previousTimeConstraints; }
  
  /*! get previous time contraints attached to the event
   \return #Container<#TimeProcess> container */
  const Container<TimeConstraint>& previousTimeConstraints() const
  { return m_previousTimeConstraints; }
  
  /*! get next time contraints attached to the event
   \return #Container<#TimeConstraint> container */
  Container<TimeConstraint>& nextTimeConstraints()
  { return m_nextTimeConstraints; }
  
  /*! get next time contraints attached to the event
   \return #Container<#TimeProcess> container */
  const Container<TimeConstraint>& nextTimeConstraints() const
  { return m_nextTimeConstraints; }
  
private:
  Container<TimeConstraint> m_previousTimeConstraints;
  Container<TimeConstraint> m_nextTimeConstraints;
};
}

