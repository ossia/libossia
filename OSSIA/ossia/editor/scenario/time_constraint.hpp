/*!
 * \file TimeConstraint.h
 *
 * \defgroup Editor
 *
 * \brief #TimeConstraint is use to describe temporal structure precising which amount of time (the duration) are between two #TimeEvents.
 *
 * \details The duration can be fixed or between a minimal and a maximal duration. \n
 * #TimeConstraint is also a #Clock and a #TimeProcess container.
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

#include <ossia/editor/scenario/clock.hpp>
#include <ossia/editor/scenario/time_value.hpp>
#include <ossia/detail/ptr_container.hpp>
#include <ossia_export.h>

namespace OSSIA
{

class State;
class TimeEvent;
class TimeProcess;
class TimeValue;

class OSSIA_EXPORT TimeConstraint : virtual Clock
{

public:
  using Clock::getSpeed;
  using Clock::setSpeed;
  using Clock::getDriveMode;
  using Clock::setDriveMode;
  using Clock::getGranularity;
  using Clock::setGranularity;
  using Clock::getDate;
  using Clock::getOffset;
  using Clock::getRunning;
  using Clock::getPosition;
  using Clock::tick;
  using Clock::pause;
  using Clock::paused;
  using Clock::resume;
  using Clock::setExecutionStatusCallback;
  using Clock::getExecutionStatusCallback;

  /*! to get the constraint execution back
   \param const #TimeValue process clock position
   \param const #TimeValue process clock date
   \param std::shared_ptr<#State> */
  using ExecutionCallback = std::function<void(TimeValue, TimeValue, const State&)>;

#if 0
# pragma mark -
# pragma mark Life cycle
#endif

  /*! factory
   \details by default a #TimeConstraint have an infinite duration with no minimal duration and an infinite maximal duration.
   \param #TimeConstraint::ExecutionCallback to use to be notified at each step
   \param #std::shared_ptr<TimeEvent> event where the #TimeConstraint starts
   \param #std::shared_ptr<TimeEvent> event where the #TimeConstraint ends
   \param const #TimeValue& nominal duration of the #TimeConstraint
   \param const #TimeValue& minimal duration of the #TimeConstraint
   \param const #TimeValue& maximal duration of the #TimeConstraint
   \return std::shared_ptr<#TimeConstraint> */
  static std::shared_ptr<TimeConstraint> create(TimeConstraint::ExecutionCallback,
                                                std::shared_ptr<TimeEvent>,
                                                std::shared_ptr<TimeEvent>,
                                                TimeValue = Infinite,
                                                TimeValue = 0.,
                                                TimeValue = Infinite);

  /*! desctructor */
  virtual ~TimeConstraint();

#if 0
# pragma mark -
# pragma mark Execution
#endif

  /*! start #TimeConstraint's #Clock */
  virtual void start() override = 0;

  /*! stop #TimeConstraint's #Clock */
  virtual void stop() override = 0;

  /*! set #TimeConstraint's #Clock offset and process a state at offset date
   \details the returned #State is made of as many as sub States for each TimeProcess the #TimeConstraint manages
   \details don't call offset when the #TimeConstraint is running
   \param const #TimeValue offset date
   \return std::shared_ptr<#State> */
  virtual State offset(TimeValue) = 0;

  /*! get a #State from the constraint depending on its #Clock date
   \details the returned #State is made of as many as sub States for each TimeProcess the #TimeConstraint manages
   \details don't call state when the #TimeConstraint is not running
   \return std::shared_ptr<#State> */
  virtual State state() = 0;

#if 0
# pragma mark -
# pragma mark Accessors
#endif

  /*! sets a new callback for the constraint
    \param #TimeConstraint::ExecutionCallback to use to be notified at each step */
  virtual void setCallback(ExecutionCallback) = 0;

  /*! get the #TimeConstraint nominal duration
   \return const #TimeValue& nominal duration */
  virtual const TimeValue & getDurationNominal() const = 0;

  /*! set the #TimeConstraint duration
   \param const #TimeValue& duration
   \return #TimeConstraint the constraint */
  virtual TimeConstraint & setDurationNominal(TimeValue) = 0;

  /*! get the #TimeConstraint minimal duration
   \return const #TimeValue& minimal duration */
  virtual const TimeValue & getDurationMin() const = 0;

  /*! set the #TimeConstraint minimal duration
   \param const #TimeValue& minimal duration
   \return #TimeConstraint the constraint */
  virtual TimeConstraint & setDurationMin(TimeValue) = 0;

  /*! get the #TimeConstraint maximal duration
   \return const #TimeValue& maximal duration */
  virtual const TimeValue & getDurationMax() const = 0;

  /*! set the #TimeConstraint maximal duration
   \param const #TimeValue& maximal duration
   \return #TimeConstraint the constraint */
  virtual TimeConstraint & setDurationMax(TimeValue) = 0;

  /*! get the event from where the #TimeConstraint starts
   \return std::shared_ptr<#TimeEvent> start event */
  virtual const std::shared_ptr<TimeEvent> & getStartEvent() const = 0;

  /*! get the event from where the #TimeConstraint starts
   \return std::shared_ptr<#TimeEvent> start event */
  virtual const std::shared_ptr<TimeEvent> & getEndEvent() const = 0;

#if 0
# pragma mark -
# pragma mark TimeProcesses
#endif

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

