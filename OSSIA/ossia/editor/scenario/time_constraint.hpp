#pragma once

#include <functional>
#include <memory>
#include <string>

#include <ossia/detail/ptr_container.hpp>
#include <ossia/editor/scenario/clock.hpp>
#include <ossia/editor/scenario/time_value.hpp>
#include <ossia_export.h>

namespace ossia
{

class state;
class time_event;
class time_process;
class time_value;

/**
 * @brief The time_constraint class
 *
 * \brief #TimeConstraint is use to describe temporal structure precising which
 * amount of time (the duration) are between two #TimeEvents.
 *
 * \details The duration can be fixed or between a minimal and a maximal
 * duration. \n
 * #TimeConstraint is also a #Clock and a #TimeProcess container.
 */
class OSSIA_EXPORT time_constraint : virtual clock
{

public:
  using clock::getSpeed;
  using clock::setSpeed;
  using clock::getDriveMode;
  using clock::setDriveMode;
  using clock::getGranularity;
  using clock::setGranularity;
  using clock::getDate;
  using clock::getOffset;
  using clock::getRunning;
  using clock::getPosition;
  using clock::tick;
  using clock::pause;
  using clock::paused;
  using clock::resume;
  using clock::setExecutionStatusCallback;
  using clock::getExecutionStatusCallback;

  /*! to get the constraint execution back
   \param const #TimeValue process clock position
   \param const #TimeValue process clock date
   \param std::shared_ptr<#State> */
  using ExecutionCallback
      = std::function<void(time_value, time_value, const state&)>;

  /*! factory
   \details by default a #TimeConstraint have an infinite duration with no
   minimal duration and an infinite maximal duration.
   \param #TimeConstraint::ExecutionCallback to use to be notified at each step
   \param #std::shared_ptr<TimeEvent> event where the #TimeConstraint starts
   \param #std::shared_ptr<TimeEvent> event where the #TimeConstraint ends
   \param const #TimeValue& nominal duration of the #TimeConstraint
   \param const #TimeValue& minimal duration of the #TimeConstraint
   \param const #TimeValue& maximal duration of the #TimeConstraint
   \return std::shared_ptr<#TimeConstraint> */
  static std::shared_ptr<time_constraint> create(
      time_constraint::ExecutionCallback, std::shared_ptr<time_event>,
      std::shared_ptr<time_event>, time_value = Infinite, time_value = 0.,
      time_value = Infinite);

  /*! desctructor */
  virtual ~time_constraint();

  /*! start #TimeConstraint's #Clock */
  virtual void start() override = 0;

  /*! stop #TimeConstraint's #Clock */
  virtual void stop() override = 0;

  /*! set #TimeConstraint's #Clock offset and process a state at offset date
   \details the returned #State is made of as many as sub States for each
   TimeProcess the #TimeConstraint manages
   \details don't call offset when the #TimeConstraint is running
   \param const #TimeValue offset date
   \return std::shared_ptr<#State> */
  virtual ossia::state offset(time_value) = 0;

  /*! get a #State from the constraint depending on its #Clock date
   \details the returned #State is made of as many as sub States for each
   TimeProcess the #TimeConstraint manages
   \details don't call state when the #TimeConstraint is not running
   \return std::shared_ptr<#State> */
  virtual ossia::state state() = 0;

  /*! sets a new callback for the constraint
    \param #TimeConstraint::ExecutionCallback to use to be notified at each
    step
    */
  virtual void setCallback(ExecutionCallback) = 0;

  /*! get the #TimeConstraint nominal duration
   \return const #TimeValue& nominal duration */
  virtual const time_value& getDurationNominal() const = 0;

  /*! set the #TimeConstraint duration
   \param const #TimeValue& duration
   \return #TimeConstraint the constraint */
  virtual time_constraint& setDurationNominal(time_value) = 0;

  /*! get the #TimeConstraint minimal duration
   \return const #TimeValue& minimal duration */
  virtual const time_value& getDurationMin() const = 0;

  /*! set the #TimeConstraint minimal duration
   \param const #TimeValue& minimal duration
   \return #TimeConstraint the constraint */
  virtual time_constraint& setDurationMin(time_value) = 0;

  /*! get the #TimeConstraint maximal duration
   \return const #TimeValue& maximal duration */
  virtual const time_value& getDurationMax() const = 0;

  /*! set the #TimeConstraint maximal duration
   \param const #TimeValue& maximal duration
   \return #TimeConstraint the constraint */
  virtual time_constraint& setDurationMax(time_value) = 0;

  /*! get the event from where the #TimeConstraint starts
   \return std::shared_ptr<#TimeEvent> start event */
  virtual const std::shared_ptr<time_event>& getStartEvent() const = 0;

  /*! get the event from where the #TimeConstraint starts
   \return std::shared_ptr<#TimeEvent> start event */
  virtual const std::shared_ptr<time_event>& getEndEvent() const = 0;

  /*! add a #TimeProcess
   \details it also stores the #TimeProcess's start and end #States into the
   #TimeConstraint's start and end #TimeEvents
   \param std::shared_ptr<#TimeProcess> to insert */
  virtual void addTimeProcess(std::shared_ptr<time_process>) = 0;

  /*! remove a #TimeProcess
   \details it also removes the #TimeProcess's start and end #States from the
   #TimeConstraint's start and end #TimeEvents
   \param std::shared_ptr<#TimeProcess> to insert */
  virtual void removeTimeProcess(std::shared_ptr<time_process>) = 0;

  /*! get time processes attached to the #TimeConstraint
   \return #Container<#TimeProcess> */
  ptr_container<time_process>& timeProcesses()
  {
    return m_timeProcesses;
  }

  /*! get time processes attached to the #TimeConstraint
   \return #Container<#TimeProcess> */
  const ptr_container<time_process>& timeProcesses() const
  {
    return m_timeProcesses;
  }

private:
  ptr_container<time_process> m_timeProcesses;
};
}
