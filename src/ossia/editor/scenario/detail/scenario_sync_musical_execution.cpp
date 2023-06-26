#include <ossia/detail/algorithms.hpp>
#include <ossia/detail/logger.hpp>
#include <ossia/editor/scenario/quantification.hpp>
#include <ossia/editor/scenario/scenario.hpp>
#include <ossia/editor/scenario/time_event.hpp>
#include <ossia/editor/scenario/time_interval.hpp>
#include <ossia/editor/scenario/time_sync.hpp>

namespace ossia
{
sync_status scenario::trigger_sync_musical(
    time_sync& sync, small_event_vec& maxReachedEvents, ossia::time_value tick_offset,
    const ossia::token_request& tk, bool maximalDurationReached) noexcept
{
  if(!sync.m_evaluating)
  {
    sync.m_evaluating = true;
    sync.end_trigger_request();
    sync.callbacks.entered_evaluation();
  }

  if(sync.m_expression && !is_expression_true(*sync.m_expression)
     && !maximalDurationReached)
  {
    if(!sync.has_trigger_date() && !sync.is_being_triggered())
    {
      if(!sync.is_observing_expression())
        expressions::update(*sync.m_expression);

      sync.observe_expression(true);

      if(sync.trigger_request)
        sync.end_trigger_request();
      else if(!expressions::evaluate(*sync.m_expression))
        return sync_status::NOT_READY;
    }

    sync.set_is_being_triggered(true);

    if(sync.m_expression)
    {
      expressions::reset(*sync.m_expression);
    }

    if(!sync.has_trigger_date() && sync.has_sync_rate())
    {
      return quantify_time_sync(sync, tk);
    }
  }
  return sync_status::NOT_READY;
}
static constexpr bool is_pending(ossia::time_event::status st)
{
  return uint8_t(st) & uint8_t(ossia::time_event::status::PENDING);
};
static constexpr bool is_finished(ossia::time_event::status st)
{
  return uint8_t(st) & uint8_t(ossia::time_event::status::FINISHED);
};
static constexpr bool is_happened(ossia::time_event::status st)
{
  return (uint8_t(st) & uint8_t(ossia::time_event::status::HAPPENED))
         == uint8_t(ossia::time_event::status::HAPPENED);
};
static constexpr bool is_disposed(ossia::time_event::status st)
{
  return (uint8_t(st) & uint8_t(ossia::time_event::status::DISPOSED))
         == uint8_t(ossia::time_event::status::DISPOSED);
};

int is_timesync_ready(
    time_sync& sync, small_event_vec& pendingEvents, bool& maximalDurationReached)
{
  pendingEvents.clear();
  auto activeCount = sync.get_time_events().size();
  std::size_t pendingCount = 0;

  auto on_pending = [&](ossia::time_event* timeEvent) {
    if(!ossia::contains(pendingEvents, timeEvent))
    {
      pendingCount++;
      pendingEvents.push_back(timeEvent);
    }

    for(const std::shared_ptr<ossia::time_interval>& timeInterval :
        timeEvent->previous_time_intervals())
    {
      if(timeInterval->get_date() >= timeInterval->get_max_duration())
      {
        maximalDurationReached = true;
        break;
      }
    }
  };

  for(const std::shared_ptr<time_event>& timeEvent : sync.get_time_events())
  {
    const auto status = timeEvent->get_status();
    if(is_finished(status) || status == time_event::status::NONE)
    {
      bool minimalDurationReached = true;

      for(const std::shared_ptr<ossia::time_interval>& timeInterval :
          timeEvent->previous_time_intervals())
      {
        // ignore graphal intervals for the sake of allowing them to repeat
        if(timeInterval->graphal)
          continue;

        const auto& ev = timeInterval->get_start_event();
        const auto start_ev_status = ev.get_status();

        // previous TimeIntervals with a DISPOSED start event are ignored
        if(is_disposed(start_ev_status))
        {
          continue;
        }

        // previous TimeInterval with a not yet HAPPENED start event
        // can't have reached its minimal duration
        if(!is_happened(start_ev_status) && !is_finished(start_ev_status)
           && !is_pending(start_ev_status))
        {
          minimalDurationReached = false;
          break;
        }

        // previous TimeInterval which doesn't reach its minimal duration
        // force to quit
        if(timeInterval->get_date() < timeInterval->get_min_duration())
        {
          minimalDurationReached = false;
          break;
        }
      }

      // access to PENDING status once all previous TimeIntervals allow it
      if(minimalDurationReached)
      {
        timeEvent->set_status(time_event::status::PENDING);
        on_pending(timeEvent.get());
      }
    }
    else if(status == time_event::status::PENDING)
    {
      on_pending(timeEvent.get());
    }
    else if(is_disposed(status))
    {
      activeCount--;
    }
  }

  return pendingCount == activeCount;
}

sync_status scenario::process_this_musical(
    time_sync& sync, small_event_vec& pendingEvents, small_event_vec& maxReachedEvents,
    ossia::time_value tick_offset, const ossia::token_request& req) noexcept
{
  // prepare to remember which event changed its status to PENDING
  // because it is needed in time_sync::trigger

  bool maximalDurationReached = false;
  // if all TimeEvents are not PENDING
  if(!is_timesync_ready(sync, pendingEvents, maximalDurationReached))
  {
    if(sync.m_evaluating)
    {
      sync.m_evaluating = false;
      sync.end_trigger_request();
      sync.callbacks.left_evaluation();
    }

    return sync_status::NOT_READY;
  }

  return trigger_sync_musical(
      sync, maxReachedEvents, tick_offset, req, maximalDurationReached);
}
}
