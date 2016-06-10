#include "Editor/JamomaClock.h"

#include <iostream> //! \todo to remove. only here for debug purpose
#include <cassert>
using namespace OSSIA;

# pragma mark -
# pragma mark Life cycle

namespace OSSIA
{
  shared_ptr<Clock> Clock::create(Clock::ExecutionCallback callback,
                                  const TimeValue& duration,
                                  const TimeValue& granularity,
                                  const TimeValue& offset,
                                  float speed,
                                  Clock::DriveMode driveMode)
  {
    return make_shared<JamomaClock>(callback, duration, granularity, offset, speed, driveMode);
  }
}

JamomaClock::JamomaClock(Clock::ExecutionCallback callback,
                         const TimeValue& duration,
                         const TimeValue& granularity,
                         const TimeValue& offset,
                         float speed,
                         Clock::DriveMode driveMode) :
mDuration(duration),
mGranularity(granularity),
mOffset(offset),
mSpeed(speed),
mDriveMode(driveMode),
mRunning(false),
mPaused(false),
mCallback(callback)
{}

JamomaClock::JamomaClock(const JamomaClock * other) :
mDuration(other->mDuration),
mGranularity(other->mGranularity),
mOffset(other->mOffset),
mSpeed(other->mSpeed),
mDriveMode(other->mDriveMode),
mRunning(false),
mPaused(false),
mCallback(other->mCallback)
{}

JamomaClock::~JamomaClock()
{
  stop();
}

Clock::~Clock()
{}

# pragma mark -
# pragma mark Execution

void JamomaClock::start()
{
  do_start();
}

void JamomaClock::stop()
{
  do_stop();
}

void JamomaClock::pause()
{
  mPaused = true;
}

bool JamomaClock::paused() const
{
    return mPaused;
}

void JamomaClock::resume()
{
  mPaused = false;

  // reset the time reference
  mLastTime = steady_clock::now();
}

bool JamomaClock::tick()
{
  if (mPaused || !mRunning)
    return false;

  long long granularityInUs(mGranularity * 1000);
  int droppedTicks = 0;

  // how many time since the last tick ?
  long long deltaInUs = duration_cast<microseconds>(steady_clock::now() - mLastTime).count();

  if (mDriveMode == Clock::DriveMode::EXTERNAL)
  {
    // if too early: avoid this tick
    if (mElapsedTime / granularityInUs == (mElapsedTime + deltaInUs) / granularityInUs)
      return false;
  }
  else
  {
    // how much ticks it represents ?
    droppedTicks = std::floor(deltaInUs / granularityInUs);

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


bool JamomaClock::tick(TimeValue usec)
{
  if (mPaused || !mRunning)
    return false;

  long long granularityInUs(mGranularity * 1000);
  int droppedTicks = 0;

  // how many time since the last tick ?
  long long deltaInUs = usec;

  assert(mDriveMode == Clock::DriveMode::EXTERNAL);
  // if too early: avoid this tick
  if (mElapsedTime / granularityInUs == (mElapsedTime + deltaInUs) / granularityInUs)
      return false;

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

# pragma mark -
# pragma mark Accessors

const TimeValue & JamomaClock::getDuration() const
{
  return mDuration;
}

Clock & JamomaClock::setDuration(const TimeValue& duration)
{
  do_setDuration(duration);
  return *this;
}

const TimeValue & JamomaClock::getOffset() const
{
  return mOffset;
}

Clock & JamomaClock::setOffset(const TimeValue& offset)
{
  do_setOffset(offset);
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

float JamomaClock::getSpeed() const
{
  return mSpeed;
}

Clock & JamomaClock::setSpeed(float speed)
{
  mSpeed = speed;
  return *this;
}

Clock::DriveMode JamomaClock::getDriveMode() const
{
  return mDriveMode;
}

Clock & JamomaClock::setDriveMode(Clock::DriveMode driveMode)
{
  mDriveMode = driveMode;
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

void JamomaClock::request_stop()
{
  mRunning = false;
}

void JamomaClock::do_start()
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

  // notify the owner
  (mCallback)(mPosition, mDate, 0);

  if (mDriveMode == Clock::DriveMode::INTERNAL)
  {
    if (mThread.joinable())
      mThread.join();

    // launch a new thread to run the clock execution
    mThread = thread(&JamomaClock::threadCallback, this);
  }
}

void JamomaClock::do_stop()
{
  mRunning = false;
  mPaused = false;

  if (mDriveMode == Clock::DriveMode::INTERNAL)
  {
    if (mThread.joinable())
      mThread.join();
  }
}

void JamomaClock::do_setDuration(const TimeValue& duration)
{
  mDuration = duration;
  mDate = mOffset;

  if (mDuration != Zero)
    mPosition = mDate / mDuration;
  else
    mPosition = Zero;
}

void JamomaClock::do_setOffset(const TimeValue& offset)
{
  mOffset = offset;
  mDate = mOffset;

  if (mDuration != Zero)
    mPosition = mDate / mDuration;
  else
    mPosition = Zero;
}

void JamomaClock::threadCallback()
{
  // launch the tick if the duration is valid and while it have to run
  if (mDuration > Zero)
    while (mRunning)
      tick();
}
