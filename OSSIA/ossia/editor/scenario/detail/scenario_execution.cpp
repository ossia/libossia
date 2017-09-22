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
#include <cassert>
#include <hopscotch_map.h>
#include <iostream>
#include <map>
#include <set>

namespace ossia
{
void scenario::make_happen(
    time_event& event, interval_set& started, interval_set& stopped, ossia::state& st)
{
  event.m_status = time_event::status::HAPPENED;

  // stop previous TimeIntervals
  for (auto& timeInterval : event.previous_time_intervals())
  {
    stopped.insert(timeInterval.get());
  }

  // setup next TimeIntervals
  for (auto& timeInterval : event.next_time_intervals())
  {
    timeInterval->start(st);
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
    stopped.insert(timeInterval.get());
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
      nextTimeInterval->get_end_event().dispose();
  }

  if (event.m_callback)
    (event.m_callback)(event.m_status);
}

void scenario::process_this(
    time_sync& node, std::vector<time_event*>& statusChangedEvents,
    interval_set& started, interval_set& stopped, ossia::state& st)
{
  // prepare to remember which event changed its status to PENDING
  // because it is needed in time_sync::trigger
  node.m_pending.clear();

  bool maximalDurationReached = false;

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
          timeEvent->set_status(time_event::status::PENDING);
        else
          break;
      }

      // PENDING TimeEvent is ready for evaluation
      case time_event::status::PENDING:
      {
        node.m_pending.push_back(timeEvent.get());

        for (const std::shared_ptr<ossia::time_interval>& timeInterval :
             timeEvent->previous_time_intervals())
        {
          if (timeInterval->get_date() >= timeInterval->get_max_duration())
          {
            maximalDurationReached = true;
            break;
          }
        }

        break;
      }

      case time_event::status::HAPPENED:
      case time_event::status::DISPOSED:
      {
        break;
      }
    }
  }

  // if all TimeEvents are not PENDING
  if (node.m_pending.size() != node.m_timeEvents.size())
  {
    if (node.m_evaluating)
    {
      node.m_evaluating = false;
      node.left_evaluation.send();
    }

    return;
  }

  if (!node.m_evaluating)
  {
    node.m_evaluating = true;
    node.entered_evaluation.send();
  }

  //! \todo force triggering if at leat one TimeEvent has
  // at least one TimeInterval over its maximal duration

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
      return;
  }

  // trigger the time sync

  // now TimeEvents will happen or be disposed
  for (auto& timeEvent : node.m_pending)
  {
    time_event& ev = *timeEvent;
    auto& expr = ev.get_expression();
    // update any Destination value into the expression
    expressions::update(expr);

    if (expressions::evaluate(expr))
      make_happen(ev, started, stopped, st);
    else
      make_dispose(ev, stopped);
  }

  // stop expression observation now the TimeSync has been processed
  node.observe_expression(false);

  // notify observers
  node.triggered.send();

  // former PENDING TimeEvents are now HAPPENED or DISPOSED
  statusChangedEvents.insert(
      statusChangedEvents.end(), node.m_pending.begin(), node.m_pending.end());

  node.m_pending.clear();

  node.m_evaluating = false;
  node.finished_evaluation.send(maximalDurationReached);
  if (maximalDurationReached)
    node.m_status = time_sync::DONE_MAX_REACHED;
  else
    node.m_status = time_sync::DONE_TRIGGERED;
}

enum progress_mode
{
  PROGRESS_MIN,
  PROGRESS_MAX
};
static const constexpr progress_mode mode{PROGRESS_MAX};

void update_overtick(
    time_interval& interval, time_sync* end_node,
    ossia::time_value tick_us, ossia::time_value cst_old_date,
    overtick_map& node_tick_dur)
{
  // Store the over-ticking min / max, scaled to speed = 1.
  // That is : tick - (max_dur - tick_start_dur) / speed
  auto cst_speed = interval.get_speed();
  auto ot
      = tick_us - (interval.get_max_duration() - cst_old_date) / cst_speed;

  auto node_it = node_tick_dur.lower_bound(end_node);
  if (node_it != node_tick_dur.end() && (end_node == node_it->first))
  {
    auto& cur = node_it->second;

    if (ot < cur.min)
      cur.min = ot;
    if (ot > cur.max)
      cur.max = ot;
  }
  else
  {
    node_tick_dur.insert(node_it, std::make_pair(end_node, overtick{ot, ot}));
  }
}

