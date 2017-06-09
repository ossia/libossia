#include <ossia/editor/scenario/time_constraint.hpp>
#include <ossia/editor/scenario/time_event.hpp>
#include <ossia/editor/scenario/time_process.hpp>
#include <ossia/detail/algorithms.hpp>
#include <ossia/detail/logger.hpp>
#include <algorithm>
#include <iostream>
namespace ossia
{
std::shared_ptr<time_constraint> time_constraint::create(
    time_constraint::exec_callback callback,
    time_event& startEvent,
    time_event& endEvent, time_value nominal, time_value min,
    time_value max)
{
  auto timeConstraint = std::make_shared<time_constraint>(
        callback, startEvent, endEvent, nominal, min, max);

  startEvent.next_time_constraints().push_back(timeConstraint);
  endEvent.previous_time_constraints().push_back(timeConstraint);

  return timeConstraint;
}


time_constraint::time_constraint(
    time_constraint::exec_callback callback,
    time_event& startEvent,
    time_event& endEvent, time_value nominal, time_value min,
    time_value max)
  : m_callback(std::move(callback))
  , m_clock(make_callback(), max, One)
  , m_start(startEvent)
  , m_end(endEvent)
  , m_nominal(nominal)
  , m_min(min)
  , m_max(max)
{
}

time_constraint::~time_constraint()
{
}

void time_constraint::start()
{
  if (m_clock.running())
  {
    throw execution_error("time_constraint::start: "
                          "time constraint is already running");
    return;
  }

  // set clock duration using maximal duration
  m_clock.set_duration(m_max);

  // start all jamoma time processes
  for (const auto& timeProcess : get_time_processes())
  {
    if(timeProcess->enabled())
      timeProcess->start();
  }

  // launch the clock
  m_clock.do_start();
}

void time_constraint::stop()
{
  // stop the clock
  m_clock.do_stop();

  // stop all jamoma time processes
  for (const auto& timeProcess : get_time_processes())
  {
    timeProcess->stop();
  }

  m_clock.m_date = 0;
  m_clock.m_position = 0;
  m_clock.m_lastTime = clock_type::time_point{};
  m_clock.m_elapsedTime = 0;
}

ossia::state time_constraint::offset(ossia::time_value date)
{
  if (m_clock.running())
  {
    throw execution_error("time_constraint::offset: "
                          "time constraint is running");
    return {};
  }

  m_clock.do_set_offset(date);

  const auto& processes = get_time_processes();
  ossia::state state;
  state.reserve(processes.size());

  // get the state of each TimeProcess at current clock position and date
  for (const auto& timeProcess : processes)
  {
    if(timeProcess->enabled())
    {
      auto res = timeProcess->offset(date);
      if(res)
        state.add(std::move(res));
    }
  }

  return state;
}

ossia::state time_constraint::state()
{
  if (!m_clock.running())
  {
    throw execution_error("time_constraint::state: "
                          "time constraint is not running");
    return {};
  }

  return state_impl();
}

void time_constraint::pause()
{
  m_clock.pause();

  // pause all jamoma time processes
  for (const auto& timeProcess : get_time_processes())
  {
    timeProcess->pause();
  }
}

void time_constraint::resume()
{
  m_clock.resume();

  // resume all jamoma time processes
  for (const auto& timeProcess : get_time_processes())
  {
    timeProcess->resume();
  }
}

void time_constraint::set_callback(
    time_constraint::exec_callback callback)
{
  m_callback = callback;
  m_clock.set_callback(make_callback());
}

void time_constraint::set_stateless_callback(time_constraint::exec_callback cb)
{
  m_callback = cb;
  m_clock.set_callback(make_stateless_callback());
}

const time_value& time_constraint::get_nominal_duration() const
{
  return m_nominal;
}

time_constraint&
time_constraint::set_nominal_duration(ossia::time_value durationNominal)
{
  m_nominal = durationNominal;

  if (m_nominal < m_min)
    set_min_duration(m_nominal);

  if (m_nominal > m_max)
    set_max_duration(m_nominal);

  m_clock.set_duration(m_nominal);

  return *this;
}

const time_value& time_constraint::get_min_duration() const
{
  return m_min;
}

time_constraint& time_constraint::set_min_duration(ossia::time_value durationMin)
{
  m_min = durationMin;

  if (m_min > m_nominal)
    set_nominal_duration(m_min);

  return *this;
}

const time_value& time_constraint::get_max_duration() const
{
  return m_max;
}

time_constraint& time_constraint::set_max_duration(ossia::time_value durationMax)
{
  m_max = durationMax;

  if (durationMax < m_nominal)
    set_nominal_duration(m_max);

  return *this;
}

time_event& time_constraint::get_start_event() const
{
  return m_start;
}

time_event& time_constraint::get_end_event() const
{
  return m_end;
}

void time_constraint::add_time_process(
    std::shared_ptr<time_process> timeProcess)
{
  if(!timeProcess)
    return;

  // store a TimeProcess if it is not already stored
  if (find(m_processes, timeProcess) == m_processes.end())
  {
    timeProcess->m_parent = this;
    m_processes.push_back(std::move(timeProcess));
  }
}

void time_constraint::remove_time_process(
    time_process* timeProcess)
{
  auto it = find_if(m_processes, [=] (const auto& other) { return other.get() == timeProcess; });
  if (it != m_processes.end())
  {
    m_processes.erase(it);
  }
}

clock::exec_callback time_constraint::make_callback()
{
  if(m_callback)
  {
    return [this] (ossia::time_value t, time_value t2, unsigned char c) {
      m_callback(t, t2, state_impl());
    };
  }
  else
  {
    return [] (ossia::time_value t, time_value t2, unsigned char c) { };
  }
}


clock::exec_callback time_constraint::make_stateless_callback()
{
  if(m_callback)
  {
    return [this] (ossia::time_value t, time_value t2, unsigned char c) {
      m_callback(t, t2, {});
    };
  }
  else
  {
    return [] (ossia::time_value t, time_value t2, unsigned char c) { };
  }
}

ossia::state time_constraint::state_impl()
{
  const auto& processes = get_time_processes();
  ossia::state state;
  state.reserve(processes.size());

  // get the state of each TimeProcess at current clock position and date
  for (const auto& timeProcess : processes)
  {
    if(timeProcess->enabled())
    {
      auto res = timeProcess->state();
      if(res)
        state.add(std::move(res));
    }
  }

  return state;
}

}
