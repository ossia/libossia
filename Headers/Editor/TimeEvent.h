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

namespace OSSIA
{

class Expression;
class State;
  
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
# pragma mark Edition
  
  /*! add a sub state into the state of the event
   \param std::shared_ptr<#State> to add */
  virtual void addState(const std::shared_ptr<State>) = 0;
  
  /*! remove a sub state from the state of the event
   \param std::shared_ptr<#State> to remove */
  virtual void removeState(const std::shared_ptr<State>) = 0;

# pragma mark -
# pragma mark Accessors
  
  /*! get the state of the event
  \return std::shared_ptr<#State> state */
  virtual const std::shared_ptr<State> & getState() const = 0;
  
  /*! get the expression of the event
  \return std::shared_ptr<#Expression> expression */
  virtual const std::shared_ptr<Expression> & getExpression() const = 0;
};
}

