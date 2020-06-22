#include <ossia/editor/scenario/scenario.hpp>
#include <ossia/editor/scenario/time_sync.hpp>
#include <ossia/editor/scenario/time_event.hpp>
#include <ossia/editor/scenario/time_interval.hpp>
#include <ossia/editor/scenario/quantification.hpp>
#include <ossia/detail/algorithms.hpp>
#include <iostream>

namespace ossia
{
sync_status scenario::trigger_sync_musical(
    time_sync& sync, small_event_vec& maxReachedEvents,
    ossia::time_value tick_offset,
    const ossia::token_request& tk, bool maximalDurationReached) noexcept
{
  if (!sync.m_evaluating)
  {
    sync.m_evaluating = true;
    sync.end_trigger_request();
    sync.entered_evaluation.send();
  }

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

    sync.set_is_being_triggered(true);

    if (!sync.has_trigger_date() && sync.has_sync_rate())
    {
      return quantify_time_sync(sync, tk);
    }
  }
  return sync_status::NOT_READY;
}

int is_timesync_ready(time_sync& sync, small_event_vec& pendingEvents, bool& maximalDurationReached)
{
  pendingEvents.clear();
  auto activeCount = sync.get_time_events().size();
  std::size_t pendingCount = 0;

  auto on_pending = [&](ossia::time_event* timeEvent) {
    if (!ossia::contains(pendingEvents, timeEvent))
    {
      pendingCount++;
      pendingEvents.push_back(timeEvent);
    }

    for (const std::shared_ptr<ossia::time_interval>& timeInterval :
         timeEvent->previous_time_intervals())
    {
      if (timeInterval->get_date() >= timeInterval->get_max_duration())
      {
        maximalDurationReached = true;
        break;
      }
    }
  };

  for (const std::shared_ptr<time_event>& timeEvent : sync.get_time_events())
  {
    switch (timeEvent->get_status())
    {
      // check if NONE TimeEvent is ready to become PENDING
      case time_event::status::NONE:
      {
        bool minimalDurationReached = true;

        for (const std::shared_ptr<ossia::time_interval>& timeInterval :
             timeEvent->previous_time_intervals())
        {
          const auto& ev = timeInterval->get_start_event();
          // previous TimeIntervals with a DISPOSED start event are ignored
          // as well as those in NONE (that is used in cyclic graphs : they haven't executed
          // yet so that's ok
          if (ev.get_status() == time_event::status::DISPOSED
           || ev.get_status() == time_event::status::NONE)
          {
            continue;
          }

          // previous TimeInterval with a not yet HAPPENED start event
          // can't have reached its minimal duration
          if (ev.get_status() != time_event::status::HAPPENED)
          {
            minimalDurationReached = false;
            break;
          }

          // previous TimeInterval which doesn't reached its minimal duration
          // force to quit
          if (timeInterval->get_date() < timeInterval->get_min_duration())
          {
            minimalDurationReached = false;
            break;
          }
        }

        // access to PENDING status once all previous TimeIntervals allow it
        if (minimalDurationReached)
        {
          timeEvent->set_status(time_event::status::PENDING);
          on_pending(timeEvent.get());
        }
        break;
      }

      // PENDING TimeEvent is ready for evaluation
      case time_event::status::PENDING:
        on_pending(timeEvent.get());
        break;

      case time_event::status::HAPPENED:
        break;

      case time_event::status::DISPOSED:
        activeCount--;
        break;
    }
  }

  return pendingCount == activeCount;
}

sync_status scenario::process_this_musical(
    time_sync& sync, small_event_vec& pendingEvents, small_event_vec& maxReachedEvents,
    ossia::time_value tick_offset,
    const ossia::token_request& req) noexcept
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

  return trigger_sync_musical(sync, maxReachedEvents, tick_offset, req, maximalDurationReached);
}
}