ossia::state_element scenario::tick_interval(time_interval& interval, time_value tick)
{
  // Tick without going over the max
  // so that the state is not 1.01*automation for instance.
  auto cst_max_dur = interval.get_max_duration();
  if (!cst_max_dur.infinite())
  {
    auto this_tick = std::min(tick, cst_max_dur - interval.get_date());
    return interval.tick(this_tick);
  }
  else
  {
    return interval.tick(tick);
  }
}

state_element scenario::state(ossia::time_value date, double pos)
{
  // ossia::logger().info("scenario::state starts");
  if (date != m_lastDate)
  {
    const bool is_unmuted = unmuted();

    auto prev_last_date = m_lastDate;
    m_lastDate = date;

    // Duration of this tick.
    time_value tick_ms
        = (prev_last_date == Infinite) ? date : (date - prev_last_date);

    ossia::state cur_state;
    m_overticks.clear();
    m_endNodes.clear();
    intervals_started.clear();
    intervals_stopped.clear();

    m_endNodes.reserve(m_nodes.size());
    m_overticks.reserve(m_nodes.size());
    // First we should find, for each running interval, the actual maximum
    // tick length
    // that they can be ticked. If it is < tick_us, then they won't execute.

    // Three categories of intervals:
    // * the ones currently we're starting from
    // * intermediary ones (e.g. fitting entirely in one tick) : we take their
    // state at 0.5
    // * the ones we're finishing in : we take their state where we finish

    ossia::state nullState;
    auto& writeState = is_unmuted ? cur_state : nullState;
    std::vector<time_event*> statusChangedEvents;
    for (time_sync* n : m_waitingNodes)
    {
      process_this(
          *n, statusChangedEvents, m_runningIntervals, m_runningIntervals, writeState);
      if (!statusChangedEvents.empty())
      {
        // TODO won't work if there are multiple waiting nodes

        if (is_unmuted)
        {
          for (const auto& ev : statusChangedEvents)
          {
            flatten_and_filter(cur_state, ev->get_state());
          }
        }

        statusChangedEvents.clear();
      }
    }
    m_waitingNodes.clear();

    for (time_interval* interval : m_runningIntervals)
    {
      auto cst_old_date = interval->get_date();
      auto st = tick_interval(*interval, tick_ms);
      if (is_unmuted)
      {
        flatten_and_filter(cur_state, std::move(st));
      }

      // ossia::logger().info("scenario::state tick {}: {}", (void*)interval,
      // tick_us);

      auto end_node = &interval->get_end_event().get_time_sync();
      m_endNodes.insert(end_node);

      update_overtick(
          *interval, end_node, tick_ms, cst_old_date, m_overticks);
    }

    // Handle time syncs / events... if they are not finished, intervals in
    // running_interval are in cur_cst
    // else, add the next intervals

    for (time_sync* node : m_endNodes)
    {
      process_this(
          *node, statusChangedEvents, intervals_started,
          intervals_stopped, writeState);
    }

    for (auto c : intervals_stopped)
      m_runningIntervals.erase(c);
    m_runningIntervals.insert(
        intervals_started.begin(), intervals_started.end());

    m_endNodes.clear();
    do
    {
      if (is_unmuted)
      {
        // For intervals that did finish, we take their last state :
        for (time_interval* interval : intervals_stopped)
        {
          //flatten_and_filter(cur_state, interval->state());
          interval->stop();
        }
      }

      for (const auto& timeEvent : statusChangedEvents)
      {
        time_event& ev = *timeEvent;
        switch (ev.get_status())
        {
          case time_event::status::HAPPENED:
          {
            if (is_unmuted)
            {
              flatten_and_filter(cur_state, ev.get_state());
            }

            auto& tn = ev.get_time_sync();
            if (tn.get_status() == time_sync::status::DONE_MAX_REACHED)
            {
              // Propagate the remaining tick to the next intervals
              auto it = m_overticks.find(&tn);
              if (it == m_overticks.end())
              {
                // ossia::logger().info("scenario::state tick_dur not found");
                continue;
              }

              time_value remaining_tick
                  = (mode == PROGRESS_MAX) ? it->second.max : it->second.min;

              const auto& next = ev.next_time_intervals();
              // overticks.reserve(overticks.size() + next.size());
              for (const auto& interval : next)
              {
                // ossia::logger().info("scenario::state tick {}: {}",
                // (void*)interval, tick_dur);
                auto st = tick_interval(*interval, remaining_tick);
                if(is_unmuted)
                  flatten_and_filter(cur_state, std::move(st));

                auto end_node = &interval->get_end_event().get_time_sync();
                m_endNodes.insert(end_node);

                update_overtick(
                    *interval, end_node, remaining_tick, 0._tv, m_overticks);
              }
            }
            break;
          }

          case time_event::status::DISPOSED:
          default:
            break;
        }
      }

      statusChangedEvents.clear();
      intervals_started.clear();
      intervals_stopped.clear();

      for (auto node : m_endNodes)
      {
        process_this(
            *node, statusChangedEvents, intervals_started,
            intervals_stopped, writeState);
      }

      for (time_interval* c : intervals_stopped)
        m_runningIntervals.erase(c);
      m_runningIntervals.insert(
          intervals_started.begin(), intervals_started.end());
      m_endNodes.clear();

    } while (!statusChangedEvents.empty());

    // Finally, take the state of the intervals that are still running.
    /* for (time_interval* interval : m_runningIntervals)
    {
      flatten_and_filter(cur_state, interval->state());
    } */

    m_lastState = cur_state;
  }

  // ossia::logger().info("scenario::state ends");
  return m_lastState;
}

