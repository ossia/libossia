// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/editor/expression/expression.hpp>
#include <ossia/editor/scenario/time_interval.hpp>
#include <ossia/editor/scenario/time_event.hpp>
#include <ossia/editor/scenario/time_process.hpp>
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
  if(!m_expression)
    m_expression = ossia::expressions::make_expression_true();
}

time_event::~time_event() = default;

void time_event::set_callback(time_event::exec_callback callback)
{
  m_callback = callback;
}

void time_event::add_time_process(
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

void time_event::remove_time_process(time_process* timeProcess)
{
  auto it = find_if(m_processes, [=](const auto& other) {
    return other.get() == timeProcess;
  });
  if (it != m_processes.end())
  {
    m_processes.erase(it);
  }
}

void time_event::tick(ossia::time_value date, double pos, ossia::time_value offset)
{
  for(auto& proc : m_processes)
  {
    proc->start();
    proc->state(0_tv, date, pos, offset, 1.);
    proc->stop();
  }
}
time_sync& time_event::get_time_sync() const
{
  return m_timesync;
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
  set_callback({});
  m_previous_time_intervals.clear();
  m_next_time_intervals.clear();
}
}
