// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/dataflow/nodes/forward_node.hpp>
#include <ossia/detail/algorithms.hpp>
#include <ossia/editor/exceptions.hpp>
#include <ossia/editor/loop/loop.hpp>
#include <ossia/editor/scenario/detail/continuity.hpp>
#include <ossia/editor/scenario/scenario.hpp>
#include <ossia/editor/scenario/time_sync.hpp>
#include <ossia/editor/state/state_element.hpp>
namespace ossia
{
loop::loop(
    time_value patternDuration,
    time_interval::exec_callback patternIntervalCallback,
    time_event::exec_callback patternStartEventCallback,
    time_event::exec_callback patternEndEventCallback)
    : m_startEvent(**m_startNode.emplace(
          m_startNode.get_time_events().begin(),
          std::move(patternStartEventCallback)))
    , m_endEvent(**m_endNode.emplace(
          m_endNode.get_time_events().begin(),
          std::move(patternEndEventCallback)))
    , m_interval{std::move(patternIntervalCallback),
                 m_startEvent,
                 m_endEvent,
                 patternDuration,
                 patternDuration,
                 patternDuration}
{
  node = std::make_shared<ossia::nodes::loop>();
  if (patternDuration <= 0)
  {
    throw std::runtime_error{"Loop duration cannot be null"};
  }
}

loop::~loop()
{
  m_startNode.cleanup();
  m_endNode.cleanup();
}

void loop::offset(ossia::time_value offset, double pos)
{
  time_value patternOffset{int64_t(
      std::fmod((double)offset, (double)m_interval.get_nominal_duration()))};
  m_interval.offset(patternOffset);
  m_lastDate = offset;
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

void loop::transport(ossia::time_value offset, double pos)
{
  time_value patternOffset{int64_t(
      std::fmod((double)offset, (double)m_interval.get_nominal_duration()))};
  m_interval.transport(patternOffset);
  m_lastDate = offset;
}

void loop::make_happen(time_event& event)
{
  event.m_status = time_event::status::HAPPENED;

  if (&event == &m_startEvent)
  {
    m_interval.start();
    mark_start_discontinuous{}(m_interval);
  }
  else if (&event == &m_endEvent)
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
  if (&event == &m_startEvent)
  {
    make_dispose(m_endEvent);
  }
  else if (&event == &m_endEvent)
  {
    m_interval.stop();
    mark_end_discontinuous{}(m_interval);
  }

  if (event.m_callback)
    (event.m_callback)(event.m_status);
}

bool loop::process_sync(
    ossia::time_sync& node, ossia::time_event& ev, bool event_pending,
    bool maximalDurationReached)
{
  if (!event_pending)
  {
    if (node.m_evaluating)
    {
      node.m_evaluating = false;
      node.trigger_request = false;
      node.left_evaluation.send();
    }

    return false;
  }

  if (!node.m_evaluating)
  {
    node.m_evaluating = true;
    node.entered_evaluation.send();
    node.trigger_request = false;
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
    node.m_status = time_sync::status::DONE_MAX_REACHED;
  else
    node.m_status = time_sync::status::DONE_TRIGGERED;

  return true;
}

void loop::state(
    ossia::time_value from, ossia::time_value date, double pos,
    ossia::time_value tick_offset, double gspeed)
{
  node->request({from, date, pos, tick_offset, gspeed});
  m_interval.set_parent_speed(gspeed);
  // if date hasn't been processed already
  // if (date != m_lastDate)
  {
    auto prev_last_date = m_lastDate;

    m_lastDate = date;

    ossia::time_value tick_amount = date - from;

    auto& start_ev = *m_startNode.get_time_events()[0];
    auto& end_ev = *m_endNode.get_time_events()[0];
    const auto& itv_dur = m_interval.get_nominal_duration();
    auto isSimpleLoop = [&] {
      return m_startNode.get_expression()
                 == ossia::expressions::expression_true()
             && start_ev.get_expression()
                    == ossia::expressions::expression_true()
             && m_endNode.get_expression()
                    == ossia::expressions::expression_true()
             && end_ev.get_expression()
                    == ossia::expressions::expression_true();
    };

    if (isSimpleLoop())
    {
      if (tick_amount >= 0)
      {
        if (m_interval.get_date() == 0)
        {
          start_ev.tick(0_tv, 0., tick_offset);
          m_interval.set_offset(0_tv);
          m_interval.start();
          m_interval.tick_current(tick_offset);
        }

        while (tick_amount > 0)
        {
          const auto cur_date = m_interval.get_date();
          if (cur_date + tick_amount < itv_dur)
          {
            m_interval.tick_offset(tick_amount, tick_offset);
            break;
          }
          else
          {
            auto this_tick = itv_dur - cur_date;

            tick_amount -= this_tick;
            m_interval.tick_offset(this_tick, tick_offset);
            tick_offset += this_tick;

            end_ev.tick(0_tv, 0., tick_offset);
            m_interval.stop();

            if (tick_amount > 0)
            {
              m_interval.offset(time_value{});
              m_interval.start();
              m_interval.set_offset(0_tv);
              m_interval.tick_current(tick_offset);
              start_ev.tick(0_tv, 0., tick_offset);
            }
          }
        }
      }
      else
      {
        while (tick_amount < 0)
        {
          const auto cur_date = m_interval.get_date();
          if (cur_date + tick_amount >= Zero)
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

            end_ev.tick(0_tv, 0., tick_offset);
            // m_interval.stop();

            if (tick_amount < 0)
            {
              m_interval.offset(itv_dur);
              // m_interval.start();
              start_ev.tick(0_tv, 0., tick_offset);
            }
          }
        }
      }
    }
    else
    {
      switch (m_startEvent.get_status())
      {
        case time_event::status::NONE:
        case time_event::status::PENDING:
        {
          process_sync(m_startNode, m_startEvent, true, false);
          if (m_startEvent.get_status() != time_event::status::HAPPENED)
          {
            m_startEvent.set_status(time_event::status::PENDING);
            m_endEvent.set_status(time_event::status::NONE);
            return;
          }
          break;
        }
        case time_event::status::HAPPENED:
          break;
        case time_event::status::DISPOSED:
          m_startEvent.set_status(time_event::status::PENDING);
          m_endEvent.set_status(time_event::status::NONE);
          return;
      }

      if (prev_last_date == Infinite)
        m_interval.tick_offset(date, tick_offset);
      else
        m_interval.tick_offset(tick_amount, tick_offset);

      switch (m_endEvent.get_status())
      {
        case time_event::status::NONE:
        {
          if (m_interval.get_date() >= m_interval.get_min_duration())
          {
            m_endEvent.set_status(time_event::status::PENDING);
            process_sync(
                m_endNode, m_endEvent, true,
                m_interval.get_date() >= m_interval.get_max_duration());
          }
          break;
        }

        case time_event::status::PENDING:
        {
          process_sync(
              m_endNode, m_endEvent, true,
              m_interval.get_date() >= m_interval.get_max_duration());
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
  if (m_lastDate == ossia::Infinite)
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

  m_lastDate = ossia::Zero;
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
}