/// Old execution algorithm ///

/*
state_element scenario::state(ossia::time_value date, double pos)
{
  // if date hasn't been processed already
  if (date != m_lastDate)
  {
    auto prev_last_date = m_lastDate;
    m_lastDate = date;

    ossia::state cur_state;
    // reset internal mCurrentState

    // process the scenario from the first TimeSync to the running intervals
    std::vector<time_event*> statusChangedEvents;
    time_sync& n = *m_nodes[0];
    n.process(statusChangedEvents);

    // add the state of each newly HAPPENED TimeEvent
    if(unmuted())
    {
      for (const auto& timeEvent : statusChangedEvents)
      {
        time_event& ev = *timeEvent;
        if (ev.get_status() == time_event::status::HAPPENED)
          flatten_and_filter(cur_state, ev.get_state());
      }
    }

    // make the time of each running TimeInterval flows and add their state
    // note : this means TimeInterval's state can overwrite TimeEvent's state
    for (const auto& timeInterval : m_intervals)
    {
      time_interval& cst = *timeInterval;
      if (cst.get_drive_mode() != clock::drive_mode::EXTERNAL)
      {
        throw execution_error("scenario_impl::state: "
            "the pattern interval clock is supposed to "
            "be in EXTERNAL drive mode");
        return {};
      }

      if (cst.running())
      {
        // don't tick if the TimeInterval is starting to avoid double ticks
        auto& startEvent = cst.get_start_event();
        bool not_starting = none_of(
            statusChangedEvents, [&](auto ev) {
              return ev->get_status() == time_event::status::HAPPENED
                     && ev == &startEvent;
            });

        if (not_starting)
        {
          // no such event found : not starting
          if (prev_last_date == Infinite)
            cst.tick(date * 1000.);
          else
            cst.tick(ossia::time_value{(date - prev_last_date) * 1000.});
        }
        else
        {

          // We advance the interval so that we don't loose time
          // TODO getDate is worst-case linear, maybe we should cache it to
          // have the executedDate in constant time ?
//          if(prev_last_date == Infinite)
//              cst.tick();
//          else
//              cst.tick(((date -
cst.get_start_event().get_time_sync()->get_date())*
//          1000.));

        }
      }

      // if the time interval is still running after the tick
      if (cst.running())
      {
        flatten_and_filter(cur_state, cst.state());
      }
    }
    m_lastState = cur_state;

    // if all the TimeEvents are not NONE : the Scenario is done
    bool done = !any_of(m_nodes, [](const std::shared_ptr<time_sync>& tn) {
      return any_of(
          tn->get_time_events(), [](const std::shared_ptr<time_event>& ev) {
            return ev->get_status() == time_event::status::NONE;
          });
    });

    // if the Scenario is done : stop the parent TimeInterval
    if (done)
    {
      if (date > parent()->get_min_duration())
      {
        ; //! \todo mParent->stop(); // if the parent TimeInterval's Clock is
          //! in EXTERNAL drive mode, it creates a deadlock.
      }
    }
  }

  return m_lastState;
}
*/
}
