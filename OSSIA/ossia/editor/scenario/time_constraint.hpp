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
struct time_value;

/**
 * @brief The time_constraint class
 *
 * \brief #time_constraint is use to describe temporal structure precising which
 * amount of time (the duration) are between two #time_events.
 *
 * \details The duration can be fixed or between a minimal and a maximal
 * duration. \n
 * #time_constraint is also a #Clock and a #TimeProcess container.
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
   \details by default a #time_constraint has an infinite duration with no
   minimal duration and an infinite maximal duration.
   \param #time_constraint::ExecutionCallback to use to be notified at each step
   \param #std::shared_ptr<TimeEvent> event where the #time_constraint starts
   \param #std::shared_ptr<TimeEvent> event where the #time_constraint ends
   \param const #TimeValue& nominal duration of the #time_constraint
   \param const #TimeValue& minimal duration of the #time_constraint
   \param const #TimeValue& maximal duration of the #time_constraint
   \return std::shared_ptr<#time_constraint> */
   static std::shared_ptr<time_constraint> create(
      time_constraint::ExecutionCallback, time_event&,
      time_event&, time_value = Infinite, time_value = Zero,
      time_value = Infinite);

   time_constraint(
       time_constraint::ExecutionCallback, time_event&,
       time_event&, time_value = Infinite, time_value = Zero,
       time_value = Infinite);

   time_constraint(
       time_constraint::ExecutionCallback, std::unique_ptr<clock>, time_event&,
       time_event&, time_value = Infinite, time_value = Zero,
       time_value = Infinite);

  /*! desctructor */
  ~time_constraint();

  /*! start #time_constraint's #Clock */
  void start();

  /*! stop #time_constraint's #Clock */
  void stop();

  /*! start #time_constraint's #Clock */
  void pause();

  /*! stop #time_constraint's #Clock */
  void resume();

  /*! set #time_constraint's #Clock offset and process a state at offset date
   \details the returned #State is made of as many as sub States for each
   TimeProcess the #time_constraint manages
   \details don't call offset when the #time_constraint is running
   \param const #TimeValue offset date
   \return std::shared_ptr<#State> */
  ossia::state offset(time_value);

  /*! get a #State from the constraint depending on its #Clock date
   \details the returned #State is made of as many as sub States for each
   TimeProcess the #time_constraint manages
   \details don't call state when the #time_constraint is not running
   \return std::shared_ptr<#State> */
  ossia::state state();

  /*! sets a new callback for the constraint
    \param #time_constraint::ExecutionCallback to use to be notified at each
    step
    */
  void setCallback(ExecutionCallback);

  //! This callback won't compute the state.
  void setStatelessCallback(ExecutionCallback);

  /*! get the #time_constraint nominal duration
   \return const #TimeValue& nominal duration */
  const time_value& getDurationNominal() const;

  /*! set the #time_constraint duration
   \param const #TimeValue& duration
   \return #time_constraint the constraint */
  time_constraint& setDurationNominal(time_value);

  /*! get the #time_constraint minimal duration
   \return const #TimeValue& minimal duration */
  const time_value& getDurationMin() const;

  /*! set the #time_constraint minimal duration
   \param const #TimeValue& minimal duration
   \return #time_constraint the constraint */
  time_constraint& setDurationMin(time_value);

  /*! get the #time_constraint maximal duration
   \return const #TimeValue& maximal duration */
  const time_value& getDurationMax() const;

  /*! set the #time_constraint maximal duration
   \param const #TimeValue& maximal duration
   \return #time_constraint the constraint */
  time_constraint& setDurationMax(time_value);

  /*! get the event from where the #time_constraint starts
   \return std::shared_ptr<#time_event> start event */
  time_event& getStartEvent() const;

  /*! get the event from where the #time_constraint starts
   \return std::shared_ptr<#time_event> start event */
  time_event& getEndEvent() const;

  /*! add a #TimeProcess
   \details it also stores the #TimeProcess's start and end #States into the
   #time_constraint's start and end #time_events
   \param std::shared_ptr<#TimeProcess> to insert */
  void addTimeProcess(std::shared_ptr<time_process>);

  /*! remove a #TimeProcess
   \details it also removes the #TimeProcess's start and end #States from the
   #time_constraint's start and end #time_events
   \param std::shared_ptr<#TimeProcess> to insert */
  void removeTimeProcess(time_process*);

  /*! get time processes attached to the #time_constraint
   \return #Container<#TimeProcess> */
  const std::vector<std::shared_ptr<time_process>>& timeProcesses() const
  {
    return mTimeProcesses;
  }

private:
  clock::ExecutionCallback make_callback();
  clock::ExecutionCallback make_stateless_callback();
  ossia::state state_impl();
  ossia::state make_state();

  std::vector<std::shared_ptr<time_process>> mTimeProcesses;
  std::unique_ptr<clock> mClock;
  time_constraint::ExecutionCallback mCallback;

  time_event& mStartEvent;
  time_event& mEndEvent;

  time_value mDurationNominal{};
  time_value mDurationMin{};
  time_value mDurationMax{};
};
}
