/*!
 * \file TimeProcess.h
 *
 * \brief #TimeProcess is the mother class to describe temporal content ...
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
#include <functional>

namespace OSSIA
{

class Clock;
class State;
class TimeConstraint;
class TimeValue;

class TimeProcess
{

public:

# pragma mark -
# pragma mark Life cycle
  
  /*! destructor */
  virtual ~TimeProcess() = default;

# pragma mark -
# pragma mark Execution
  
  /*! execute and optionnaly log the execution into a file
   \param bool to enable log
   \param string to give a log file name where to write */
  virtual void play(bool log = false, std::string name = "") const = 0;
    
  /*! stop the execution */
  virtual void stop() const = 0;
  
  /*! pause the execution */
  virtual void pause() const = 0;
  
  /*! resume the execution */
  virtual void resume() const = 0;
  
  /*! to get the process execution back
   \param const #TimeValue process clock position
   \param const #TimeValue process clock date
   \param std::shared_ptr<#State> */
  using ExecutionCallback = std::function<void(const TimeValue&, const TimeValue&, std::shared_ptr<State>)>;
  
  /*! get the execution #State of the process
   \return std::shared_ptr<#State> */
  virtual std::shared_ptr<State> state() const = 0;
  
# pragma mark -
# pragma mark Accessors
  
  /*! get the start #State
   \return std::shared_ptr<#State> start state */
  virtual const std::shared_ptr<State> & getStartState() const = 0;
  
  /*! get the end #State
   \return std::shared_ptr<#State> end state */
  virtual const std::shared_ptr<State> & getEndState() const = 0;
  
  /*! get the #Clock
   \return std::shared_ptr<#Clock> clock */
  virtual const std::shared_ptr<Clock> & getClock() const = 0;
  
  /*! get the parent #TimeConstraint
   \return std::shared_ptr<#TimeConstraint> */
  virtual const std::shared_ptr<TimeConstraint> & getParentTimeConstraint() const = 0;
};
}
