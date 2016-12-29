#include <ossia/editor/scenario/clock.hpp>
#include <ossia/detail/math.hpp>
#include <ossia/detail/logger.hpp>
#include <cassert>
#include <iostream>

namespace ossia
{

clock::clock(
    clock::ExecutionCallback callback, time_value duration,
    time_value granularity, time_value offset, float speed,
    clock::DriveMode driveMode)
    : mDuration(duration)
    , mGranularity(granularity)
    , mOffset(offset)
    , mSpeed(speed)
    , mDriveMode(driveMode)
    , mRunning(false)
    , mPaused(false)
    , mCallback(std::move(callback))
{
}

clock::~clock()
{
  stop();
}

void clock::start()
{
  do_start();
}

void clock::stop()
{
  do_stop();
}

void clock::pause()
{
  mPaused = true;
}

bool clock::paused() const
{
  return mPaused;
}

void clock::resume()
{
  mPaused = false;

  // reset the time reference
  mLastTime = clock_type::now();
}

bool clock::tick()
{
  using namespace std::chrono;
  bool paused = mPaused;
  bool running = mRunning;
  if (paused || !running)
    return false;

  int64_t granularityInUs(ossia::llround(mGranularity * 1000.));
  int64_t droppedTicks = 0;

  // how many time since the last tick ?
  int64_t deltaInUs
      = duration_cast<microseconds>(clock_type::now() - mLastTime).count();

  if (mDriveMode == clock::DriveMode::EXTERNAL)
  {
    // if too early: avoid this tick
    if (mElapsedTime / granularityInUs
        == (mElapsedTime + deltaInUs) / granularityInUs)
      return false;
  }
  else
  {
    // how much ticks it represents ?
    droppedTicks = ossia::llround(std::floor(deltaInUs / granularityInUs));

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
    int64_t pauseInUs = granularityInUs - mElapsedTime % granularityInUs;
    // if too early: wait
    if (pauseInUs > 0)
    {
      while (pauseInUs > 5000)
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
        while (duration_cast<microseconds>(clock_type::now() - t1).count()
               < (pauseInUs + 10))
          ;
      }

      deltaInUs
          = duration_cast<microseconds>(clock_type::now() - mLastTime).count()
            - droppedTicks * granularityInUs;
    }
  }

  // how many time elapsed from the start ?

  mDate += (deltaInUs / 1000.) * mSpeed;
  mElapsedTime += deltaInUs;

  // note the time now to evaluate how long is the callback processing
  mLastTime = clock_type::now();

  // test paused and running status after computing the date because there is a
  // sleep before
  if (!paused && running)
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

bool clock::tick(time_value usec)
{
  if (mPaused || !mRunning)
    return false;

  int64_t granularityInUs(ossia::llround(mGranularity * 1000));
  int64_t droppedTicks = 0;

  // how many time since the last tick ?
  int64_t deltaInUs = ossia::llround(usec);

  assert(mDriveMode == clock::DriveMode::EXTERNAL);
  // if too early: avoid this tick
  if (mElapsedTime / granularityInUs
      == (mElapsedTime + deltaInUs) / granularityInUs)
    return false;

  // how many time elapsed from the start ?
  mDate += (deltaInUs / 1000.) * mSpeed;
  mElapsedTime += deltaInUs;

  //! \debug cout << "+ " << (deltaInUs / 1000.) * mSpeed << endl;

  // note the time now to evaluate how long is the callback processing
  mLastTime = clock_type::now();

  // test paused and running status after computing the date because there is a
  // sleep before
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

time_value clock::getDuration() const
{
  return mDuration;
}

ossia::clock& clock::setDuration(time_value duration)
{
  do_setDuration(duration);
  return *this;
}

time_value clock::getOffset() const
{
  return mOffset;
}

ossia::clock& clock::setOffset(time_value offset)
{
  do_setOffset(offset);
  return *this;
}

time_value clock::getGranularity() const
{
  return mGranularity;
}

ossia::clock& clock::setGranularity(time_value granularity)
{
  mGranularity = granularity;
  return *this;
}

double clock::getSpeed() const
{
  return mSpeed;
}

ossia::clock& clock::setSpeed(double speed)
{
  mSpeed = speed;
  return *this;
}

clock::DriveMode clock::getDriveMode() const
{
  return mDriveMode;
}

ossia::clock& clock::setDriveMode(clock::DriveMode driveMode)
{
  mDriveMode = driveMode;
  return *this;
}

bool clock::getRunning() const
{
  return mRunning;
}

time_value clock::getPosition() const
{
  return mPosition;
}

time_value clock::getDate() const
{
  return mDate;
}

void clock::request_stop()
{
  if (mRunning)
  {
    if(mDriveMode == DriveMode::EXTERNAL)
    {
      mRunning = false;
    }
    else
    {
      mShouldStop = true;
    }

    mPaused = false;
    if (mStatusCallback)
      mStatusCallback(ClockExecutionStatus::STOPPED);
  }
}

void clock::setCallback(clock::ExecutionCallback c)
{
  mCallback = std::move(c);
}

void clock::do_start()
{
  if (mDuration <= mOffset)
    return stop();

  if (mRunning)
    return;

  // reset timing informations
  mRunning = true;
  mPaused = false;
  mShouldStop = false;

  // set clock at a tick
  mDate = std::floor(mOffset / (mGranularity * mSpeed))
          * (mGranularity * mSpeed);
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
    mThread = std::thread(&clock::threadCallback, this);
  }
}

void clock::setExecutionStatusCallback(ExecutionStatusCallback e)
{
    mStatusCallback = e;
}

clock::ExecutionStatusCallback clock::getExecutionStatusCallback() const
{
    return mStatusCallback;
}

void clock::do_stop()
{
  request_stop();

  if (mDriveMode == clock::DriveMode::INTERNAL)
  {
    if (mThread.joinable())
      mThread.join();
  }
}

void clock::do_setDuration(time_value duration)
{
  mDuration = duration;
  mDate = mOffset;

  if (mDuration != Zero)
    mPosition = mDate / mDuration;
  else
    mPosition = Zero;
}

void clock::do_setOffset(time_value offset)
{
  mOffset = offset;
  mDate = mOffset;

  if (mDuration != Zero)
    mPosition = mDate / mDuration;
  else
    mPosition = Zero;
}

void clock::threadCallback()
{
  try
  {
    // launch the tick if the duration is valid and while it have to run
    if (mDuration > Zero)
      while (mRunning && !mShouldStop)
        tick();

    if(mShouldStop)
      mRunning = false;
  }
  catch(std::exception& e)
  {
    logger().error("clock::threadCallback() catched: {}", e.what());
  }
  catch(...)
  {
    logger().error("An error occured in clock::threadCallback()");
  }
}
}
