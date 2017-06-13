#include <ossia/editor/loop/loop.hpp>

#include <ossia/editor/scenario/time_node.hpp>
#include <ossia/detail/algorithms.hpp>

namespace ossia
{
loop::loop(
    time_value patternDuration,
    time_constraint::exec_callback patternConstraintCallback,
    time_event::exec_callback patternStartEventCallback,
    time_event::exec_callback patternEndEventCallback)
    : m_startCallback(std::move(patternStartEventCallback))
    , m_endCallback(std::move(patternEndEventCallback))
    , m_constraintCallback(std::move(patternConstraintCallback))
{
  m_startNode = std::make_shared<time_node>();
  m_startNode->emplace(
      m_startNode->get_time_events().begin(),
      [&](time_event::status result) { start_event_callback(result); });

  m_endNode = std::make_shared<time_node>();
  m_endNode->emplace(
      m_endNode->get_time_events().begin(),
      [&](time_event::status result) { end_event_callback(result); });

  // create a pattern TimeConstraint with all durations equal by default
  m_constraint = time_constraint::create(
      [=](ossia::time_value position, time_value date, const ossia::state_element& state) {
        return constraint_callback(position, date, state);
      },
      *m_startNode->get_time_events()[0], *m_endNode->get_time_events()[0],
      patternDuration, patternDuration, patternDuration);

  // set pattern TimeConstraint's clock in external mode
  m_constraint->set_drive_mode(clock::drive_mode::EXTERNAL);
}

loop::~loop()
{
  m_startNode->cleanup();
  m_endNode->cleanup();
}

state_element loop::offset(ossia::time_value offset, double pos)
{
  // reset internal mOffsetState
  m_offsetState.clear();

  time_value patternOffset{std::fmod(
      (double)offset, (double)m_constraint->get_nominal_duration())};
  flatten_and_filter(m_offsetState, m_constraint->offset(patternOffset));

  // compile mOffsetState with all HAPPENED event's states
  if(unmuted())
  {
    if (m_constraint->get_start_event().get_status()
        == time_event::status::HAPPENED)
      flatten_and_filter(
            m_offsetState, m_constraint->get_start_event().get_state());
  }

  // TODO why is mOffsetState different from mCurrentState
  return m_offsetState;
}

state_element loop::state(ossia::time_value date, double pos)
{
  // if date hasn't been processed already
  if (date != m_lastDate)
  {
    auto prev_last_date = m_lastDate;
    m_lastDate = date;

    // reset internal State
    m_currentState.clear();

    // process the loop from the pattern start TimeNode
    std::vector<time_event*> statusChangedEvents;
    m_startNode->process(statusChangedEvents);

    // add the state of each newly HAPPENED TimeEvent
    if(unmuted())
    {
      for (const auto& timeEvent : statusChangedEvents)
        if (timeEvent->get_status() == time_event::status::HAPPENED)
          flatten_and_filter(m_currentState, timeEvent->get_state());
    }

    // make time flow for the pattern constraint
    if (m_constraint->running())
    {
      if (m_constraint->get_drive_mode() != clock::drive_mode::EXTERNAL)
      {
        throw execution_error("loop::state: "
            "the pattern constraint clock is supposed to "
            "be in EXTERNAL drive mode");
        return {};
      }

      if (m_constraint->running())
      {
        // don't tick if the pattern constraint is starting to avoid double
        // ticks
        auto& startEvent = m_constraint->get_start_event();
        bool not_starting = none_of(
            statusChangedEvents, [&](time_event* ev) {
              return ev->get_status() == time_event::status::HAPPENED
                     && ev == &startEvent;
            });

        if (not_starting)
        {
          // no such event found : not starting
          // no such event found : not starting
          if (prev_last_date == Infinite)
            m_constraint->tick();
          else
            m_constraint->tick(ossia::time_value{(date - prev_last_date) * 1000.});
        }
        else
        {
          // TODO we should advance the loop a bit at least.
        }
      }

      // if the pattern constraint is still running after the tick
      if (m_constraint->running())
        flatten_and_filter(m_currentState, m_constraint->state());
    }

    // if the pattern end event happened : stop and reset the loop
    if (m_constraint->get_end_event().get_status()
        == time_event::status::HAPPENED)
      stop();
  }

  //! \see mCurrentState is filled below in
  //! loop::PatternConstraintCallback
  if(unmuted())
    return m_currentState;
  return ossia::state_element{};
}

void loop::start()
{
}

void loop::stop()
{
  m_constraint->stop();

  m_constraint->offset(Zero);

  m_constraint->get_start_event().set_status(time_event::status::PENDING);
  m_constraint->get_end_event().set_status(time_event::status::NONE);
}

void loop::pause()
{
  m_constraint->pause();
}

void loop::resume()
{
  m_constraint->resume();
}

const std::shared_ptr<time_constraint>
loop::get_time_constraint() const
{
  return m_constraint;
}

const std::shared_ptr<time_node> loop::get_start_timenode() const
{
  return m_startNode;
}

const std::shared_ptr<time_node> loop::get_end_timenode() const
{
  return m_endNode;
}

void loop::constraint_callback(
    time_value position, time_value date, const ossia::state_element&)
{
  if (m_constraintCallback)
  {
    // add the state of the pattern TimeConstraint
    flatten_and_filter(m_currentState, m_constraint->state());

    (m_constraintCallback)(position, date, m_currentState);
  }
}

void loop::start_event_callback(time_event::status newStatus)
{
  if (m_startCallback)
    (m_startCallback)(newStatus);
}

void loop::end_event_callback(time_event::status newStatus)
{
  if (m_endCallback)
    (m_endCallback)(newStatus);
}
}
