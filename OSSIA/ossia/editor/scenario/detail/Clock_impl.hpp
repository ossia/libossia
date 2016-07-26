/*!
 * \file Clock_impl.h
 *
 * \brief
 *
 * \details
 *
 * \author Théo de la Hogue
 *
 * \copyright This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#pragma once

#include <ctime>
#include <ratio>
#include <chrono>
#include <thread>
#include <mutex>
#include <cmath>

#include <ossia/editor/scenario/detail/Clock.hpp>

using namespace OSSIA;

using namespace std::chrono;

namespace impl
{
class JamomaClock : public virtual Clock
{

protected:
#if 0
# pragma mark -
# pragma mark Implementation specific
#endif

  TimeValue         mDuration{};        /// the time (in ms) the clock will run at normal speed factor
  TimeValue         mGranularity{};     /// the minimum time between each tick (in ms)
  TimeValue         mOffset{};          /// the date (in ms) the clock will run from
  double            mSpeed{};           /// the speed factor of the clock

  Clock::DriveMode  mDriveMode{};       /// in EXTERNAL drive mode the tick() method is called from outside

  bool              mRunning{};         /// is the clock running right now ?
  bool              mPaused{};          /// is the clock paused right now ?
  TimeValue         mPosition{};        /// the progression of the clock between the beginning and the end [0. :: 1.]
  TimeValue         mDate{};            /// how many time the clock is running (without no speed factor consideration)

  std::thread       mThread;            /// a thread to launch the clock execution

  steady_clock::time_point mLastTime{}; /// a time reference used to compute time tick
  long long         mElapsedTime{};     /// a time reference used to know how many time are elapsed in microsecond

private:
  ExecutionCallback mCallback;          /// the callback to use for each step
  ExecutionStatusCallback   mStatusCallback;

public:
#if 0
# pragma mark -
# pragma mark Life cycle
#endif

  JamomaClock(Clock::ExecutionCallback,
              TimeValue = Infinite,
              TimeValue = 1.,
              TimeValue = 0.,
              float = 1.,
              Clock::DriveMode = Clock::DriveMode::INTERNAL);

  JamomaClock(const JamomaClock *);

  ~JamomaClock();
#if 0
# pragma mark -
# pragma mark Execution
#endif

  void start() override;

  void stop() override;

  void pause() override;

  bool paused() const final override;

  void resume() override;

  bool tick() final override;
  bool tick(TimeValue usec) final override;
#if 0
# pragma mark -
# pragma mark Accessors
#endif

  const TimeValue & getDuration() const override;

  Clock & setDuration(TimeValue) override;

  const TimeValue & getOffset() const final override;

  Clock & setOffset(TimeValue) override;

  const TimeValue & getGranularity() const final override;

  Clock & setGranularity(TimeValue) final override;

  float getSpeed() const final override;

  Clock & setSpeed(float) final override;

  Clock::DriveMode getDriveMode() const final override;

  Clock & setDriveMode(Clock::DriveMode) final override;

  bool getRunning() const final override;

  const TimeValue & getPosition() const final override;

  const TimeValue & getDate() const final override;

  void setExecutionStatusCallback(ExecutionStatusCallback e) final override
  { mStatusCallback = e; }
  ExecutionStatusCallback getExecutionStatusCallback() const final override
  { return mStatusCallback; }
#if 0
# pragma mark -
# pragma mark Implementation specific
#endif
  /*! to avoid dead lock in EXTERNAL drive mode if a TimeProcess wants to end its ParentTimeConstraint's clock */
  void request_stop();

    protected:

  /*! to allow TimeConstraint to override start method */
  void do_start();

  /*! to allow TimeConstraint to override stop method */
  void do_stop();

  /*! to allow TimeConstraint to override setDuration accessor */
  void do_setDuration(TimeValue);

  /*! to allow TimeConstraint to override setOffset accessor */
  void do_setOffset(TimeValue);

private:

  /*! called back by the internal thread */
  void threadCallback();
};
}
