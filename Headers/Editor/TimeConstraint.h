/*!
 * \file TimeConstraint.h
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

class TimeEvent;
class TimeProcess;
class TimeValue;

class TimeConstraint
{
  
public:
  
# pragma mark -
# pragma mark Life cycle

  /*! factory
   \details if the 3 duration are equals it means ...
   \param #TimeValue duration of the constraint
   \param #std::shared_ptr<TimeEvent> event where the constraint starts
   \param #std::shared_ptr<TimeEvent> event where the constraint ends
   \param #TimeValue minimal duration of the constraint
   \param #TimeValue maximal duration of the constraint
   \return std::shared_ptr<#TimeConstraint> */
  static std::shared_ptr<TimeConstraint> create(TimeValue nominal,
                                                std::shared_ptr<TimeEvent>,
                                                std::shared_ptr<TimeEvent>,
                                                TimeValue,
                                                TimeValue);
  
  /*! clone 
   \return std::shared_ptr<#TimeConstraint> */
  virtual std::shared_ptr<TimeConstraint> clone() const = 0;
  
  /*! desctructor */
  virtual ~TimeConstraint() = default;

# pragma mark -
# pragma mark Execution

  /*! execute the scenario and optionnaly log the execution into a file
   \param bool to enable log
   \param std::string to give a log file name where to write */
  virtual void play(bool log = false, std::string name = "") const = 0;

# pragma mark -
# pragma mark Accessors
  
  /*! get the time constraint duration
   \return #TimeValue duration */
  virtual TimeValue getDuration() const = 0;
  
  /*! get the time constraint minimal duration
   \return #TimeValue minimal duration */
  virtual TimeValue getDurationMin() const = 0;
  
  /*! get the time constraint maximal duration
   \return #TimeValue maximal duration */
  virtual TimeValue getDurationMax() const = 0;
  
  /*! get the event from where the constraint starts
   \return std::shared_ptr<#TimeEvent> start event */
  virtual const std::shared_ptr<TimeEvent> & getStartEvent() const = 0;
  
  /*! get the event from where the constraint starts
   \return std::shared_ptr<#TimeEvent> start event */
  virtual const std::shared_ptr<TimeEvent> & getEndEvent() const = 0;

# pragma mark -
# pragma mark Time Processes
  
  /*! get processes attached to the constraint
   \return #Container<#TimeProcess> container */
  Container<TimeProcess>& timeProcesses()
  { return m_timeProcesses; }
  
  /*! get processes attached to the constraint
   \return #Container<#TimeProcess> container */
  const Container<TimeProcess>& timeProcesses() const
  { return m_timeProcesses; }

private:
  Container<TimeProcess> m_timeProcesses;
};

}

