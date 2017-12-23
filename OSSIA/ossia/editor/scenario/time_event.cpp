// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/editor/expression/expression.hpp>
#include <ossia/editor/scenario/time_interval.hpp>
#include <ossia/editor/scenario/time_event.hpp>
#include <ossia/editor/exceptions.hpp>

namespace ossia
{
time_event::time_event(
    time_event::exec_callback callback, time_sync& aTimeSync,
    expression_ptr anExpression)
    : m_callback(callback)
    , m_timesync(aTimeSync)
    , m_status(time_event::status::NONE)
    , m_expression(std::move(anExpression))
{
}

time_event::~time_event() = default;

void time_event::set_callback(time_event::exec_callback callback)
{
  m_callback = callback;
}

void time_event::happen(ossia::state& st)
{
  if (m_status != time_event::status::PENDING)
  {
    throw execution_error(
        "time_event::happen: "
        "only PENDING event can happens");
    return;
  }

  m_status = time_event::status::HAPPENED;

  // stop previous TimeIntervals
  for (auto& timeInterval : previous_time_intervals())
  {
    timeInterval->stop();
  }

  ossia::flatten_and_filter(st, this->get_state());

  // setup next TimeIntervals
  for (auto& timeInterval : next_time_intervals())
  {
    timeInterval->start(st);
  }

  if (m_callback)
    (m_callback)(m_status);
}

void time_event::dispose()
{
  if (m_status == time_event::status::HAPPENED)
  {
    throw execution_error(
        "time_event::dispose: "
        "HAPPENED event cannot be disposed");
    return;
  }

  m_status = time_event::status::DISPOSED;

  // stop previous TimeIntervals
  for (auto& timeInterval : previous_time_intervals())
  {
    timeInterval->stop();
  }

  // dispose next TimeIntervals end event if everything is disposed before
  for (auto& nextTimeInterval : next_time_intervals())
  {
    bool dispose = true;

    for (auto& previousTimeInterval :
         nextTimeInterval->get_end_event().previous_time_intervals())
    {
      if (previousTimeInterval->get_start_event().get_status()
          != time_event::status::DISPOSED)
      {
        dispose = false;
        break;
      }
    }

    if (dispose)
      nextTimeInterval->get_end_event().dispose();
  }

  if (m_callback)
    (m_callback)(m_status);
}

void time_event::add_state(state_element&& state)
{
  m_state.add(std::move(state));
}

void time_event::remove_state(const state_element& state)
{
  m_state.remove(state);
}

time_sync& time_event::get_time_sync() const
{
  return m_timesync;
}

const state& time_event::get_state() const
{
  return m_state;
}

const expression& time_event::get_expression() const
{
  return *m_expression;
}

time_event& time_event::set_expression(expression_ptr exp)
{
  assert(exp);

  m_expression = std::move(exp);

  return *this;
}

time_event::status time_event::get_status() const
{
  return m_status;
}

time_event::offset_behavior time_event::get_offset_behavior() const
{
  return m_offset;
}

time_event& time_event::set_offset_behavior(offset_behavior b)
{
  m_offset = b;
  return *this;
}

void time_event::set_status(status status)
{
  m_status = status;
  if (m_callback)
    (m_callback)(m_status);
}

void time_event::reset()
{
  set_status(status::NONE);
}

void time_event::cleanup()
{
  m_previous_time_intervals.clear();
  m_next_time_intervals.clear();
}
}
