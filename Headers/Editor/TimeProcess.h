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
  
  /*! get the time process length
   \return #TimeValue length */
  virtual TimeValue getLength() const = 0;
  
  /*! set the time process length
   \param #TimeValue length */
  virtual void setLength(TimeValue) = 0;
  
  /*! get the time process start event
   \return std::shared_ptr<#TimeEvent> start event */
  const std::shared_ptr<TimeEvent> & getStartEvent() const {return m_startEvent;};
  
  /*! get the time process end event
   \return std::shared_ptr<#TimeEvent> end event */
  const std::shared_ptr<TimeEvent> & getEndEvent() const {return m_endEvent;};
  
  /*! get the time process start state
   \return std::shared_ptr<#State> start state */
  const std::shared_ptr<State> & getStartState() const {return m_startState;};
  
  /*! set the time process start state
   \param std::shared_ptr<#State> start state */
  void setStartState(std::shared_ptr<State> startState) {m_startState = startState;};
  
  /*! get the time process end state
   \return std::shared_ptr<#State> end state */
  const std::shared_ptr<State> & getEndState() const {return m_endState;};
  
  /*! set the time process end state
   \param std::shared_ptr<#State> end state */
  void setEndState(std::shared_ptr<State> endState) {m_endState = endState;};
  
  /*! get the time process clock
   \todo
   \return std::shared_ptr<#Clock> clock */
  //const std::shared_ptr<Clock> & getClock() const {return m_clock;};
  
  /*! set the time process clock
   \todo
   \param std::shared_ptr<#Clock> clock */
  //void setClock(std::shared_ptr<Clock>) {m_clock = clock;};
};
}
