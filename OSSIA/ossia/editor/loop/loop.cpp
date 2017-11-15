// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/detail/algorithms.hpp>
#include <ossia/editor/scenario/time_sync.hpp>
#include <ossia/editor/loop/loop.hpp>
#include <ossia/editor/scenario/scenario.hpp>
#include <ossia/editor/state/state_element.hpp>
#include <ossia/editor/scenario/detail/continuity.hpp>
#include <ossia/dataflow/graph.hpp>
namespace ossia
{
class loop_node : public ossia::graph_node
{
  public:
    loop_node();
    void run(ossia::token_request t, ossia::execution_state&) override;
};

loop_node::loop_node()
{
  // todo maybe we can optimize by having m_outlets == m_inlets
  // this way no copy.
  m_inlets.push_back(ossia::make_inlet<ossia::audio_port>());
  m_outlets.push_back(ossia::make_outlet<ossia::audio_port>());
}


void loop_node::run(token_request t, execution_state&)
{
  auto i = m_inlets[0]->data.target<ossia::audio_port>();
  auto o = m_outlets[0]->data.target<ossia::audio_port>();
  o->samples = i->samples;
}
loop::loop(time_value patternDuration,
           time_interval::exec_callback patternIntervalCallback,
           time_event::exec_callback patternStartEventCallback,
           time_event::exec_callback patternEndEventCallback
           )
  : m_startCallback(std::move(patternStartEventCallback))
  , m_endCallback(std::move(patternEndEventCallback))
  , m_startEvent(**m_startNode.emplace(
                   m_startNode.get_time_events().begin(),
                   [&](time_event::status result) { start_event_callback(result); }))
  , m_endEvent(**m_endNode.emplace(
                   m_endNode.get_time_events().begin(),
                   [&](time_event::status result) { end_event_callback(result); }))
  , m_interval{[=](double position, ossia::time_value date) { return interval_callback(position, date); },
               m_startEvent, m_endEvent, patternDuration, patternDuration, patternDuration}
  , m_intervalCallback(std::move(patternIntervalCallback))
{
  node = std::make_shared<loop_node>();
}

loop::~loop()
{
  m_startNode.cleanup();
  m_endNode.cleanup();
}

void loop::offset(ossia::time_value offset, double pos)
{
  time_value patternOffset{
    std::fmod((double)offset, (double)m_interval.get_nominal_duration())};
  m_interval.offset(patternOffset);
  /*
    std::cerr << "Offset: " << offset << std::endl;
   // reset internal mOffsetState
    m_offsetState.clear();
    flatten_and_filter(
        m_offsetState, m_interval.get_start_event().get_state());
  m_offsetState.clear();

  time_value patternOffset{
      std::fmod((double)offset, (double)m_interval.get_nominal_duration())};
  flatten_and_filter(m_offsetState, m_interval.offset(patternOffset));
  // compile mOffsetState with all HAPPENED event's states
  if (unmuted())
  {
    auto status = m_interval.get_start_event().get_status();
    if (status == time_event::status::HAPPENED)
    {
      flatten_and_filter(
          m_offsetState, m_interval.get_start_event().get_state());
    }
  }
  */
}

void loop::make_happen(time_event& event)
{
  event.m_status = time_event::status::HAPPENED;

  if(&event == &m_startEvent)
  {
    m_interval.start();
    m_interval.tick();
    mark_start_discontinuous{}(m_interval);
  }
  else if(&event == &m_endEvent)
  {
    m_interval.stop();
    mark_end_discontinuous{}(m_interval);
  }

  if (event.m_callback)
    (event.m_callback)(event.m_status);
}

void loop::make_dispose(time_event& event)
{
  if (event.m_status == time_event::status::HAPPENED)
  {
    throw execution_error(
          "time_event::dispose: "
          "HAPPENED event cannot be disposed");
    return;
  }

  event.m_status = time_event::status::DISPOSED;
  if(&event == &m_startEvent)
  {
    make_dispose(m_endEvent);
  }
  else if(&event == &m_endEvent)
  {
    m_interval.stop();
    mark_end_discontinuous{}(m_interval);
  }

  if (event.m_callback)
    (event.m_callback)(event.m_status);
}

bool loop::process_sync(ossia::time_sync& node, ossia::time_event& ev, bool event_pending, bool maximalDurationReached)
{
  if (!event_pending)
  {
    if (node.m_evaluating)
    {
      node.m_evaluating = false;
      node.left_evaluation.send();
    }

    return false;
  }

  if (!node.m_evaluating)
  {
    node.m_evaluating = true;
    node.entered_evaluation.send();
  }

  // update the expression one time
  // then observe and evaluate TimeSync's expression before to trig
  // only if no maximal duration have been reached
  if (*node.m_expression != expressions::expression_true()
      && !maximalDurationReached)
  {
    if (!node.is_observing_expression())
      expressions::update(*node.m_expression);

    node.observe_expression(true);

    if (node.trigger_request)
      node.trigger_request = false;
    else if (!expressions::evaluate(*node.m_expression))
      return false;
  }

  // now TimeEvents will happen or be disposed
  auto& expr = ev.get_expression();
  // update any Destination value into the expression
  expressions::update(expr);

  if (expressions::evaluate(expr))
    make_happen(ev);
  else
    make_dispose(ev);

  // stop expression observation now the TimeSync has been processed
  node.observe_expression(false);

  // notify observers
  node.triggered.send();

  node.m_evaluating = false;
  node.finished_evaluation.send(maximalDurationReached);

  if (maximalDurationReached)
    node.m_status = time_sync::DONE_MAX_REACHED;
  else
    node.m_status = time_sync::DONE_TRIGGERED;

  return true;
}


void loop::state(ossia::time_value date, double pos, ossia::time_value tick_offset)
{
  node->requested_tokens.push_back({date, pos, tick_offset});
  // if date hasn't been processed already
  if (date != m_lastDate)
  {
    auto prev_last_date = m_lastDate;

    m_lastDate = date;

    ossia::time_value tick_amount = date - prev_last_date;

    auto& start_ev = *m_startNode.get_time_events()[0];
    auto& end_ev = *m_endNode.get_time_events()[0];
    const auto& itv_dur = m_interval.get_nominal_duration();
    auto isSimpleLoop = [&] {
      return m_startNode.get_expression() == ossia::expressions::expression_true() &&
          start_ev.get_expression() == ossia::expressions::expression_true() &&
          m_endNode.get_expression() == ossia::expressions::expression_true() &&
          end_ev.get_expression() == ossia::expressions::expression_true();
    };

    if(isSimpleLoop())
    {
      if(tick_amount >= 0)
      {
        while(tick_amount > 0)
        {
          const auto cur_date = m_interval.get_date();
          if(cur_date + tick_amount < itv_dur)
          {
            if(cur_date == 0)
            {
              // flatten_and_filter(m_currentState, start_ev.get_state());
              m_interval.start();
              m_interval.tick();
            }
            m_interval.tick_offset(tick_amount, tick_offset);
            break;
          }
          else
          {
            auto this_tick = itv_dur - cur_date;

            tick_amount -= this_tick;
            m_interval.tick_offset(this_tick, tick_offset);
            tick_offset += this_tick;

            // flatten_and_filter(m_currentState, end_ev.get_state());
            m_interval.stop();

            if(tick_amount > 0)
            {
              m_interval.offset(time_value{});
              m_interval.start_and_tick();
              // flatten_and_filter(m_currentState, start_ev.get_state());
            }
          }
        }
      }
      else
      {
        while(tick_amount < 0)
        {
          const auto cur_date = m_interval.get_date();
          if(cur_date + tick_amount >= Zero)
          {
            m_interval.tick_offset(tick_amount, tick_offset);
            break;
          }
          else
          {
            const auto& this_tick = cur_date;

            tick_amount += this_tick;
            m_interval.tick_offset(this_tick, tick_offset);
            tick_offset += this_tick;

            // flatten_and_filter(m_currentState, end_ev.get_state());
            //m_interval.stop();

            if(tick_amount < 0)
            {
              m_interval.offset(itv_dur);
              //m_interval.start();
              // flatten_and_filter(m_currentState, start_ev.get_state());
            }
          }
        }
      }
    }
    else
    {
      std::vector<time_event*> statusChangedEvents;
      switch(m_startEvent.get_status())
      {
        case time_event::status::NONE:
        case time_event::status::PENDING:
        {
          process_sync(m_startNode, m_startEvent, true, false);
          if(m_startEvent.get_status() != time_event::status::HAPPENED)
            return;
          break;
        }
        case time_event::status::HAPPENED:
          break;
        case time_event::status::DISPOSED:
          return;
      }

      if (prev_last_date == Infinite)
        m_interval.tick_offset(date, tick_offset);
      else
        m_interval.tick_offset(ossia::time_value{(date - prev_last_date)}, tick_offset);


      switch (m_endEvent.get_status())
      {
        case time_event::status::NONE:
        {
          if (m_interval.get_date() >= m_interval.get_min_duration())
          {
            m_endEvent.set_status(time_event::status::PENDING);
            process_sync(m_endNode, m_endEvent, true, m_interval.get_date() >= m_interval.get_max_duration());
          }
          break;
        }

        case time_event::status::PENDING:
        {
          process_sync(m_endNode, m_endEvent, true, m_interval.get_date() >= m_interval.get_max_duration());
          break;
        }
        case time_event::status::HAPPENED:
        case time_event::status::DISPOSED:
          process_sync(m_endNode, m_endEvent, false, false);
          break;
      }

      if (m_endEvent.get_status() == time_event::status::HAPPENED)
      {
        stop();
      }
    }

  }
}

void loop::start()
{
  m_lastDate = ossia::Zero;
  m_interval.get_start_event().set_status(time_event::status::PENDING);
}

void loop::stop()
{
  m_startNode.reset();
  m_endNode.reset();
  m_interval.stop();

  m_interval.offset(Zero);

  m_interval.get_start_event().set_status(time_event::status::PENDING);
  m_interval.get_end_event().set_status(time_event::status::NONE);
}

void loop::pause()
{
  m_interval.pause();
}

void loop::resume()
{
  m_interval.resume();
}

const time_interval& loop::get_time_interval() const
{
  return m_interval;
}
time_interval& loop::get_time_interval()
{
  return m_interval;
}

const time_sync& loop::get_start_timesync() const
{
  return m_startNode;
}

const time_sync& loop::get_end_timesync() const
{
  return m_endNode;
}
time_sync& loop::get_start_timesync()
{
  return m_startNode;
}

time_sync& loop::get_end_timesync()
{
  return m_endNode;
}

void loop::interval_callback(
    double position, ossia::time_value date)
{
  if (m_intervalCallback)
  {
    // add the state of the pattern TimeInterval
    //m_interval.state();
    (m_intervalCallback)(position, date);
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
