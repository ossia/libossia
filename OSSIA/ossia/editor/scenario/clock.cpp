#include <ossia/editor/scenario/clock.hpp>
#include <ossia/detail/math.hpp>
#include <ossia/detail/logger.hpp>
#include <ossia/detail/thread.hpp>
#include <cassert>
#include <iostream>

namespace ossia
{

clock::clock(
    clock::exec_callback callback, time_value duration,
    time_value granularity, time_value offset, float speed,
    clock::drive_mode driveMode, clock_source s)
    : m_duration(duration)
    , m_granularity(granularity)
    , m_offset(offset)
    , m_speed(speed)
    , m_source{s}
    , m_drive_mode(driveMode)
    , m_running(false)
    , m_paused(false)
    , m_callback(std::move(callback))
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
  m_paused = true;
}

bool clock::paused() const
{
  return m_paused;
}

void clock::resume()
{
  m_paused = false;

  // reset the time reference
  m_lastTime = m_source.now();
}

bool clock::tick()
{
  using namespace std::chrono;
  bool paused = m_paused;
  bool running = m_running;
  if (paused || !running)
    return false;

  int64_t granularityInUs(ossia::llround(m_granularity * 1000.));
  int64_t droppedTicks = 0;

  // how many time since the last tick ?
  int64_t deltaInUs
      = duration_cast<microseconds>(m_source.now() - m_lastTime).count();

  if (m_drive_mode == clock::drive_mode::EXTERNAL)
  {
    // if too early: avoid this tick
    if (m_elapsedTime / granularityInUs
        == (m_elapsedTime + deltaInUs) / granularityInUs)
      return false;
  }
  else
  {
    // how much ticks it represents ?
    droppedTicks = ossia::llround(std::floor(deltaInUs / granularityInUs));

    // adjust date and elapsed time considering the dropped ticks
    if (droppedTicks)
    {
      m_date += droppedTicks * m_granularity * m_speed;
      m_elapsedTime += droppedTicks * granularityInUs;

      //! \debug cout << "+ " << droppedTicks * mGranularity * mSpeed << endl;

      // maybe the clock reaches the end ?
      if (m_duration - m_date < Zero && !m_duration.infinite())
      {
        m_position = m_date / m_duration;

        // notify the owner
        (m_callback)(m_position, m_date, droppedTicks);

        request_stop();

        return true;
      }
    }


    // how many time to pause to reach the next tick ?
    int64_t pauseInUs = granularityInUs - m_elapsedTime % granularityInUs;
    // if too early: wait
    if (pauseInUs > 0)
    {
      while (pauseInUs > 5000)
      {
        // pause the thread logarithmically
        auto t1 = m_source.now();
        std::this_thread::sleep_for(std::chrono::microseconds(pauseInUs / 2));
        auto t2 = m_source.now();
        pauseInUs -= duration_cast<microseconds>(t2 - t1).count();
      }

      {
        // busy loop
        auto t1 = m_source.now();
        while (duration_cast<microseconds>(m_source.now() - t1).count()
               < (pauseInUs + 10))
          ;
      }

      deltaInUs
          = duration_cast<microseconds>(m_source.now() - m_lastTime).count()
            - droppedTicks * granularityInUs;
    }
  }

  // how many time elapsed from the start ?

  m_date += (deltaInUs / 1000.) * m_speed;
  m_elapsedTime += deltaInUs;

  // note the time now to evaluate how long is the callback processing
  m_lastTime = m_source.now();

  // test paused and running status after computing the date because there is a
  // sleep before
  if (!paused && running)
  {
    m_position = m_date / m_duration;

    // notify the owner
    (m_callback)(m_position, m_date, droppedTicks);

    // is this the end
    if (m_duration - m_date < Zero && !m_duration.infinite())
    {
      request_stop();
    }
  }

  return true;
}

