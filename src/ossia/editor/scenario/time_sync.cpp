// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/detail/algorithms.hpp>
#include <ossia/detail/logger.hpp>
#include <ossia/editor/scenario/time_interval.hpp>
#include <ossia/editor/scenario/time_sync.hpp>

#include <csignal>

namespace ossia
{

time_sync_callback::~time_sync_callback() = default;
void time_sync_callback::triggered() { }
void time_sync_callback::entered_evaluation() { }
void time_sync_callback::entered_triggering() { }
void time_sync_callback::trigger_date_fixed(ossia::time_value) { }
void time_sync_callback::left_evaluation() { }
void time_sync_callback::finished_evaluation(bool) { }

time_sync::time_sync()
    : m_expression(expressions::make_expression_true())
    , m_status{status::NOT_DONE}
    , m_start{}
    , m_observe{}
    , m_evaluating{}
    , m_muted{}
    , m_autotrigger{}
    , m_is_being_triggered{}
{
  m_timeEvents.reserve(4);
}

time_sync::~time_sync()
{
  callbacks.clear();
}

time_value time_sync::get_date() const noexcept
{
  // compute the date from each first previous time interval
  // ignoring zero duration time interval
  if(!get_time_events().empty())
  {
    for(auto& timeEvent : get_time_events())
    {
      auto& prev = timeEvent->previous_time_intervals();
      if(!prev.empty())
      {
        auto& prev_itv = *prev[0];
        if(!prev_itv.graphal)
          return prev_itv.get_nominal_duration()
                 + prev_itv.get_start_event().get_time_sync().get_date();
      }
    }
  }

  return Zero;
}

const expression& time_sync::get_expression() const noexcept
{
  return *m_expression;
}

time_sync& time_sync::set_expression(expression_ptr exp) noexcept
{
  assert(exp);
  m_expression = std::move(exp);
  return *this;
}

time_sync::iterator
time_sync::insert(time_sync::const_iterator pos, std::shared_ptr<time_event> ev)
{
  if(m_muted)
    ev->mute(true);
  return m_timeEvents.insert(pos, std::move(ev));
}

void time_sync::remove(const std::shared_ptr<time_event>& e)
{
  remove_one(m_timeEvents, e);
}

time_sync::iterator time_sync::emplace(
    const_iterator pos, time_event::exec_callback callback, ossia::expression_ptr exp)
{
  return insert(pos, std::make_shared<time_event>(callback, *this, std::move(exp)));
}

bool time_sync::is_evaluating() const noexcept
{
  return m_evaluating;
}

void time_sync::start_trigger_request() noexcept
{
  if(!trigger_request)
  {
    trigger_request = true;
  }
}

void time_sync::end_trigger_request() noexcept
{
  trigger_request = false;
}

bool time_sync::is_autotrigger() const noexcept
{
  return m_autotrigger;
}

void time_sync::set_autotrigger(bool a) noexcept
{
  m_autotrigger = a;
}

bool time_sync::is_start() const noexcept
{
  return m_start;
}

void time_sync::set_start(bool a) noexcept
{
  m_start = a;
}

bool time_sync::is_observing_expression() const noexcept
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
  if(!m_expression || m_expression->target<ossia::expressions::expression_bool>())
    return;

  if(observe != m_observe)
  {
    bool wasObserving = m_observe;
    m_observe = observe;

    if(m_observe)
    {
      if(m_callback)
      {
        ossia::logger().error("Warning: time_sync can only have one callback\n");
        expressions::remove_callback(*m_expression, *m_callback);
      }
      m_callback = expressions::add_callback(*m_expression, cb);
    }
    else
    {
      // stop expression observation
      if(wasObserving && m_callback)
      {
        expressions::remove_callback(*m_expression, *m_callback);
        m_callback = std::nullopt;
      }
    }
  }
}

void time_sync::reset()
{
  if(m_expression)
    expressions::reset(*m_expression);

  for(auto& timeEvent : m_timeEvents)
  {
    timeEvent->reset();
  }

  m_trigger_date = Infinite;
  m_status = status::NOT_DONE;
  m_observe = false;
  m_evaluating = false;
  m_is_being_triggered = false;
}

void time_sync::cleanup()
{
  for(auto& timeevent : m_timeEvents)
    timeevent->cleanup();

  m_timeEvents.clear();
  callbacks.clear();
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

void time_sync::set_is_being_triggered(bool v) noexcept
{
  if(m_is_being_triggered != v)
  {
    m_is_being_triggered = v;
    if(v)
    {
      callbacks.entered_triggering();
    }
  }
  if(!v)
  {
    m_trigger_date = Infinite;
    trigger_request = false;
  }
}
}
