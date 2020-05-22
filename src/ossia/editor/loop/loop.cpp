// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/dataflow/nodes/forward_node.hpp>
#include <ossia/detail/algorithms.hpp>
#include <ossia/editor/exceptions.hpp>
#include <ossia/editor/loop/loop.hpp>
#include <ossia/editor/scenario/detail/continuity.hpp>
#include <ossia/editor/scenario/scenario.hpp>
#include <ossia/editor/scenario/time_sync.hpp>
#include <ossia/editor/scenario/quantification.hpp>
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
  if (patternDuration <= 0_tv)
  {
    throw std::runtime_error{"Loop duration cannot be null"};
  }
}

loop::~loop()
{
  m_startNode.cleanup();
  m_endNode.cleanup();
}

void loop::offset_impl(ossia::time_value offset)
{
  time_value patternOffset{int64_t(
      std::fmod((double)offset.impl, (double)m_interval.get_nominal_duration().impl))};
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

void loop::transport_impl(ossia::time_value offset)
{
  time_value patternOffset{int64_t(
      std::fmod((double)offset.impl, (double)m_interval.get_nominal_duration().impl))};
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

void loop::mute_impl(bool m)
{
  m_startNode.mute(m);
  m_endNode.mute(m);
  m_interval.mute(m);
}
sync_status loop::trigger_quantified_time_sync(time_sync& sync, bool& maximalDurationReached) noexcept
{
  if(&sync == &this->m_startNode)
  {
    return sync_status::DONE;
  }
  else
  {
    if (m_startEvent.get_status() != ossia::time_event::status::HAPPENED)
      return sync_status::NOT_READY;

    if(!m_sync_date)
      return sync_status::NOT_READY;

    if(m_interval.get_date() == *m_sync_date)
      return sync_status::NOT_READY;

    maximalDurationReached = true;

    m_sync_date = std::nullopt;

    return sync_status::DONE;
  }
}

sync_status loop::quantify_time_sync(time_sync& sync, const ossia::token_request& tk) noexcept
{
  // we are asked to execute, now we must quantize to the next step
  auto qdate = tk.get_quantification_date(sync.get_sync_rate());

  if(qdate)
  {
    // compute the "fake max" date at which intervals must end for this to
    // work
    sync.set_trigger_date(*qdate);
    auto diff_date = *qdate - tk.prev_date - 1_tv;

    m_sync_date = /*m_interval.get_date() + */ diff_date;

    sync.observe_expression(false);
    return sync_status::RETRY;
  }

  return sync_status::NOT_READY;
}


ossia::sync_status loop::process_sync(
    ossia::time_sync& sync, const ossia::token_request& tk,
    ossia::time_event& ev, bool event_pending,
    bool maximalDurationReached)
{
  if (!event_pending)
  {
    if (sync.m_evaluating)
    {
      sync.m_evaluating = false;
      sync.end_trigger_request();
      sync.left_evaluation.send();
    }

    return sync_status::NOT_READY;
  }

  if (!sync.m_evaluating)
  {
    sync.m_evaluating = true;
    sync.entered_evaluation.send();
    sync.end_trigger_request();
  }

  // update the expression one time
  // then observe and evaluate TimeSync's expression before to trig
  // only if no maximal duration have been reached
  if (*sync.m_expression != expressions::expression_true()
      && !maximalDurationReached)
  {
    if (!sync.has_trigger_date() && !sync.is_being_triggered())
    {
      if (!sync.is_observing_expression())
        expressions::update(*sync.m_expression);

      sync.observe_expression(true);

      if (sync.trigger_request)
        sync.end_trigger_request();
      else if (!expressions::evaluate(*sync.m_expression))
        return sync_status::NOT_READY;
    }

    // at this point we can assume we are going to trigger.
    sync.set_is_being_triggered(true);

    if (sync.has_trigger_date())
    {
      auto res = trigger_quantified_time_sync(sync, maximalDurationReached);
      if(res != sync_status::DONE)
        return res;
    }
    else if (sync.has_sync_rate())
    {
      return quantify_time_sync(sync, tk);
    }
  }

  sync.set_is_being_triggered(false);

  // now TimeEvents will happen or be disposed
  auto& expr = ev.get_expression();
  // update any Destination value into the expression
  expressions::update(expr);

  if (expressions::evaluate(expr))
    make_happen(ev);
  else
    make_dispose(ev);

  // stop expression observation now the TimeSync has been processed
  sync.observe_expression(false);

  // notify observers
  sync.triggered.send();

  sync.m_evaluating = false;
  sync.finished_evaluation.send(maximalDurationReached);

  if (maximalDurationReached)
    sync.m_status = time_sync::status::DONE_MAX_REACHED;
  else
    sync.m_status = time_sync::status::DONE_TRIGGERED;

  return sync_status::DONE;
}

void loop::simple_tick(
    ossia::token_request& req,
    time_value tick_amount,
    const time_value& itv_dur)
{
  if (tick_amount >= 0_tv)
  {
    if (m_interval.get_date() == 0_tv)
    {
      m_startEvent.tick(0_tv, req.offset);
      m_interval.set_offset(0_tv);
      m_interval.start();
      m_interval.tick_current(req.offset, req);
    }

    while (tick_amount > 0_tv)
    {
      // TODO this is not stateless ! we should compute
      // it from "from"
      const auto cur_date = m_interval.get_date();
      if (cur_date + tick_amount < itv_dur)
      {
        m_interval.tick_offset(tick_amount, req.offset, req);
        break;
      }
      else
      {
        auto this_tick = itv_dur - cur_date;

        tick_amount -= this_tick;
        m_interval.tick_offset(this_tick, req.offset, req);
        req.offset += this_tick;

        m_endEvent.tick(0_tv, req.offset);
        m_interval.stop();

        if (tick_amount > 0_tv)
        {
          m_interval.offset(time_value{});
          m_interval.start();
          m_interval.set_offset(0_tv);
          m_interval.tick_current(req.offset, req);
          m_startEvent.tick(0_tv, req.offset);
        }
      }
    }
  }
  else
  {
    while (tick_amount < 0_tv)
    {
      const auto cur_date = m_interval.get_date();
      if (cur_date + tick_amount >= Zero)
      {
        m_interval.tick_offset(tick_amount, req.offset, req);
        break;
      }
      else
      {
        const auto& this_tick = cur_date;

        tick_amount += this_tick;
        m_interval.tick_offset(this_tick, req.offset, req);
        req.offset += this_tick;

        m_endEvent.tick(0_tv, req.offset);
        // m_interval.stop();

        if (tick_amount < 0_tv)
        {
          m_interval.offset(itv_dur);
          // m_interval.start();
          m_startEvent.tick(0_tv, req.offset);
        }
      }
    }
  }
}

void loop::general_tick(
    const ossia::token_request& req,
    const ossia::time_value prev_last_date,
    ossia::time_value tick_amount
    )
{
  while(tick_amount > 0_tv)
  {
    // First check the start event
    switch (m_startEvent.get_status())
    {
      case time_event::status::NONE:
      case time_event::status::PENDING:
      {
        process_sync(m_startNode, req, m_startEvent, true, false);
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

    // If the end sync is quantized, we must adapt the tick duration.
    if(m_endNode.get_sync_rate() > 0.)
    {
      if(m_endEvent.get_status() == time_event::status::PENDING)
      {

        if (*m_endNode.m_expression != expressions::expression_true()
            && !(m_interval.get_date() >= m_interval.get_max_duration()))
        {
          if (!m_endNode.has_trigger_date() && !m_endNode.is_being_triggered())
          {
            if (!m_endNode.is_observing_expression())
              expressions::update(*m_endNode.m_expression);

            m_endNode.observe_expression(true);

            if (m_endNode.trigger_request)
              m_endNode.end_trigger_request();
            else if (!expressions::evaluate(*m_endNode.m_expression))
              goto continue_running;
          }
          m_endNode.set_is_being_triggered(true);

          // TODO what if sync_date > interval's max date

          if (!m_endNode.has_trigger_date() && m_endNode.has_sync_rate())
          {
            auto res = quantify_time_sync(m_endNode, req);
            if(res == sync_status::RETRY)
            {
              m_interval.tick_offset(*m_sync_date, req.offset, req);
              tick_amount -= *m_sync_date;
              m_sync_date = std::nullopt;

              process_sync(m_endNode, req, m_endEvent, true, true);

              stop();
              start();
              continue;
            }
          }
        }
      }
    }
continue_running:
      // Run the interval.
    if (prev_last_date == Infinite)
      m_interval.tick_offset(req.date, req.offset, req);
    else
      m_interval.tick_offset(tick_amount, req.offset, req);

    tick_amount = 0_tv;

    // Check if the end timesync can be triggered
    switch (m_endEvent.get_status())
    {
      case time_event::status::NONE:
      {
        if (m_interval.get_date() >= m_interval.get_min_duration())
        {
          m_endEvent.set_status(time_event::status::PENDING);
          process_sync(
                m_endNode, req, m_endEvent, true,
                m_interval.get_date() >= m_interval.get_max_duration());
        }
        break;
      }

      case time_event::status::PENDING:
      {
        process_sync(
              m_endNode, req, m_endEvent, true,
              m_interval.get_date() >= m_interval.get_max_duration());
        break;
      }
      case time_event::status::HAPPENED:
      case time_event::status::DISPOSED:
        process_sync(m_endNode, req, m_endEvent, false, false);
        break;
    }

    // We are done, put us back in the initial state
    if (m_endEvent.get_status() == time_event::status::HAPPENED)
    {
      stop();
    }
  }
}

bool loop::is_simple() const noexcept
{
  return m_startNode.get_expression()
             == ossia::expressions::expression_true()
         && m_startEvent.get_expression()
                == ossia::expressions::expression_true()
         && m_endNode.get_expression()
                == ossia::expressions::expression_true()
         && m_endEvent.get_expression()
                == ossia::expressions::expression_true();
}

void loop::state_impl(ossia::token_request req)
{
  node->request(req);
  m_interval.set_parent_speed(req.speed);
  // if date hasn't been processed already
  // if (date != m_lastDate)
  {
    auto prev_last_date = m_lastDate;

    m_lastDate = req.date;

    ossia::time_value tick_amount = req.date - req.prev_date;

    if (is_simple())
    {
      simple_tick(req, tick_amount, m_interval.get_nominal_duration());
    }
    else
    {
      general_tick(req, prev_last_date, tick_amount);
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
