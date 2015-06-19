/*!
 * \file JamomaClock.h
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

#include <sys/time.h> //! \todo this file doesn't exist on Windows system
#include <thread>
#include <mutex>

#include "Editor/Clock.h"

using namespace OSSIA;
using namespace std;

class JamomaClock : public Clock
{
  
private:
  
# pragma mark -
# pragma mark Implementation specific
  
  TimeValue         mDuration;      /// the time (in ms) the clock will run at normal speed factor
  TimeValue         mOffset;        /// the date (in ms) the clock will run from
  double            mSpeed;         /// the speed factor of the clock
  
  bool              mExternalTick;  /// if true the tick() method is called from outside
  //! bool              mInfinite;      /// if true the clock will run until stop method call (this attribute overrides mDuration) \todo use TimeValue::Inf
  
  bool              mRunning;       /// is the clock running right now ?
  bool              mPaused;        /// is the clock paused right now ?
  double            mPosition;      /// the progression of the clock between the beginning and the end [0. :: 1.]
  TimeValue         mDate;          /// how many time the clock is running (without no speed factor consideration)
  
  double            mGranularity;   /// the minimun time between each tick (in ms)
  
  thread            mThread;        /// a thread to launch the clock execution
  mutex             mThreadMutex;   /// prevents the thread object from being used concurrently
  uint64_t          mLastTime;      /// a time reference used to compute delta time between each tick (in µs)
  
  ExecutionCallback mCallback;      /// the callback to use for each step
  
public:
  
# pragma mark -
# pragma mark Life cycle
  
  JamomaClock(const TimeValue& = 0.,
              const TimeValue& = 0.,
              float = 0.);
  
  ~JamomaClock();
  
# pragma mark -
# pragma mark Execution
  
  void go() override;
  
  void stop() override;
  
  void pause() override;
  
  void resume() override;
  
  void tick() override;
  
# pragma mark -
# pragma mark Accessors
  
  const TimeValue & getDuration() const override;
  
  Clock & setDuration(const TimeValue&) override;
  
  const TimeValue & getOffset() const override;
  
  Clock & setOffset(const TimeValue&) override;
  
  float getSpeed() const override;
  
  Clock & setSpeed(float) override;
  
# pragma mark -
# pragma mark Callback
  
  ExecutionCallback getExecutionCallback() const override;
  
  void setExecutionCallback(ExecutionCallback) override;

# pragma mark -
# pragma mark Internal
  
private:
  
  /*! compute the amount of time spent since the last call */
  double computeDeltaTime();
  
  /*! called back by the internal thread */
  void threadCallback();
  
  /*! stop the internal thread execution */
  void threadStop();

};
