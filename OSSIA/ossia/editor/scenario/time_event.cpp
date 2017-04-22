#include <ossia/editor/scenario/time_event.hpp>
#include <ossia/editor/scenario/time_constraint.hpp>
#include <ossia/editor/expression/expression.hpp>

namespace ossia
{
time_event::time_event(
    time_event::exec_callback callback,
    time_node& aTimeNode,
    expression_ptr anExpression)
    : m_callback(callback)
    , m_timenode(aTimeNode)
    , m_status(time_event::status::NONE)
    , m_expression(std::move(anExpression))
{
}

time_event::~time_event() = default;

void time_event::set_callback(time_event::exec_callback callback)
{
  m_callback = callback;
}

void time_event::happen()
{
  if (m_status != time_event::status::PENDING)
  {
    throw execution_error("time_event::happen: "
                          "only PENDING event can happens");
    return;
  }

  m_status = time_event::status::HAPPENED;

  // stop previous TimeConstraints
  for (auto& timeConstraint : previous_time_constraints())
  {
    timeConstraint->stop();
  }

  // setup next TimeConstraints
  for (auto& timeConstraint : next_time_constraints())
  {
    timeConstraint->start();
  }

  if (m_callback)
    (m_callback)(m_status);
}

void time_event::dispose()
{
  if (m_status == time_event::status::HAPPENED)
  {
    throw execution_error("time_event::dispose: "
                          "HAPPENED event cannot be disposed");
    return;
  }

  m_status = time_event::status::DISPOSED;

  // stop previous TimeConstraints
  for (auto& timeConstraint : previous_time_constraints())
  {
    timeConstraint->stop();
  }

  // dispose next TimeConstraints end event if everything is disposed before
  for (auto& nextTimeConstraint : next_time_constraints())
  {
    bool dispose = true;

    for (auto& previousTimeConstraint : nextTimeConstraint->get_end_event().previous_time_constraints())
    {
      if (previousTimeConstraint->get_start_event().get_status()
          != time_event::status::DISPOSED)
      {
        dispose = false;
        break;
      }
    }

    if (dispose)
      nextTimeConstraint->get_end_event().dispose();
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

time_node& time_event::get_time_node() const
{
  return m_timenode;
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
  m_previous_time_constraints.clear();
  m_next_time_constraints.clear();
}
}
