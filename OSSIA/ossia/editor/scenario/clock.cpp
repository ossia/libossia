// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/detail/logger.hpp>
#include <ossia/detail/math.hpp>
#include <ossia/detail/thread.hpp>
#include <ossia/editor/scenario/clock.hpp>
#include <ossia/editor/scenario/time_interval.hpp>
#include <cassert>
#include <iostream>
#include <ossia/editor/state/message.hpp>
#include <ossia/editor/state/state.hpp>

namespace ossia
{
clock::clock(ossia::time_interval& cst, double ratio)
    : m_interval{cst}
    , m_ratio(ratio)
    , m_duration{
          cst.get_max_duration().infinite() ? time_value::infinity
                                            : cst.get_max_duration() * ratio}
    , m_granularity{1}
    , m_running{false}
    , m_paused{false}
{
}

clock::~clock()
{
  stop();
}

void clock::start_and_tick()
{
  if (m_running)
    return;

  // reset timing informations
  m_running = true;
  m_paused = false;
  m_shouldStop = false;

  // set clock at a tick
  m_date = 0.;
  m_lastTime = clock_type::now();
  m_elapsedTime = 0.;

  // notify the owner
  m_interval.start();
  m_interval.tick_current(0_tv);

  if (m_thread.joinable())
    m_thread.join();

  // launch a new thread to run the clock execution
  m_thread = std::thread(&clock::thread_callback, this);
  set_thread_realtime(m_thread);
}

void clock::stop()
{
  request_stop();

  if (m_thread.joinable())
    m_thread.join();

  m_interval.stop();

  m_date = 0;
  m_lastTime = clock_type::time_point{};
  m_elapsedTime = 0;
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
  m_lastTime = clock_type::now();
}

bool clock::tick()
{
  using namespace std::chrono;
  bool paused = m_paused;
  bool running = m_running;
  if (paused || !running)
    return false;

  int64_t droppedTicks = 0;

  // how many time since the last tick ?
  int64_t deltaInUs
      = duration_cast<microseconds>(clock_type::now() - m_lastTime).count();

  // how much ticks it represents ?
  droppedTicks = ossia::llround(std::floor(double(deltaInUs) / double(m_granularity)));

  // adjust date and elapsed time considering the dropped ticks
  if (droppedTicks)
  {
    m_date += droppedTicks * m_granularity;
    m_elapsedTime += droppedTicks * m_granularity;

    // maybe the clock reaches the end ?
    if (m_duration - m_date < Zero && !m_duration.infinite())
    {
      // notify the owner
      m_interval.tick_offset(time_value{deltaInUs}, 0_tv);

      request_stop();

      return true;
    }
  }

  // how many time to pause to reach the next tick ?
  int64_t pauseInUs = m_granularity - m_elapsedTime % m_granularity;
  // std::cerr << m_granularity << " " << m_ratio << " " << deltaInUs << " "<<
  // droppedTicks << " "  << granularityInUs << " " << pauseInUs << std::endl;
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
        = duration_cast<microseconds>(clock_type::now() - m_lastTime).count()
          - droppedTicks * m_granularity;
  }

  // std::cerr << deltaInUs << std::endl;
  // how many time elapsed from the start ?

  m_date += deltaInUs;
  m_elapsedTime += deltaInUs;

  // note the time now to evaluate how long is the callback processing
  m_lastTime = clock_type::now();

  // test paused and running status after computing the date because there is a
  // sleep before
  if (!paused && running)
  {
    // notify the owner
    m_interval.tick(time_value{deltaInUs}, m_ratio);

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

time_value clock::get_granularity() const
{
  return m_granularity;
}

ossia::clock& clock::set_granularity(std::chrono::milliseconds granularity)
{
  m_granularity = granularity.count() * 1000;
  return *this;
}

ossia::clock& clock::set_granularity(std::chrono::microseconds granularity)
{
  m_granularity = granularity.count();
  return *this;
}

bool clock::running() const
{
  return m_running;
}

time_value clock::get_date() const
{
  return m_date;
}

void clock::request_stop()
{
  if (m_running)
  {
    m_shouldStop = true;
    m_paused = false;
    if (m_statusCallback)
      m_statusCallback(exec_status::STOPPED);
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

void clock::do_set_duration(ossia::time_value duration)
{
  m_duration = duration;
}

void clock::thread_callback()
{
  try
  {
    // launch the tick if the duration is valid and while it have to run
    if (m_duration > Zero)
    {

      while (m_running && !m_shouldStop)
      {
        if (!m_paused)
          tick();
        else
          std::this_thread::sleep_for(std::chrono::milliseconds(30));
      }
    }

    if (m_shouldStop)
      m_running = false;
  }
  catch (std::exception& e)
  {
    logger().error("clock::threadCallback() catched: {}", e.what());
  }
  catch (...)
  {
    logger().error("An error occured in clock::threadCallback()");
  }
}
}
