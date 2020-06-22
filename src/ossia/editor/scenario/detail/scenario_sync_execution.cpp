#include <ossia/editor/scenario/scenario.hpp>
#include <ossia/editor/scenario/time_sync.hpp>
#include <ossia/editor/scenario/time_event.hpp>
#include <ossia/editor/scenario/time_interval.hpp>
#include <ossia/editor/scenario/quantification.hpp>
#include <ossia/detail/algorithms.hpp>
#include <iostream>
namespace ossia
{


sync_status scenario::quantify_time_sync(time_sync& sync, const ossia::token_request& tk) noexcept
{
  // we are asked to execute, now we must quantize to the next step
  auto qdate = tk.get_quantification_date(sync.get_sync_rate());

  if(qdate)
  {
    // compute the "fake max" date at which intervals must end for this to
    // work
    sync.set_trigger_date(*qdate);
    auto diff_date = *qdate - tk.prev_date - 1_tv;

    for (const std::shared_ptr<time_event>& ev : sync.get_time_events())
    {
      for (const auto& cst : ev->previous_time_intervals())
      {
        m_itv_end_map.insert(
            std::make_pair(cst.get(), cst->get_date() + diff_date));
      }
    }
    sync.observe_expression(false);
    return sync_status::RETRY;
  }

  return sync_status::NOT_READY;
}


sync_status scenario::trigger_quantified_time_sync(time_sync& sync, bool& maximalDurationReached) noexcept
{
  bool ok = true;
  for (const auto& ev : sync.get_time_events())
  {
    for (const auto& cst : ev->previous_time_intervals())
    {
      if (cst->get_start_event().get_status()
          == ossia::time_event::status::HAPPENED)
      {
        auto cst_it = m_itv_end_map.find(cst.get());

        // if we add an interval at execution after the time sync has been triggered,
        // it is ignored
        if (cst_it != m_itv_end_map.end())
          ok &= (cst->get_date() == cst_it->second);
      }
      if (!ok)
        break;
    }
    if (!ok)
      break;
  }

  if (!ok)
  {
    return sync_status::NOT_READY;
  }
  else
  {
    maximalDurationReached = true;

    for (const auto& ev : sync.get_time_events())
    {
      for (const auto& cst : ev->previous_time_intervals())
      {
        m_itv_end_map.erase(cst.get());
      }
    }

    return sync_status::DONE;
  }
}

sync_status scenario::trigger_sync(
    time_sync& sync, small_event_vec& pending, small_event_vec& maxReachedEv,
    interval_set& started, interval_set& stopped,
    ossia::time_value tick_offset, const ossia::token_request& tk, bool maximalDurationReached)
{
  if (!sync.m_evaluating)
  {
    sync.m_evaluating = true;
    sync.end_trigger_request();
    sync.entered_evaluation.send();
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

  // trigger the time sync
  sync.set_is_being_triggered(false);

  // now TimeEvents will happen or be disposed.
  // the last added events are necessarily the ones of this node.
  for (time_event* ev : pending)
  {
    auto& expr = ev->get_expression();
    // update any Destination value into the expression
    expressions::update(expr);

    if (expressions::evaluate(expr))
    {
      make_happen(*ev, started, stopped, tick_offset, tk);
      if (maximalDurationReached)
        maxReachedEv.push_back(ev);
    }
    else
    {
      make_dispose(*ev, stopped);
    }
  }

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

int is_timesync_ready(time_sync& sync, small_event_vec& pendingEvents, bool& maximalDurationReached);
sync_status scenario::process_this(
    time_sync& sync, small_event_vec& pendingEvents,
    small_event_vec& maxReachedEvents, interval_set& started,
    interval_set& stopped, ossia::time_value tick_offset,
    const ossia::token_request& req)
{
  // prepare to remember which event changed its status to PENDING
  // because it is needed in time_sync::trigger

  bool maximalDurationReached = false;
  // if all TimeEvents are not PENDING
  if (!is_timesync_ready(sync, pendingEvents, maximalDurationReached))
  {
    if (sync.m_evaluating)
    {
      sync.m_evaluating = false;
      sync.end_trigger_request();
      sync.left_evaluation.send();
    }

    return sync_status::NOT_READY;
  }

  return trigger_sync(
      sync, pendingEvents, maxReachedEvents, started, stopped, tick_offset, req,
      maximalDurationReached);
}
}
