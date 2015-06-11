/*!
 * \file TimeProcess.h
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

class Clock;
class State;
class TimeEvent;
class TimeValue;

class TimeProcess
{

protected:
  
  std::shared_ptr<TimeEvent> m_startEvent;
  std::shared_ptr<TimeEvent> m_endEvent;
  
  std::shared_ptr<State>     m_startState;
  std::shared_ptr<State>     m_endState;
  
  std::shared_ptr<Clock>     m_clock;
  
public:

# pragma mark -
# pragma mark Life cycle
  
  /*! factory
   \return std::shared_ptr<#TimeProcess> */
  virtual ~TimeProcess() = default;

# pragma mark -
# pragma mark Execution
  
  /*! execute the time process and optionnaly log the execution into a file
   \param bool to enable log
   \param string to give a log file name where to write */
  virtual void play(bool log = false, std::string name = "") const = 0;

# pragma mark -
# pragma mark Accessors
  
  /*! get the time process start event
   \return std::shared_ptr<#TimeEvent> start event */
  virtual const std::shared_ptr<TimeEvent> & getStartEvent() const = 0;
  
  /*! get the time process end event
   \return std::shared_ptr<#TimeEvent> end event */
  virtual const std::shared_ptr<TimeEvent> & getEndEvent() const = 0;
  
  virtual TimeValue getLength() const = 0;
  
  virtual void setLength(TimeValue) = 0;
  
  virtual const std::shared_ptr<State> & getStartState() const = 0;
  
  virtual void setStartState(std::shared_ptr<State>) = 0;
  
  virtual const std::shared_ptr<State> & getEndState() const = 0;
  
  virtual void setEndState(std::shared_ptr<State>) = 0;
  
  virtual const std::shared_ptr<Clock> & getClock() const = 0;
  
  virtual void setClock(std::shared_ptr<Clock>) = 0;

  };
}
