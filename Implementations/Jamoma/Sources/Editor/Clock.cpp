#include "Editor/JamomaClock.h"

#include <iostream> //! \todo to remove. only here for debug purpose

using namespace OSSIA;

# pragma mark -
# pragma mark Life cycle

std::shared_ptr<Clock> Clock::create(const TimeValue& duration,
                                     const TimeValue& granularity,
                                     const TimeValue& offset,
                                     float speed,
                                     bool external)
{
  return make_shared<JamomaClock>(duration, granularity, offset, speed, external);
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
mExternal(external)
{}

JamomaClock::JamomaClock(const JamomaClock * other)
{}

JamomaClock::~JamomaClock()
{}

shared_ptr<Clock> JamomaClock::clone() const
{
  return make_shared<JamomaClock>(this);
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
  mPosition = mOffset / mDuration;
  mDate = mOffset;
  mLastTime = steady_clock::now();
  mElapsedTime = mOffset * 1000;
  
  // notify the owner
  if (!mExternal)
    (mCallback)(mPosition, mDate);
  
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
  if (mRunning)
  {
    mRunning = false;
    
    if (!mExternal)
    {
      while (!mThread.joinable())
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
      
      try
      {
        mThread.join();
      }
      catch (std::system_error& e)
      {
        // thread::join failed: Resource deadlock avoided
        // this error is expected as the clock have to close its thread it self when reaching the end
        ;
      }
    }
    
    // reset all time info
    mPaused = false;
    mOffset = Zero;
    mPosition = Zero;
    mDate = Zero;
    
    //! \todo notify each observers
    // sendNotification(TTSymbol("ClockRunningChanged"), mRunning);
  }
}

void JamomaClock::pause()
{
  mPaused = true;
}

void JamomaClock::resume()
{
  mPaused = false;
}

void JamomaClock::tick()
{
  if (mPaused || !mRunning)
     return;
  
  long long granularityInUs(mGranularity * 1000);
  
  // how many time since the last tick ?
  long long deltaInUs = duration_cast<microseconds>(steady_clock::now() - mLastTime).count();
  
  if (mExternal)
  {
    // if too early: avoid this tick
    if (mElapsedTime / granularityInUs == (mElapsedTime + deltaInUs) / granularityInUs)
      return;
  }
  else
  {
    // how many time to pause to reach the next time grain ?
    long long pauseInUs = granularityInUs - mElapsedTime % granularityInUs;
    
    // if too early: wait
    if (pauseInUs > 0)
    {
      // pause the thread
      this_thread::sleep_for(std::chrono::microseconds(pauseInUs));
      
      // how many time since the last tick ?
      deltaInUs = duration_cast<microseconds>(steady_clock::now() - mLastTime).count();
    }
  }
  
  // how many time elapsed from the start ?
  mDate = TimeValue(mElapsedTime / 1000.) + TimeValue(deltaInUs / 1000.) * mSpeed;
  mElapsedTime += deltaInUs;

  // test also paused and running status after computing the date because there is a sleep before
  if (!mPaused && mRunning)
  {
    if (mDuration - mDate >= 0. || mDuration.isInfinite())
    {
      mPosition = mDate / mDuration;
      
      // notify the owner
      (mCallback)(mPosition, mDate);
    }
    else
    {
      // forcing position to 1. to allow filtering
      mPosition = One;
      
      // notify the owner
      (mCallback)(mPosition, mDate);
      
      // stop the clock
      stop();
    }
  }
  
  // tick is done now
  mLastTime = steady_clock::now();
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
  mPosition = mDate / mDuration;
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
  mPosition = mDate / mDuration;
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