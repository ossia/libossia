#include <ossia/editor/scenario/detail/Clock_impl.hpp>
#include <cassert>

#include <iostream>
namespace impl
{
JamomaClock::JamomaClock(clock::ExecutionCallback callback,
                         time_value duration,
                         time_value granularity,
                         time_value offset,
                         float speed,
                         clock::DriveMode driveMode) :
mDuration(duration),
mGranularity(granularity),
mOffset(offset),
mSpeed(speed),
mDriveMode(driveMode),
mRunning(false),
mPaused(false),
mCallback(callback)
{}

JamomaClock::~JamomaClock()
{
  stop();
}


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
  mLastTime = clock_type::now();
}

bool JamomaClock::tick()
{
  if (mPaused || !mRunning)
    return false;

  long long granularityInUs(mGranularity * 1000);
  int droppedTicks = 0;

  // how many time since the last tick ?
  long long deltaInUs = duration_cast<microseconds>(clock_type::now() - mLastTime).count();

  if (mDriveMode == clock::DriveMode::EXTERNAL)
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

        request_stop();

        return true;
      }
    }

    // how many time to pause to reach the next tick ?
    long long pauseInUs = granularityInUs - mElapsedTime % granularityInUs;

    // if too early: wait
    if (pauseInUs > 0)
    {
      while(pauseInUs > 5000)
      {
        // pause the thread logarithmically
        auto t1 = clock_type::now();
        std::this_thread::sleep_for(std::chrono::microseconds(pauseInUs / 2));
        auto t2 = clock_type::now();
        pauseInUs -= duration_cast<microseconds>(t2 - t1).count();
      }

      {
        // busy loop
        auto t1 = clock_type::now();
        while(duration_cast<microseconds>(clock_type::now() - t1).count() < (pauseInUs + 10)) ;
      }

      deltaInUs = duration_cast<microseconds>(clock_type::now() - mLastTime).count() - droppedTicks * granularityInUs;
    }
  }

  // how many time elapsed from the start ?
  mDate += (deltaInUs / 1000.) * mSpeed;
  mElapsedTime += deltaInUs;

  //! \debug cout << "+ " << (deltaInUs / 1000.) * mSpeed << endl;

  // note the time now to evaluate how long is the callback processing
  mLastTime = clock_type::now();

  // test paused and running status after computing the date because there is a sleep before
  if (!mPaused && mRunning)
  {
    mPosition = mDate / mDuration;

    // notify the owner
    (mCallback)(mPosition, mDate, droppedTicks);

    // is this the end
    if (mDuration - mDate < Zero && !mDuration.isInfinite())
    {
        request_stop();
    }
  }

  return true;
}


bool JamomaClock::tick(time_value usec)
{
  if (mPaused || !mRunning)
    return false;

  long long granularityInUs(mGranularity * 1000);
  int droppedTicks = 0;

  // how many time since the last tick ?
  long long deltaInUs = usec;

  assert(mDriveMode == clock::DriveMode::EXTERNAL);
  // if too early: avoid this tick
  if (mElapsedTime / granularityInUs == (mElapsedTime + deltaInUs) / granularityInUs)
      return false;

  // how many time elapsed from the start ?
  mDate += (deltaInUs / 1000.) * mSpeed;
  mElapsedTime += deltaInUs;

  //! \debug cout << "+ " << (deltaInUs / 1000.) * mSpeed << endl;

  // note the time now to evaluate how long is the callback processing
  mLastTime = clock_type::now();

  // test paused and running status after computing the date because there is a sleep before
  if (!mPaused && mRunning)
  {
    mPosition = mDate / mDuration;

    // notify the owner
    (mCallback)(mPosition, mDate, droppedTicks);

    // is this the end
    if (mDuration - mDate < Zero && !mDuration.isInfinite())
    {
      request_stop();
    }
  }

  return true;
}

# pragma mark -
# pragma mark Accessors

const time_value & JamomaClock::getDuration() const
{
  return mDuration;
}

ossia::clock & JamomaClock::setDuration(time_value duration)
{
  do_setDuration(duration);
  return *this;
}

const time_value & JamomaClock::getOffset() const
{
  return mOffset;
}

ossia::clock & JamomaClock::setOffset(time_value offset)
{
  do_setOffset(offset);
  return *this;
}

const time_value & JamomaClock::getGranularity() const
{
  return mGranularity;
}

ossia::clock & JamomaClock::setGranularity(time_value granularity)
{
  mGranularity = granularity;
  return *this;
}

float JamomaClock::getSpeed() const
{
  return mSpeed;
}

ossia::clock & JamomaClock::setSpeed(float speed)
{
  mSpeed = speed;
  return *this;
}

clock::DriveMode JamomaClock::getDriveMode() const
{
  return mDriveMode;
}

ossia::clock & JamomaClock::setDriveMode(clock::DriveMode driveMode)
{
  mDriveMode = driveMode;
  return *this;
}

bool JamomaClock::getRunning() const
{
  return mRunning;
}

const time_value & JamomaClock::getPosition() const
{
  return mPosition;
}

const time_value & JamomaClock::getDate() const
{
  return mDate;
}

# pragma mark -
# pragma mark Internal

void JamomaClock::request_stop()
{
    if(mRunning)
    {
        mRunning = false;
        mPaused = false;
        if(mStatusCallback)
            mStatusCallback(ClockExecutionStatus::STOPPED);
    }
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
  mLastTime = clock_type::now();
  mElapsedTime = std::floor(mOffset / mGranularity) * mGranularity * 1000;

  // notify the owner
  (mCallback)(mPosition, mDate, 0);

  if (mDriveMode == clock::DriveMode::INTERNAL)
  {
    if (mThread.joinable())
      mThread.join();

    // launch a new thread to run the clock execution
    mThread = std::thread(&JamomaClock::threadCallback, this);
  }
}

void JamomaClock::do_stop()
{
  request_stop();

  if (mDriveMode == clock::DriveMode::INTERNAL)
  {
    if (mThread.joinable())
      mThread.join();
  }
}

void JamomaClock::do_setDuration(time_value duration)
{
  mDuration = duration;
  mDate = mOffset;

  if (mDuration != Zero)
    mPosition = mDate / mDuration;
  else
    mPosition = Zero;
}

void JamomaClock::do_setOffset(time_value offset)
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
}
