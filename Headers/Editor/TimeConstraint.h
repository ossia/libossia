/*!
 * \file TimeConstraint.h
 *
 * \brief #TimeConstraint is use to describe temporal structure precising which amount of time (the duration) are between two #TimeEvents.
 *
 * \details The duration can be fixed or between a minimal and a maximal duration. \n
 * #TimeConstraint is also a #TimeProcess container.
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

class State;
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
   \param #std::shared_ptr<TimeEvent> event where the #TimeConstraint starts
   \param #std::shared_ptr<TimeEvent> event where the #TimeConstraint ends
   \param const #TimeValue& duration of the #TimeConstraint
   \param const #TimeValue& minimal duration of the #TimeConstraint
   \param const #TimeValue& maximal duration of the #TimeConstraint
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
  
  /*! get the #State at a position or a date
   \param const TimeValue& time position
   \param const TimeValue& date which is infinite by default
   \return std::shared_ptr<State> */
  virtual std::shared_ptr<State> state(const TimeValue&, const TimeValue& = Infinite) const = 0;

# pragma mark -
# pragma mark Accessors
  
  /*! get the #TimeConstraint duration
   \return const #TimeValue& duration */
  virtual const TimeValue & getDuration() const = 0;
  
  /*! set the #TimeConstraint duration
   \param const #TimeValue& duration */
  virtual TimeConstraint & setDuration(const TimeValue&) = 0;
  
  /*! get the #TimeConstraint minimal duration
   \return const #TimeValue& minimal duration */
  virtual const TimeValue & getDurationMin() const = 0;
  
  /*! set the #TimeConstraint minimal duration
   \param const #TimeValue& minimal duration */
  virtual TimeConstraint & setDurationMin(const TimeValue&) = 0;
  
  /*! get the #TimeConstraint maximal duration
   \return const #TimeValue& maximal duration */
  virtual const TimeValue & getDurationMax() const = 0;
  
  /*! set the #TimeConstraint maximal duration
   \param const #TimeValue& maximal duration */
  virtual TimeConstraint & setDurationMax(const TimeValue&) = 0;
  
  /*! get the event from where the #TimeConstraint starts
   \return std::shared_ptr<#TimeEvent> start event */
  virtual const std::shared_ptr<TimeEvent> & getStartEvent() const = 0;
  
  /*! get the event from where the #TimeConstraint starts
   \return std::shared_ptr<#TimeEvent> start event */
  virtual const std::shared_ptr<TimeEvent> & getEndEvent() const = 0;

# pragma mark -
# pragma mark TimeProcesses
  
  /*! add a #TimeProcess
   \details it also stores the #TimeProcess's start and end #States into the #TimeConstraint's start and end #TimeEvents
   \param std::shared_ptr<#TimeProcess> to insert */
  virtual void addTimeProcess(std::shared_ptr<TimeProcess>) = 0;
  
  /*! remove a #TimeProcess
   \details it also removes the #TimeProcess's start and end #States from the #TimeConstraint's start and end #TimeEvents
   \param std::shared_ptr<#TimeProcess> to insert */
  virtual void removeTimeProcess(std::shared_ptr<TimeProcess>) = 0;
  
  /*! get time processes attached to the #TimeConstraint
   \return #Container<#TimeProcess> */
  Container<TimeProcess>& timeProcesses()
  { return m_timeProcesses; }
  
  /*! get time processes attached to the #TimeConstraint
   \return #Container<#TimeProcess> */
  const Container<TimeProcess>& timeProcesses() const
  { return m_timeProcesses; }

private:
  Container<TimeProcess> m_timeProcesses;
};

}

