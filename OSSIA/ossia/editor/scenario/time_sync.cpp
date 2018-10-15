// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/editor/scenario/time_interval.hpp>
#include <ossia/editor/scenario/time_sync.hpp>

namespace ossia
{

time_sync::time_sync() : m_expression(expressions::make_expression_true())
{
}

time_sync::~time_sync() = default;

time_value time_sync::get_date() const
{
  // compute the date from each first previous time interval
  // ignoring zero duration time interval
  if (!get_time_events().empty())
  {
    for (auto& timeEvent : get_time_events())
    {
      if (!timeEvent->previous_time_intervals().empty())
      {
        if (timeEvent->previous_time_intervals()[0]->get_nominal_duration()
            > Zero)
          return timeEvent->previous_time_intervals()[0]
                     ->get_nominal_duration()
                 + timeEvent->previous_time_intervals()[0]
                       ->get_start_event()
                       .get_time_sync()
                       .get_date();
      }
    }
  }

  return Zero;
}

const expression& time_sync::get_expression() const
{
  return *m_expression;
}

time_sync& time_sync::set_expression(expression_ptr exp)
{
  assert(exp);
  m_expression = std::move(exp);
  return *this;
}

time_sync::iterator time_sync::insert(
    time_sync::const_iterator pos, std::shared_ptr<time_event> ev)
{
  if(m_muted) ev->mute(true);
  return m_timeEvents.insert(pos, std::move(ev));
}

void time_sync::remove(const std::shared_ptr<time_event>& e)
{
  remove_one(m_timeEvents, e);
}

time_sync::iterator time_sync::emplace(
    const_iterator pos, time_event::exec_callback callback,
    ossia::expression_ptr exp)
{
  return insert(
      pos, std::make_shared<time_event>(callback, *this, std::move(exp)));
}

bool time_sync::is_evaluating() const
{
  return m_evaluating;
}

bool time_sync::is_observing_expression() const
{
  return m_observe;
}

void time_sync::observe_expression(bool observe)
{
  // start expression observation; dummy callback used.
  // Do not remove it : else the expressions will stop listening.
  return observe_expression(observe, [](bool) {});
}

void time_sync::observe_expression(
    bool observe, ossia::expressions::expression_result_callback cb)
{
  if (!m_expression || *m_expression == expressions::expression_true()
      || *m_expression == expressions::expression_false())
    return;

  if (observe != m_observe)
  {
    bool wasObserving = m_observe;
    m_observe = observe;

    if (m_observe)
    {
      m_callback = expressions::add_callback(*m_expression, cb);
    }
    else
    {
      // stop expression observation
      if (wasObserving && m_callback)
      {
        expressions::remove_callback(*m_expression, *m_callback);
        m_callback = ossia::none;
      }
    }
  }
}

void time_sync::reset()
{
  for (auto& timeEvent : m_timeEvents)
  {
    timeEvent->reset();
  }

  m_trigger_date = Infinite;
  m_observe = false;
  m_evaluating = false;
}

void time_sync::cleanup()
{
  for (auto& timeevent : m_timeEvents)
    timeevent->cleanup();

  m_timeEvents.clear();
  triggered.callbacks_clear();
  entered_evaluation.callbacks_clear();
  left_evaluation.callbacks_clear();
  finished_evaluation.callbacks_clear();
}

void time_sync::mute(bool m)
{
  if(m != m_muted)
  {
    m_muted = m;
    for(const auto& e : get_time_events())
      e->mute(m);
  }
}
}
