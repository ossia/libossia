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

namespace OSSIA
{

class Clock;
class State;
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
  
  /*! get the #State at a position or a date
   \param const TimeValue& position [0::1] or date to state
   \param const TimeValue& true if the first parameter is a date
   \return std::shared_ptr<State> */
  virtual std::shared_ptr<State> state(const TimeValue&, const TimeValue& = Infinite) const = 0;

# pragma mark -
# pragma mark Accessors
  
  /*! get the time process start state
   \return std::shared_ptr<#State> start state */
  virtual const std::shared_ptr<State> & getStartState() const = 0;
  
  /*! get the time process end state
   \return std::shared_ptr<#State> end state */
  virtual const std::shared_ptr<State> & getEndState() const = 0;
  
  /*! get the time process clock
   \return std::shared_ptr<#Clock> clock */
  virtual const std::shared_ptr<Clock> & getClock() const = 0;
};
}
