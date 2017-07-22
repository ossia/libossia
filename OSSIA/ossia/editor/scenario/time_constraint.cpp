// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/detail/algorithms.hpp>
#include <ossia/detail/logger.hpp>
#include <ossia/editor/scenario/time_constraint.hpp>
#include <ossia/editor/scenario/time_event.hpp>
#include <ossia/editor/scenario/time_process.hpp>
#include <ossia/editor/state/state_element.hpp>
#include <algorithm>
#include <iostream>
namespace ossia
{
void time_constraint::tick(time_value usec)
{
  m_date += std::llround(usec.impl * m_speed);
  m_position = double(m_date) / double(m_nominal);

  if (m_callback)
    (m_callback)(m_position, m_date, state());
}

void time_constraint::tick(time_value usec, double ratio)
{
  m_date += std::llround(usec.impl * m_speed / ratio);
  m_position = double(m_date) / double(m_nominal);

  if (m_callback)
    (m_callback)(m_position, m_date, state());
}

std::shared_ptr<time_constraint> time_constraint::create(
    time_constraint::exec_callback callback, time_event& startEvent,
    time_event& endEvent, time_value nominal, time_value min, time_value max)
{
  auto timeConstraint = std::make_shared<time_constraint>(
      callback, startEvent, endEvent, nominal, min, max);

  startEvent.next_time_constraints().push_back(timeConstraint);
  endEvent.previous_time_constraints().push_back(timeConstraint);

  return timeConstraint;
}

time_constraint::time_constraint(
    time_constraint::exec_callback callback, time_event& startEvent,
    time_event& endEvent, time_value nominal, time_value min, time_value max)
    : m_callback(std::move(callback))
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
  if (!m_running)
  {
    m_running = true;
    // start all time processes
    for (const auto& timeProcess : get_time_processes())
    {
      if (timeProcess->enabled())
        timeProcess->start();
    }

    // launch the clock
    if (m_nominal <= m_offset)
      return stop();

    // set clock at a tick
    m_date = m_offset;
    tick(0_tv);
  }
}

void time_constraint::stop()
{
  if (m_running)
  {
    // stop all time processes
    for (const auto& timeProcess : get_time_processes())
    {
      timeProcess->stop();
    }
    m_date = 0;
    m_position = 0;
    m_running = false;
  }
}

ossia::state_element time_constraint::offset(ossia::time_value date)
{
  m_offset = date;
  m_date = date;

  if (m_nominal != Zero)
    m_position = m_date / m_nominal;
  else
    m_position = Zero;

  const auto& processes = get_time_processes();
  const auto N = processes.size();
  if (N > 0)
  {
    ossia::state state;
    state.reserve(N);
    const auto pos = date / get_nominal_duration();

    // get the state of each TimeProcess at current clock position and date
    for (const auto& timeProcess : processes)
    {
      if (timeProcess->enabled())
      {
        auto res = timeProcess->offset(date, pos);
        if (res)
          state.add(std::move(res));
      }
    }
    return state;
  }
  return {};
}

ossia::state_element time_constraint::state()
{
  const auto& processes = get_time_processes();
  const auto N = processes.size();
  if (N > 0)
  {
    ossia::state state;
    state.reserve(N);

    const auto date = get_date();
    const auto pos = date / get_nominal_duration();

    // get the state of each TimeProcess at current clock position and date
    for (const std::shared_ptr<ossia::time_process>& timeProcess : processes)
    {
      auto& p = *timeProcess;
      if (p.enabled())
      {
        if (auto res = p.state(date, pos))
          state.add(std::move(res));
      }
    }

    return state;
  }
  return {};
}

void time_constraint::pause()
{
  // pause all time processes
  for (const auto& timeProcess : get_time_processes())
  {
    timeProcess->pause();
  }
}

void time_constraint::resume()
{
  // resume all time processes
  for (const auto& timeProcess : get_time_processes())
  {
    timeProcess->resume();
  }
}

void time_constraint::set_callback(time_constraint::exec_callback callback)
{
  m_callback = callback;
}

void time_constraint::set_stateless_callback(time_constraint::exec_callback cb)
{
  m_callback = cb;
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

  return *this;
}

const time_value& time_constraint::get_min_duration() const
{
  return m_min;
}

time_constraint&
time_constraint::set_min_duration(ossia::time_value durationMin)
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

time_constraint&
time_constraint::set_max_duration(ossia::time_value durationMax)
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
  if (!timeProcess)
    return;

  // store a TimeProcess if it is not already stored
  if (find(m_processes, timeProcess) == m_processes.end())
  {
    m_processes.push_back(std::move(timeProcess));
  }
}

void time_constraint::remove_time_process(time_process* timeProcess)
{
  auto it = find_if(m_processes, [=](const auto& other) {
    return other.get() == timeProcess;
  });
  if (it != m_processes.end())
  {
    m_processes.erase(it);
  }
}
}
