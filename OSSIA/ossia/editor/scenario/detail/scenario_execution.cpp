// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/detail/algorithms.hpp>
#include <ossia/detail/logger.hpp>
#include <ossia/editor/exceptions.hpp>
#include <ossia/editor/scenario/scenario.hpp>
#include <ossia/editor/scenario/time_interval.hpp>
#include <ossia/editor/scenario/time_event.hpp>
#include <ossia/editor/scenario/time_sync.hpp>
#include <boost/container/flat_map.hpp>
#include <boost/container/flat_set.hpp>
#include <ossia/dataflow/graph.hpp>
#include <ossia/editor/scenario/detail/continuity.hpp>
#include <ossia/editor/scenario/detail/scenario_graph.hpp>
#include <cassert>
#include <hopscotch_map.h>
#include <iostream>
#include <map>
#include <set>

namespace ossia
{
void scenario::make_happen(
    time_event& event,
    interval_set& started, interval_set& stopped,
    ossia::time_value tick_offset)
{
  event.m_status = time_event::status::HAPPENED;

  // stop previous TimeIntervals
  for (const std::shared_ptr<ossia::time_interval>& timeInterval : event.previous_time_intervals())
  {
    timeInterval->stop();
    mark_end_discontinuous{}(*timeInterval);
    stopped.erase(timeInterval.get());
  }

  event.tick(tick_offset);

  // setup next TimeIntervals
  for (const std::shared_ptr<ossia::time_interval>& timeInterval : event.next_time_intervals())
  {
    timeInterval->start();
    timeInterval->tick_current(tick_offset);
    mark_start_discontinuous{}(*timeInterval);

    started.insert(timeInterval.get());
  }

  if (event.m_callback)
    (event.m_callback)(event.m_status);
}

void scenario::make_dispose(time_event& event, interval_set& stopped)
{
  if (event.m_status == time_event::status::HAPPENED)
  {
    throw execution_error(
        "time_event::dispose: "
        "HAPPENED event cannot be disposed");
    return;
  }

  event.m_status = time_event::status::DISPOSED;

  // stop previous TimeIntervals
  for (auto& timeInterval : event.previous_time_intervals())
  {
    timeInterval->stop();
    mark_end_discontinuous{}(*timeInterval);
    stopped.erase(timeInterval.get());
  }

  // dispose next TimeIntervals end event if everything is disposed before
  for (auto& nextTimeInterval : event.next_time_intervals())
  {
    bool dispose = true;

    for (auto& previousTimeInterval :
         nextTimeInterval->get_end_event().previous_time_intervals())
    {
      if (previousTimeInterval->get_start_event().get_status()
          != time_event::status::DISPOSED)
      {
        dispose = false;
        break;
      }
    }

    if (dispose)
      make_dispose(nextTimeInterval->get_end_event(), stopped);
  }

  if (event.m_callback)
    (event.m_callback)(event.m_status);
}

bool scenario::trigger_sync(
    time_sync& node, small_event_vec::iterator pendingBegin, small_event_vec::iterator pendingEnd,
    interval_set& started, interval_set& stopped,
    ossia::time_value tick_offset, bool maximalDurationReached)
{
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
    if(!node.has_trigger_date())
    {
      if (!node.is_observing_expression())
        expressions::update(*node.m_expression);

      node.observe_expression(true);

      if (node.trigger_request)
        node.trigger_request = false;
      else if (!expressions::evaluate(*node.m_expression))
        return false;
    }

    // at this point we can assume we are going to trigger.
    const auto& cur_date = this->node->requested_tokens.back().date;
    if(node.has_trigger_date())
    {
      bool ok = true;
      for(const auto& ev : node.get_time_events())
      {
        for(const auto& cst : ev->previous_time_intervals())
        {
          if(cst->get_start_event().get_status() == ossia::time_event::status::HAPPENED)
          {
            ok &= (cst->get_date() == m_itv_end_map.find(cst.get())->second);
          }
          if(!ok)
            break;
        }
        if(!ok)
          break;
      }

      if(!ok)
        return false;
      else
      {
        maximalDurationReached = true;

        for(const auto& ev : node.get_time_events())
        {
          for(const auto& cst : ev->previous_time_intervals())
          {
            m_itv_end_map.erase(cst.get());
          }
        }
      }
    }
    else if(node.has_sync_rate())
    {
      // compute absolute date at which it should execute,
      // assuming we start at the next tick
      // TODO div by zero
      time_value expected_date{node.get_sync_rate().impl * (1 + cur_date.impl / node.get_sync_rate().impl)};
      node.set_trigger_date(expected_date);
      auto diff_date = expected_date - cur_date;

      // compute the "fake max" date at which intervals must end for this to work
      for(const auto& ev : node.get_time_events())
      {
        for(const auto& cst : ev->previous_time_intervals())
        {
          m_itv_end_map.insert(std::make_pair(cst.get(), cst->get_date() + diff_date));
        }
      }
      node.observe_expression(false);
      return false;
    }
  }

