// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/detail/algorithms.hpp>
#include <ossia/editor/scenario/time_sync.hpp>
#include <ossia/editor/loop/loop.hpp>
#include <ossia/editor/scenario/scenario.hpp>
#include <ossia/editor/state/state_element.hpp>
#include <ossia/dataflow/graph.hpp>
namespace ossia
{
loop::loop(time_value patternDuration,
    time_interval::exec_callback patternIntervalCallback,
    time_event::exec_callback patternStartEventCallback,
    time_event::exec_callback patternEndEventCallback,
    std::shared_ptr<ossia::graph> graph
           )
    : m_startCallback(std::move(patternStartEventCallback))
    , m_endCallback(std::move(patternEndEventCallback))
    , m_intervalCallback(std::move(patternIntervalCallback))
    , m_graph{graph}
{
  m_startNode = std::make_shared<time_sync>();
  m_startNode->emplace(
      m_startNode->get_time_events().begin(),
      [&](time_event::status result) { start_event_callback(result); });

  m_endNode = std::make_shared<time_sync>();
  m_endNode->emplace(
      m_endNode->get_time_events().begin(),
      [&](time_event::status result) { end_event_callback(result); });

  // create a pattern TimeInterval with all durations equal by default
  m_interval = time_interval::create(
      [=](double position, time_value date,
          const ossia::state_element& state) {
        return interval_callback(position, date, state);
      },
      *m_startNode->get_time_events()[0], *m_endNode->get_time_events()[0],
      patternDuration, patternDuration, patternDuration);

  node = std::make_shared<scenario_node>();
}

loop::~loop()
{
  m_startNode->cleanup();
  m_endNode->cleanup();
}

state_element loop::offset(ossia::time_value offset, double pos)
{
  time_value patternOffset{
      std::fmod((double)offset, (double)m_interval->get_nominal_duration())};
  m_interval->offset(patternOffset);
    /*
    std::cerr << "Offset: " << offset << std::endl;
   // reset internal mOffsetState
    m_offsetState.clear();
    flatten_and_filter(
        m_offsetState, m_interval->get_start_event().get_state());
  m_offsetState.clear();

  time_value patternOffset{
      std::fmod((double)offset, (double)m_interval->get_nominal_duration())};
  flatten_and_filter(m_offsetState, m_interval->offset(patternOffset));
  // compile mOffsetState with all HAPPENED event's states
  if (unmuted())
  {
    auto status = m_interval->get_start_event().get_status();
    if (status == time_event::status::HAPPENED)
    {
      flatten_and_filter(
          m_offsetState, m_interval->get_start_event().get_state());
    }
  }
  */

  // TODO why is mOffsetState different from mCurrentState
  return {};
}

state_element loop::state(ossia::time_value date, double pos, ossia::time_value tick_offset)
{
  // if date hasn't been processed already
  if (date != m_lastDate)
  {
    auto prev_last_date = m_lastDate;

    m_lastDate = date;


    // reset internal State
    m_currentState.clear();

    // process the loop from the pattern start TimeSync
    std::vector<time_event*> statusChangedEvents;

    if(unmuted())
    {
        m_startNode->process(statusChangedEvents, m_currentState);
    }
    else
    {
        ossia::state st;
        m_startNode->process(statusChangedEvents, st);
    }
    // make time flow for the pattern interval
    // don't tick if the pattern interval is starting to avoid double
    // ticks
    auto& startEvent = m_interval->get_start_event();
    bool not_starting = none_of(statusChangedEvents, [&](time_event* ev) {
      return ev->get_status() == time_event::status::HAPPENED
             && ev == &startEvent;
    });

    if (not_starting && startEvent.get_status() == time_event::status::HAPPENED)
    {
      // no such event found : not starting
      if (prev_last_date == Infinite)
        m_interval->tick(date);
      else
      {
        m_interval->tick(ossia::time_value{(date - prev_last_date)});
      }
    }
    else
    {
      // TODO we should advance the loop a bit at least.
    }

    flatten_and_filter(
                m_currentState,
                m_interval->state());
  }

  // if the pattern end event happened : stop and reset the loop
  if (m_interval->get_end_event().get_status()
      == time_event::status::HAPPENED)
    stop();

  //! \see mCurrentState is filled below in
  //! loop::PatternIntervalCallback
  if (unmuted())
    return m_currentState;
  return ossia::state_element{};
}

void loop::start(ossia::state& st)
{
  if(auto g = m_graph.lock()) g->enable(*node);
}

void loop::stop()
{
  if(auto g = m_graph.lock()) g->disable(*node);
  m_interval->stop();

  m_interval->offset(Zero);

  m_interval->get_start_event().set_status(time_event::status::PENDING);
  m_interval->get_end_event().set_status(time_event::status::NONE);
}

void loop::pause()
{
  m_interval->pause();
}

void loop::resume()
{
  m_interval->resume();
}

const std::shared_ptr<time_interval> loop::get_time_interval() const
{
  return m_interval;
}

const std::shared_ptr<time_sync> loop::get_start_timesync() const
{
  return m_startNode;
}

const std::shared_ptr<time_sync> loop::get_end_timesync() const
{
  return m_endNode;
}

void loop::interval_callback(
    double position, time_value date, const ossia::state_element&)
{
  if (m_intervalCallback)
  {
    // add the state of the pattern TimeInterval
    flatten_and_filter(m_currentState, m_interval->state());

    (m_intervalCallback)(position, date, m_currentState);
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
