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
class OSSIA_EXPORT time_constraint
{

public:
  auto getRunning() const { return mClock->getRunning(); }
  auto getDate() const { return mClock->getDate(); }
  auto getPosition() const { return mClock->getPosition(); }
  auto getDriveMode() const { return mClock->getDriveMode(); }
  auto getGranularity() const { return mClock->getGranularity(); }
  auto getOffset() const { return mClock->getOffset(); }
  auto getSpeed() const { return mClock->getSpeed(); }
  auto getExecutionStatusCallback() const { return mClock->getExecutionStatusCallback(); }
  auto paused() const { return mClock->paused(); }
  void setOffset(time_value g) const { mClock->setOffset(g); }
  void setDriveMode(clock::DriveMode m) const { mClock->setDriveMode(m); }
  void setGranularity(time_value g) const { mClock->setGranularity(g); }
  void setSpeed(double g) const { mClock->setSpeed(g); }
  bool tick() { return mClock->tick(); }
  bool tick(time_value usec) { return mClock->tick(usec); }
  void setExecutionStatusCallback(clock::ExecutionStatusCallback c) { mClock->setExecutionStatusCallback(c); }

  clock& getClock() { return *mClock; }
  /*! to get the constraint execution back
   \param const #TimeValue process clock position
   \param const #TimeValue process clock date
   \param std::shared_ptr<#State> */
  using ExecutionCallback
      = std::function<void(time_value, time_value, const state&)>;

  /*! constructor
   \details by default a #TimeConstraint has an infinite duration with no
   minimal duration and an infinite maximal duration.
   \param #TimeConstraint::ExecutionCallback to use to be notified at each step
   \param #std::shared_ptr<TimeEvent> event where the #TimeConstraint starts
   \param #std::shared_ptr<TimeEvent> event where the #TimeConstraint ends
   \param const #TimeValue& nominal duration of the #TimeConstraint
   \param const #TimeValue& minimal duration of the #TimeConstraint
   \param const #TimeValue& maximal duration of the #TimeConstraint
   \return std::shared_ptr<#TimeConstraint> */
   static std::shared_ptr<time_constraint> create(
      time_constraint::ExecutionCallback, time_event&,
      time_event&, time_value = Infinite, time_value = Zero,
      time_value = Infinite);

   time_constraint(
       time_constraint::ExecutionCallback, time_event&,
       time_event&, time_value = Infinite, time_value = Zero,
       time_value = Infinite);

  /*! desctructor */
  ~time_constraint();

  /*! start #TimeConstraint's #Clock */
  void start();

  /*! stop #TimeConstraint's #Clock */
  void stop();

  /*! start #TimeConstraint's #Clock */
  void pause();

  /*! stop #TimeConstraint's #Clock */
  void resume();

  /*! set #TimeConstraint's #Clock offset and process a state at offset date
   \details the returned #State is made of as many as sub States for each
   TimeProcess the #TimeConstraint manages
   \details don't call offset when the #TimeConstraint is running
   \param const #TimeValue offset date
   \return std::shared_ptr<#State> */
  ossia::state offset(time_value);

  /*! get a #State from the constraint depending on its #Clock date
   \details the returned #State is made of as many as sub States for each
   TimeProcess the #TimeConstraint manages
   \details don't call state when the #TimeConstraint is not running
   \return std::shared_ptr<#State> */
  ossia::state state();

  /*! sets a new callback for the constraint
    \param #TimeConstraint::ExecutionCallback to use to be notified at each
    step
    */
  void setCallback(ExecutionCallback);

  /*! get the #TimeConstraint nominal duration
   \return const #TimeValue& nominal duration */
  const time_value& getDurationNominal() const;

  /*! set the #TimeConstraint duration
   \param const #TimeValue& duration
   \return #TimeConstraint the constraint */
  time_constraint& setDurationNominal(time_value);

  /*! get the #TimeConstraint minimal duration
   \return const #TimeValue& minimal duration */
  const time_value& getDurationMin() const;

  /*! set the #TimeConstraint minimal duration
   \param const #TimeValue& minimal duration
   \return #TimeConstraint the constraint */
  time_constraint& setDurationMin(time_value);

  /*! get the #TimeConstraint maximal duration
   \return const #TimeValue& maximal duration */
  const time_value& getDurationMax() const;

  /*! set the #TimeConstraint maximal duration
   \param const #TimeValue& maximal duration
   \return #TimeConstraint the constraint */
  time_constraint& setDurationMax(time_value);

  /*! get the event from where the #TimeConstraint starts
   \return std::shared_ptr<#TimeEvent> start event */
  time_event& getStartEvent() const;

  /*! get the event from where the #TimeConstraint starts
   \return std::shared_ptr<#TimeEvent> start event */
  time_event& getEndEvent() const;

  /*! add a #TimeProcess
   \details it also stores the #TimeProcess's start and end #States into the
   #TimeConstraint's start and end #TimeEvents
   \param std::shared_ptr<#TimeProcess> to insert */
  void addTimeProcess(std::unique_ptr<time_process>);

  /*! remove a #TimeProcess
   \details it also removes the #TimeProcess's start and end #States from the
   #TimeConstraint's start and end #TimeEvents
   \param std::shared_ptr<#TimeProcess> to insert */
  void removeTimeProcess(time_process*);

  /*! get time processes attached to the #TimeConstraint
   \return #Container<#TimeProcess> */
  const std::vector<std::unique_ptr<time_process>>& timeProcesses() const
  {
    return mTimeProcesses;
  }

private:
  ossia::state state_impl();
  void ClockCallback(
      time_value position, time_value date, unsigned char droppedTicks);
  ossia::state make_state();

  std::vector<std::unique_ptr<time_process>> mTimeProcesses;
  std::unique_ptr<clock> mClock;
  time_constraint::ExecutionCallback mCallback;

  time_event& mStartEvent;
  time_event& mEndEvent;

  time_value mDurationNominal{};
  time_value mDurationMin{};
  time_value mDurationMax{};
};
}
