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

#include <ctime>
#include <ratio>
#include <chrono>
#include <thread>
#include <mutex>

#include "Editor/Clock.h"

using namespace OSSIA;
using namespace std;
using namespace std::chrono;

class JamomaClock : public Clock
{
  
private:
  
# pragma mark -
# pragma mark Implementation specific
  
  TimeValue         mDuration;      /// the time (in ms) the clock will run at normal speed factor
  TimeValue         mGranularity;   /// the minimum time between each tick (in ms)
  TimeValue         mOffset;        /// the date (in ms) the clock will run from
  double            mSpeed;         /// the speed factor of the clock
  
  bool              mExternal;      /// if true the tick() method is called from outside
  
  bool              mRunning;       /// is the clock running right now ?
  bool              mPaused;        /// is the clock paused right now ?
  TimeValue         mPosition;      /// the progression of the clock between the beginning and the end [0. :: 1.]
  TimeValue         mDate;          /// how many time the clock is running (without no speed factor consideration)
  
  thread            mThread;        /// a thread to launch the clock execution
  
  steady_clock::time_point mLastTime;/// a time reference used to compute time tick
  long long         mElapsedTime;   /// a time reference used to know how many time are elapsed in microsecond
  
  ExecutionCallback mCallback;      /// the callback to use for each step
  
public:
  
# pragma mark -
# pragma mark Life cycle
  
  JamomaClock(const TimeValue&,
              const TimeValue&,
              const TimeValue&,
              float,
              bool);
  
  JamomaClock(const JamomaClock *);
  
  shared_ptr<Clock> clone() const override;
  
  ~JamomaClock();
  
# pragma mark -
# pragma mark Execution
  
  void go() override;
  
  void stop() override;
  
  void pause() override;
  
  void resume() override;
  
  bool tick() override;
  
  ExecutionCallback getExecutionCallback() const override;
  
  void setExecutionCallback(ExecutionCallback) override;
  
# pragma mark -
# pragma mark Accessors
  
  const TimeValue & getDuration() const override;
  
  Clock & setDuration(const TimeValue&) override;
  
  const TimeValue & getGranularity() const override;
  
  Clock & setGranularity(const TimeValue&) override;
  
  const TimeValue & getOffset() const override;
  
  Clock & setOffset(const TimeValue&) override;
  
  float getSpeed() const override;
  
  Clock & setSpeed(float) override;
  
  bool getExternal() const override;
  
  Clock & setExternal(bool) override;
  
  bool getRunning() const override;
  
  const TimeValue & getPosition() const override;
  
  const TimeValue & getDate() const override;
  
# pragma mark -
# pragma mark Internal
  
private:
  
  /*! called back by the internal thread */
  void threadCallback();
};