bool clock::tick(ossia::time_value usec)
{
  bool paused = m_paused;
  bool running = m_running;
  if (paused || !running)
    return false;

  // how many time since the last tick ?
  int64_t deltaInUs = ossia::llround(usec);

  assert(m_drive_mode == clock::drive_mode::EXTERNAL);

  // how many time elapsed from the start ?
  m_date += (deltaInUs / 1000.) * m_speed;
  m_elapsedTime += deltaInUs;

  //! \debug cout << "+ " << (deltaInUs / 1000.) * mSpeed << endl;

  // note the time now to evaluate how long is the callback processing
  m_lastTime = m_source.now();

  // test paused and running status after computing the date because there is a
  // sleep before
  if (!m_paused && m_running)
  {
    m_position = m_date / m_duration;

    // notify the owner
    (m_callback)(m_position, m_date, 0);

    // is this the end
    if (m_duration - m_date < Zero && !m_duration.infinite())
    {
      request_stop();
    }
  }

  return true;
}

time_value clock::get_duration() const
{
  return m_duration;
}

ossia::clock& clock::set_duration(ossia::time_value duration)
{
  do_set_duration(duration);
  return *this;
}

time_value clock::get_offset() const
{
  return m_offset;
}

ossia::clock& clock::set_offset(ossia::time_value offset)
{
  do_set_offset(offset);
  return *this;
}

time_value clock::get_granularity() const
{
  return m_granularity;
}

ossia::clock& clock::set_granularity(ossia::time_value granularity)
{
  m_granularity = granularity;
  return *this;
}

double clock::get_speed() const
{
  return m_speed;
}

ossia::clock& clock::set_speed(double speed)
{
  m_speed = speed;
  return *this;
}

clock::drive_mode clock::get_drive_mode() const
{
  return m_drive_mode;
}

ossia::clock& clock::set_drive_mode(clock::drive_mode driveMode)
{
  m_drive_mode = driveMode;
  return *this;
}

bool clock::running() const
{
  return m_running;
}

time_value clock::get_position() const
{
  return m_position;
}

time_value clock::get_date() const
{
  return m_date;
}

void clock::request_stop()
{
  if (m_running)
  {
    if(m_drive_mode == drive_mode::EXTERNAL)
    {
      m_running = false;
    }
    else
    {
      m_shouldStop = true;
    }

    m_paused = false;
    if (m_statusCallback)
      m_statusCallback(exec_status::STOPPED);
  }
}

void clock::set_callback(clock::exec_callback c)
{
  m_callback = std::move(c);
}

void clock::do_start()
{
  if (m_duration <= m_offset)
    return stop();

  if (m_running)
    return;

  // reset timing informations
  m_running = true;
  m_paused = false;
  m_shouldStop = false;

  // set clock at a tick
  m_date = std::floor(m_offset / (m_granularity * m_speed))
          * (m_granularity * m_speed);
  m_position = m_date / m_duration;
  m_lastTime = m_source.now();
  m_elapsedTime = std::floor(m_offset / m_granularity) * m_granularity * 1000;

  // notify the owner
  (m_callback)(m_position, m_date, 0);

  if (m_drive_mode == clock::drive_mode::INTERNAL)
  {
    if (m_thread.joinable())
      m_thread.join();

    // launch a new thread to run the clock execution
    m_thread = std::thread(&clock::threadCallback, this);
    set_thread_realtime(m_thread);
  }
}

void clock::set_exec_status_callback(exec_status_callback e)
{
    m_statusCallback = e;
}

clock::exec_status_callback clock::get_exec_status_callback() const
{
    return m_statusCallback;
}

void clock::do_stop()
{
  request_stop();

  if (m_drive_mode == clock::drive_mode::INTERNAL)
  {
    if (m_thread.joinable())
      m_thread.join();
  }
}

void clock::do_set_duration(ossia::time_value duration)
{
  m_duration = duration;

  if (m_duration != Zero)
    m_position = m_date / m_duration;
  else
    m_position = Zero;
}

void clock::do_set_offset(ossia::time_value offset)
{
  m_offset = offset;
  m_date = m_offset;

  if (m_duration != Zero)
    m_position = m_date / m_duration;
  else
    m_position = Zero;
}

void clock::threadCallback()
{
  try
  {
    // launch the tick if the duration is valid and while it have to run
    if (m_duration > Zero)
      while (m_running && !m_shouldStop)
        tick();

    if(m_shouldStop)
      m_running = false;
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
