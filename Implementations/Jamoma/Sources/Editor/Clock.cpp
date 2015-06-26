#include "Editor/JamomaClock.h"

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
  // do we need to ramp at all ?
  if (mDuration <= mOffset)
  {
    threadStop();
    mRunning = false;
    mPaused = false;
    mPosition = Zero;
    mDate = Zero;
    
    // notify the owner
    mCallback(mPosition, mDate);
    
    //! \todo notify each observers
    // sendNotification(TTSymbol("ClockRunningChanged"), mRunning);
  }
  else if (mExternal)
  {
    // reset timing informations
    mRunning = true;
    mPaused = false;
    mLastTime = steady_clock::now();
    
    //! \todo notify each observers
    // sendNotification(TTSymbol("ClockRunningChanged"), mRunning);
    
    // launch a first tick if the duration is valid
    if (mDuration > Zero)
      tick();
  }
  // if the thread is not running
  else if (!mRunning)
  {
    if (mThread.joinable())
      mThread.join();
    
    // launch a new thread to run the clock execution
    mThread = thread(&JamomaClock::threadCallback, this);
  }
}

void JamomaClock::stop()
{
  mPaused = false;
  
  // stop thread execution
  threadStop();
  
  //! \todo notify each observers
  // sendNotification(TTSymbol("ClockRunningChanged"), mRunning);
  
  // reset all time info
  mOffset = Zero;
  mPosition = Zero;
  mDate = Zero;
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
  // in external drive mode : don't wait to not block external driver
  TimeValue elapsedTime = computeElapsedTime(!mExternal);
  
  // test paused and running status after the computeDeltatTime because there is a sleep inside
  if (mPaused || !mRunning)
    return;
  
  // in external drive mode : allow to tick only if elapsed time is greater than granularity
  if (mExternal && elapsedTime < mGranularity)
    return;
  
  TimeValue delta = elapsedTime * mSpeed;
  
  mPosition += delta / mDuration;
  mDate += delta;
  
  if ((mDuration - mDate) >= mGranularity || mDuration.isInfinite())
  {
    // notify the owner
    (mCallback)(mPosition, mDate);
  }
  else
  {
    // forcing position to 1. to allow filtering
    mPosition = One;
    
    // notify the owner
    (mCallback)(mPosition, mDate);
    
    // if the clock is still running : stop it
    // note : because it is possible that another thread stops the clock before
    if (mRunning)
      stop();
  }
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
# pragma mark Callback

JamomaClock::ExecutionCallback JamomaClock::getExecutionCallback() const
{
  return mCallback;
}

void JamomaClock::setExecutionCallback(ExecutionCallback callback)
{
  mCallback = callback;
}

# pragma mark -
# pragma mark Internal

TimeValue JamomaClock::computeElapsedTime(bool wait)
{
  // compute the duration in microseconds between the last tick and now
  steady_clock::time_point currentTime = steady_clock::now();
  long long deltaInUs = duration_cast<microseconds>(currentTime - mLastTime).count();
  long long granularityInUs(mGranularity * 1000);
  
  if (wait)
  {
    // wait if the duration is less than granularity
    if (deltaInUs < granularityInUs)
    {
      this_thread::sleep_for(std::chrono::microseconds(granularityInUs - deltaInUs));
      
      deltaInUs = granularityInUs;
    }
    
    // memorise new time point
    mLastTime = steady_clock::now();
  }
  
  // return the delta in ms
  return TimeValue(deltaInUs / 1000.);
}

void JamomaClock::threadCallback()
{
  // reset timing informations
  mRunning = true;
  mPaused = false;
  mLastTime = steady_clock::now();
  
  //! \todo notify each observers
  // sendNotification(TTSymbol("ClockRunningChanged"), mRunning);
  
  // launch the tick if the duration is valid and while it have to run
  if (mDuration > Zero)
    while (mRunning)
      tick();
}

void JamomaClock::threadStop()
{
  if (mRunning)
  {
    mRunning = false;
    while (!mThread.joinable())
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    try
    {
      mThread.join();
    }
    catch (std::system_error& e)
    {
      //! \todo should we throw an exception here ?
      // cout << "JamomaClock::threadStop() : " << e.what() << "\n";
    }
  }
}