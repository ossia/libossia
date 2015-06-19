/*!
 * \file TimeConstraint.h
 *
 * \brief #TimeConstraint is use to describe temporal structure precising which amount of time (the duration) are between two TimeEvents.
 *
 * \details The duration can be fixed or between a minimal and a maximal duration. \n
 * TimeConstraint is also a #TimeProcess container.
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

#include "Editor/TimeValue.h"
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
   \details by default a #TimeConstraint have an infinite duration with no minimal duration and an infinite maximal duration.
   \param #std::shared_ptr<TimeEvent> event where the constraint starts
   \param #std::shared_ptr<TimeEvent> event where the constraint ends
   \param const #TimeValue& duration of the constraint
   \param const #TimeValue& minimal duration of the constraint
   \param const #TimeValue& maximal duration of the constraint
   \return std::shared_ptr<#TimeConstraint> */
  static std::shared_ptr<TimeConstraint> create(std::shared_ptr<TimeEvent>,
                                                std::shared_ptr<TimeEvent>,
                                                const TimeValue& = Infinite,
                                                const TimeValue& = 0.,
                                                const TimeValue& = Infinite);
  
  /*! clone 
   \return std::shared_ptr<#TimeConstraint> */
  virtual std::shared_ptr<TimeConstraint> clone() const = 0;
  
  /*! desctructor */
  virtual ~TimeConstraint() = default;

# pragma mark -
# pragma mark Execution

  /*! execute and optionnaly log the execution into a file
   \param bool to enable log
   \param std::string to give a log file name where to write */
  virtual void play(bool log = false, std::string name = "") const = 0;

# pragma mark -
# pragma mark Accessors
  
  /*! get the time constraint duration
   \return const #TimeValue& duration */
  virtual const TimeValue & getDuration() const = 0;
  
  /*! get the time constraint minimal duration
   \return const #TimeValue& minimal duration */
  virtual const TimeValue & getDurationMin() const = 0;
  
  /*! get the time constraint maximal duration
   \return const #TimeValue& maximal duration */
  virtual const TimeValue & getDurationMax() const = 0;
  
  /*! get the event from where the constraint starts
   \return std::shared_ptr<#TimeEvent> start event */
  virtual const std::shared_ptr<TimeEvent> & getStartEvent() const = 0;
  
  /*! get the event from where the constraint starts
   \return std::shared_ptr<#TimeEvent> start event */
  virtual const std::shared_ptr<TimeEvent> & getEndEvent() const = 0;

# pragma mark -
# pragma mark Time Processes
  
  /*! get time processes attached to the time constraint
   \return #Container<#TimeProcess> container */
  Container<TimeProcess>& timeProcesses()
  { return m_timeProcesses; }
  
  /*! get time processes attached to the time constraint
   \return #Container<#TimeProcess> container */
  const Container<TimeProcess>& timeProcesses() const
  { return m_timeProcesses; }

private:
  Container<TimeProcess> m_timeProcesses;
};

}