  // trigger the time sync

  // now TimeEvents will happen or be disposed.
  // the last added events are necessarily the ones of this node.
  for (auto it = pendingBegin; it != pendingEnd; ++it)
  {
    time_event& ev = **it;
    auto& expr = ev.get_expression();
    // update any Destination value into the expression
    expressions::update(expr);

    if (expressions::evaluate(expr))
      make_happen(ev, started, stopped, tick_offset);
    else
      make_dispose(ev, stopped);
  }

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

bool scenario::process_this(
    time_sync& node, small_event_vec& pendingEvents,
    interval_set& started, interval_set& stopped,
    ossia::time_value tick_offset)
{
  // prepare to remember which event changed its status to PENDING
  // because it is needed in time_sync::trigger
  auto activeCount = node.get_time_events().size();
  std::size_t pendingCount = 0;
  const auto beginPending = pendingEvents.size();

  bool maximalDurationReached = false;
  auto on_pending = [&] (ossia::time_event* timeEvent)
  {
    if(!ossia::contains(pendingEvents, timeEvent))
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

  for (const std::shared_ptr<time_event>& timeEvent : node.get_time_events())
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
          if (ev.get_status() == time_event::status::DISPOSED)
          {
            continue;
          }

          // previous TimeInterval with a none HAPPENED start event
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

  // if all TimeEvents are not PENDING
  if (pendingCount != activeCount)
  {
    if (node.m_evaluating)
    {
      node.m_evaluating = false;
      node.left_evaluation.send();
    }

    return false;
  }

  return trigger_sync(node,
                      pendingEvents.begin() + beginPending,
                      pendingEvents.end(),
                      started, stopped,
                      tick_offset, maximalDurationReached);
}

enum progress_mode
{
  PROGRESS_MIN,
  PROGRESS_MAX
};
static const constexpr progress_mode mode{PROGRESS_MAX};

void scenario::state(ossia::time_value date, double pos, ossia::time_value tick_offset, double gspeed)
{
  node->requested_tokens.push_back({date, pos, tick_offset, gspeed});
  // ossia::logger().info("scenario::state starts");
  if (date != m_lastDate)
  {
    auto prev_last_date = m_lastDate;
    m_lastDate = date;

    // Duration of this tick.
    time_value tick_ms
        = (prev_last_date == Infinite) ? date : (date - prev_last_date);

    m_overticks.clear();
    m_endNodes.clear();

    m_endNodes.reserve(m_nodes.size());
    m_overticks.reserve(m_nodes.size());

    for(auto it  = m_runningIntervals.begin(); it != m_runningIntervals.end(); )
    {
      if((*it)->get_end_event().get_status() == time_event::status::HAPPENED)
        it = m_runningIntervals.erase(it);
      else
        ++it;
    }
    // First we should find, for each running interval, the actual maximum
    // tick length
    // that they can be ticked. If it is < tick_us, then they won't execute.

    // Three categories of intervals:
    // * the ones currently we're starting from
    // * intermediary ones (e.g. fitting entirely in one tick) : we take their
    // state at 0.5
    // * the ones we're finishing in : we take their state where we finish

    small_event_vec pendingEvents;

    sgraph sg{*this};
    auto comps = sg.components();
    for(auto& n : m_rootNodes)
    {
      if(!n->is_observing_expression())
      {
        n->observe_expression(true, [n] (bool b) {
          if(b)
            n->trigger_request = true;
        });
      }

      if(n->trigger_request)
      {
        if(m_waitingNodes.find(n) != m_waitingNodes.end())
        {
          // it will execute soon after
          continue;
        }
        else
        {
          auto& evs = n->get_time_events();
          for(auto& e : evs)
          {
            const auto st = e->get_status();
            if(st == ossia::time_event::status::HAPPENED || st == ossia::time_event::status::DISPOSED)
            {
              sg.reset_component(comps, *n);
              break;
            }
          }
          m_waitingNodes.insert(n);
        }
      }

    }

    for (auto it = m_waitingNodes.begin(); it != m_waitingNodes.end(); )
    {
      auto n = *it;
      // Note: we pass m_runningIntervals as stopped because it does not matter:
      // by design, no interval could be stopped at this point since it's the
      // root scenarios. So this prevents initializing a dummy class.
      bool res = process_this(
          *n, pendingEvents, m_runningIntervals, m_runningIntervals, tick_offset);
      if (res)
      {
        pendingEvents.clear();
        it = m_waitingNodes.erase(it);
      }
      else
      {
        ++it;
      }
    }

    pendingEvents.clear();

    auto run_interval = [&] (ossia::time_interval& interval, ossia::time_value tick, ossia::time_value offset)
    {
      const auto& cst_old_date = interval.get_date();
      auto cst_max_dur = interval.get_max_duration();
      const auto end_node = &interval.get_end_event().get_time_sync();

      // TODO only insert it if we are past min bound ?
      m_endNodes.insert(end_node);

      auto it = m_itv_end_map.find(&interval);
      if(it != m_itv_end_map.end() && it->second < cst_max_dur)
      {
        cst_max_dur = it->second;
      }

      interval.set_parent_speed(gspeed);

      // Tick without going over the max
      // so that the state is not 1.01*automation for instance.
      if (!cst_max_dur.infinite())
      {
        const auto this_tick = std::min(tick, cst_max_dur - cst_old_date);
        if(this_tick != 0)
        {
          interval.tick_offset(this_tick, offset);
        }

        const auto ot
            = tick - (cst_max_dur - cst_old_date) / interval.get_speed();

        const auto node_it = m_overticks.lower_bound(end_node);
        if (node_it != m_overticks.end() && (end_node == node_it->first))
        {
          auto& cur = node_it->second;

          if (ot < cur.min)
            cur.min = ot;
          if (ot > cur.max)
          {
            cur.max = ot;
            cur.offset = tick_offset + tick_ms - cur.max;
          }
        }
        else
        {
          m_overticks.insert(node_it, std::make_pair(end_node, overtick{ot, ot, tick_offset + tick_ms - ot}));
        }
      }
      else
      {
        interval.tick_offset(tick, offset);
      }
    };

    for (time_interval* interval : m_runningIntervals)
    {
      run_interval(*interval, tick_ms, tick_offset);
    }

    // Handle time syncs / events... if they are not finished, intervals in
    // running_interval are in cur_cst
    // else, add the next intervals

    do
    {
      for (const auto& timeEvent : pendingEvents)
      {
        time_event& ev = *timeEvent;
        if(ev.get_status() == time_event::status::HAPPENED)
        {
          auto& tn = ev.get_time_sync();
          if (tn.get_status() == time_sync::status::DONE_MAX_REACHED)
          {
            // Propagate the remaining tick to the next intervals
            auto it = m_overticks.find(&tn);
            if (it == m_overticks.end())
            {
              ossia::logger().info("scenario::state tick_dur not found");
              continue;
            }

            const time_value remaining_tick
                = (mode == PROGRESS_MAX) ? it->second.max : it->second.min;

            const auto offset = tick_offset + tick_ms - remaining_tick;
            it->second.offset = offset;
            for (const auto& interval : ev.next_time_intervals())
            {
              run_interval(*interval, remaining_tick, offset);
            }
          }
        }
      }

      pendingEvents.clear();

      for (auto node : m_endNodes)
      {
        process_this(
            *node, pendingEvents, m_runningIntervals,
            m_runningIntervals, m_overticks.find(node)->second.offset);
      }

      m_endNodes.clear();

    } while (!pendingEvents.empty());

  }

  // ossia::logger().info("scenario::state ends");
}

}
