#include "Editor/JamomaClock.h"

#include <iostream> //! \todo to remove. only here for debug purpose

using namespace OSSIA;

# pragma mark -
# pragma mark Life cycle

namespace OSSIA
{
  shared_ptr<Clock> Clock::create(const TimeValue& duration,
                                  const TimeValue& granularity,
                                  const TimeValue& offset,
                                  float speed,
                                  bool external)
  {
    return make_shared<JamomaClock>(duration, granularity, offset, speed, external);
  }
}

JamomaClock::JamomaClock(const TimeValue& duration,
                         const TimeValue& granularity,
                         const TimeValue& offset,
                         float speed,
                         bool external) :
mDuration(duration),
mGranularity(granularity),
mOffset(offset),
mSpeed(speed),
mExternal(external),
mRunning(false)
{}

JamomaClock::JamomaClock(const JamomaClock * other)
{}

shared_ptr<Clock> JamomaClock::clone() const
{
  return make_shared<JamomaClock>(this);
}

JamomaClock::~JamomaClock()
{
  stop();
}

# pragma mark -
# pragma mark Execution

void JamomaClock::go()
{
  if (mDuration <= mOffset)
    return stop();
  
  if (mRunning)
    return;
  
  // reset timing informations
  mRunning = true;
  mPaused = false;
  
  // set clock at a tick
  mDate = std::floor(mOffset / (mGranularity * mSpeed)) * (mGranularity * mSpeed);
  mPosition = mDate / mDuration;
  mLastTime = steady_clock::now();
  mElapsedTime = std::floor(mOffset / mGranularity) * mGranularity * 1000;
  
  //! \todo notify each observers
  // sendNotification(TTSymbol("ClockRunningChanged"), mRunning);
  
  if (!mExternal)
  {
    if (mThread.joinable())
      mThread.join();
    
    // launch a new thread to run the clock execution
    mThread = thread(&JamomaClock::threadCallback, this);
  }
}

void JamomaClock::stop()
{
  mRunning = false;
  mPaused = false;
  
  if (!mExternal)
  {
    if (mThread.joinable())
      mThread.join();
  }
  
  //! \todo notify each observers
  // sendNotification(TTSymbol("ClockRunningChanged"), mRunning);
}

void JamomaClock::pause()
{
  mPaused = true;
}

void JamomaClock::resume()
{
  mPaused = false;
}

bool JamomaClock::tick()
{
  if (mPaused || !mRunning)
    return false;
  
  long long granularityInUs(mGranularity * 1000);
  
  // how many time since the last tick ?
  long long deltaInUs = duration_cast<microseconds>(steady_clock::now() - mLastTime).count();
  
  // how much ticks it represents ?
  int droppedTicks = std::floor(deltaInUs / granularityInUs);
  
  // adjust date and elapsed time considering the dropped ticks
  if (droppedTicks)
  {
    mDate += droppedTicks * mGranularity * mSpeed;
    mElapsedTime += droppedTicks * granularityInUs;
    
    //! \debug cout << "+ " << droppedTicks * mGranularity * mSpeed << endl;

    // maybe the clock reaches the end ?
    if (mDuration - mDate < Zero && !mDuration.isInfinite())
    {
      mPosition = mDate / mDuration;
      
      // notify the owner
      (mCallback)(mPosition, mDate, droppedTicks);

      mRunning = false;
      mPaused = false;

      return true;
    }
  }

  if (mExternal)
  {
    // if too early: avoid this tick
    if (mElapsedTime / granularityInUs == (mElapsedTime + deltaInUs) / granularityInUs)
      return false;
  }
  else
  {
    // how many time to pause to reach the next tick ?
    long long pauseInUs = granularityInUs - mElapsedTime % granularityInUs;

    // if too early: wait
    if (pauseInUs > 0)
    {
      // pause the thread
      this_thread::sleep_for(std::chrono::microseconds(pauseInUs));
      
      // how many time since the last tick ? (minus dropped ticks)
      deltaInUs = duration_cast<microseconds>(steady_clock::now() - mLastTime).count() - droppedTicks * granularityInUs;
    }
  }
  
  // how many time elapsed from the start ?
  mDate += (deltaInUs / 1000.) * mSpeed;
  mElapsedTime += deltaInUs;

  //! \debug cout << "+ " << (deltaInUs / 1000.) * mSpeed << endl;
  
  // note the time now to evaluate how long is the callback processing
  mLastTime = steady_clock::now();
  
  // test paused and running status after computing the date because there is a sleep before
  if (!mPaused && mRunning)
  {
    mPosition = mDate / mDuration;
    
    // notify the owner
    (mCallback)(mPosition, mDate, droppedTicks);
    
    // is this the end
    if (mDuration - mDate < Zero && !mDuration.isInfinite())
    {
      mRunning = false;
      mPaused = false;
    }
  }
  
  return true;
}

JamomaClock::ExecutionCallback JamomaClock::getExecutionCallback() const
{
  return mCallback;
}

void JamomaClock::setExecutionCallback(ExecutionCallback callback)
{
  mCallback = callback;
}

# pragma mark -
# pragma mark Accessors

const TimeValue & JamomaClock::getDuration() const
{
  return mDuration;
}

Clock & JamomaClock::setDuration(const TimeValue& duration)
{
  mDuration = duration;
  mDate = mOffset;
  
  if (mDuration != Zero)
    mPosition = mDate / mDuration;
  else
    mPosition = Zero;
  
  return *this;
}

const TimeValue & JamomaClock::getGranularity() const
{
  return mGranularity;
}

Clock & JamomaClock::setGranularity(const TimeValue& granularity)
{
  mGranularity = granularity;
  return *this;
}

const TimeValue & JamomaClock::getOffset() const
{
  return mOffset;
}

Clock & JamomaClock::setOffset(const TimeValue& offset)
{
  mOffset = offset;
  mDate = mOffset;
  
  if (mDuration != Zero)
    mPosition = mDate / mDuration;
  else
    mPosition = Zero;
  
  return *this;
}

float JamomaClock::getSpeed() const
{
  return mSpeed;
}

Clock & JamomaClock::setSpeed(float speed)
{
  mSpeed = speed;
  return *this;
}

bool JamomaClock::getExternal() const
{
  return mExternal;
}

Clock & JamomaClock::setExternal(bool external)
{
  mExternal = external;
  return *this;
}

bool JamomaClock::getRunning() const
{
  return mRunning;
}

const TimeValue & JamomaClock::getPosition() const
{
  return mPosition;
}

const TimeValue & JamomaClock::getDate() const
{
  return mDate;
}

# pragma mark -
# pragma mark Internal

void JamomaClock::threadCallback()
{
  // launch the tick if the duration is valid and while it have to run
  if (mDuration > Zero)
    while (mRunning)
      tick();
}
