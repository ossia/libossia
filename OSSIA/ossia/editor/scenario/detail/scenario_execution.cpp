// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/detail/algorithms.hpp>
#include <ossia/detail/flat_map.hpp>
#include <ossia/detail/flat_set.hpp>
#include <ossia/detail/logger.hpp>
#include <ossia/editor/exceptions.hpp>
#include <ossia/editor/scenario/detail/continuity.hpp>
#include <ossia/editor/scenario/scenario.hpp>
#include <ossia/editor/scenario/time_event.hpp>
#include <ossia/editor/scenario/time_interval.hpp>
#include <ossia/editor/scenario/time_sync.hpp>

#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/labeled_graph.hpp>
#include <boost/graph/topological_sort.hpp>

#include <tsl/hopscotch_map.h>

#include <cassert>
#include <iostream>
#include <map>
#include <set>

namespace ossia
{
void scenario::make_happen(
    time_event& event, interval_set& started, interval_set& stopped,
    ossia::time_value tick_offset)
{
  event.m_status = time_event::status::HAPPENED;

  // stop previous TimeIntervals
  for (const std::shared_ptr<ossia::time_interval>& timeInterval :
       event.previous_time_intervals())
  {
    timeInterval->stop();
    mark_end_discontinuous{}(*timeInterval);
    stopped.erase(timeInterval.get());
  }

  event.tick(0_tv, 0., tick_offset);

  // setup next TimeIntervals
  for (const std::shared_ptr<ossia::time_interval>& timeInterval :
       event.next_time_intervals())
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
    time_sync& node, small_event_vec& pending, small_event_vec& maxReachedEv,
    interval_set& started, interval_set& stopped,
    ossia::time_value tick_offset, bool maximalDurationReached)
{
  if (!node.m_evaluating)
  {
    node.m_evaluating = true;
    node.trigger_request = false;
    node.entered_evaluation.send();
  }

  // update the expression one time
  // then observe and evaluate TimeSync's expression before to trig
  // only if no maximal duration have been reached
  if (*node.m_expression != expressions::expression_true()
      && !maximalDurationReached)
  {
    if (!node.has_trigger_date())
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
    if (node.has_trigger_date())
    {
      bool ok = true;
      for (const auto& ev : node.get_time_events())
      {
        for (const auto& cst : ev->previous_time_intervals())
        {
          if (cst->get_start_event().get_status()
              == ossia::time_event::status::HAPPENED)
          {
            ok &= (cst->get_date() == m_itv_end_map.find(cst.get())->second);
          }
          if (!ok)
            break;
        }
        if (!ok)
          break;
      }

      if (!ok)
        return false;
      else
      {
        maximalDurationReached = true;

        for (const auto& ev : node.get_time_events())
        {
          for (const auto& cst : ev->previous_time_intervals())
          {
            m_itv_end_map.erase(cst.get());
          }
        }
      }
    }
    else if (node.has_sync_rate())
    {
      // compute absolute date at which it should execute,
      // assuming we start at the next tick
      // TODO div by zero
      time_value expected_date{
          node.get_sync_rate().impl
          * (1 + cur_date.impl / node.get_sync_rate().impl)};
      node.set_trigger_date(expected_date);
      auto diff_date = expected_date - cur_date;

      // compute the "fake max" date at which intervals must end for this to
      // work
      for (const auto& ev : node.get_time_events())
      {
        for (const auto& cst : ev->previous_time_intervals())
        {
          m_itv_end_map.insert(
              std::make_pair(cst.get(), cst->get_date() + diff_date));
        }
      }
      node.observe_expression(false);
      return false;
    }
  }

  // trigger the time sync

  // now TimeEvents will happen or be disposed.
  // the last added events are necessarily the ones of this node.
  for (time_event* ev : pending)
  {
    auto& expr = ev->get_expression();
    // update any Destination value into the expression
    expressions::update(expr);

    if (expressions::evaluate(expr))
    {
      make_happen(*ev, started, stopped, tick_offset);
      if (maximalDurationReached)
        maxReachedEv.push_back(ev);
    }
    else
    {
      make_dispose(*ev, stopped);
    }
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
    small_event_vec& maxReachedEvents, interval_set& started,
    interval_set& stopped, ossia::time_value tick_offset)
{
  // prepare to remember which event changed its status to PENDING
  // because it is needed in time_sync::trigger
  pendingEvents.clear();
  auto activeCount = node.get_time_events().size();
  std::size_t pendingCount = 0;

  bool maximalDurationReached = false;
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
      node.trigger_request = false;
      node.left_evaluation.send();
    }

    return false;
  }

  return trigger_sync(
      node, pendingEvents, maxReachedEvents, started, stopped, tick_offset,
      maximalDurationReached);
}

enum progress_mode
{
  PROGRESS_MIN,
  PROGRESS_MAX
};

ossia::time_value clamp_zero(ossia::time_value t)
{
   return t >= 0 ? t : 0_tv;
}
static const constexpr progress_mode mode{PROGRESS_MAX};

void scenario::state(
    ossia::time_value from, ossia::time_value date, double pos,
    ossia::time_value tick_offset, double gspeed)
{
  node->request({from, date, pos, tick_offset, gspeed});
  // ossia::logger().info("scenario::state starts");
  // if (date != m_lastDate)
  {
    auto prev_last_date = m_lastDate;
    m_lastDate = date;

    // Duration of this tick.
    time_value tick_ms
        = (prev_last_date == Infinite) ? date : (date - prev_last_date);

    m_overticks.clear();
    m_endNodes.clear();

    m_endNodes.container.reserve(m_nodes.size());
    m_overticks.container.reserve(m_nodes.size());

    for (auto it = m_runningIntervals.begin(); it != m_runningIntervals.end();)
    {
      if ((*it)->get_end_event().get_status() == time_event::status::HAPPENED)
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

    m_pendingEvents.clear();
    m_maxReachedEvents.clear();

    for (auto& n : m_rootNodes)
    {
      if (!n->is_observing_expression())
      {
        n->observe_expression(true, [n](bool b) {
          if (b)
            n->trigger_request = true;
        });
      }

      if (n->trigger_request)
      {
        if (m_waitingNodes.find(n) != m_waitingNodes.end())
        {
          // it will execute soon after
          continue;
        }
        else
        {
          auto& evs = n->get_time_events();
          for (auto& e : evs)
          {
            const auto st = e->get_status();
            if (st == ossia::time_event::status::HAPPENED
                || st == ossia::time_event::status::DISPOSED)
            {
              m_sg.reset_component(*n);
              break;
            }
          }
          m_waitingNodes.insert(n);
        }
      }
    }

    for (auto it = m_waitingNodes.begin(); it != m_waitingNodes.end();)
    {
      auto n = *it;
      // Note: we pass m_runningIntervals as stopped because it does not
      // matter: by design, no interval could be stopped at this point since
      // it's the root scenarios. So this prevents initializing a dummy class.
      bool res = process_this(
          *n, m_pendingEvents, m_maxReachedEvents, m_runningIntervals,
          m_runningIntervals, tick_offset);
      if (res)
      {
        it = m_waitingNodes.erase(it);
      }
      else
      {
        ++it;
      }
      m_pendingEvents.clear();
    }

    m_pendingEvents.clear();

    auto run_interval = [&](ossia::time_interval& interval,
                            ossia::time_value tick, ossia::time_value offset) {
      const auto& cst_old_date = interval.get_date();
      auto cst_max_dur = interval.get_max_duration();
      const auto end_node = &interval.get_end_event().get_time_sync();

      auto it = m_itv_end_map.find(&interval);
      if (it != m_itv_end_map.end() && it->second < cst_max_dur)
      {
        cst_max_dur = it->second;
      }

      interval.set_parent_speed(gspeed);

      // Tick without going over the max
      // so that the state is not 1.01*automation for instance.
      if (!cst_max_dur.infinite())
      {
        if(auto s = interval.get_speed(); BOOST_LIKELY(s >= 0.))
        {
          auto max_tick = time_value{cst_max_dur - cst_old_date};
          auto diff = s * tick - max_tick;
          if (diff <= 0)
          {
            if (tick != 0)
              interval.tick_offset(tick, offset);
          }
          else
          {
            if (max_tick != 0)
            {
              interval.tick_offset_speed_precomputed(max_tick, offset);
            }

            const auto ot
                = ossia::time_value{int64_t(diff)};
            const auto node_it = m_overticks.lower_bound(end_node);
            if (node_it != m_overticks.end() && (end_node == node_it->first))
            {
              auto& cur = const_cast<overtick&>(node_it->second);

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
              m_overticks.insert(
                  node_it,
                  std::make_pair(
                      end_node, overtick{ot, ot, tick_offset + tick_ms - ot}));
            }
          }
        }
        else
        {
          if (tick * s < cst_old_date)
          {
            if (tick != 0)
              interval.tick_offset(tick, offset);
          }
          else
          {
            if (tick != 0)
            {
              interval.tick_offset_speed_precomputed(ossia::time_value{-cst_old_date.impl}, offset);
            }
          }
          // no overtick support for running backwards for now - we just stop at zero
        }
      }
      else
      {
        interval.tick_offset(tick, offset);
      }
      if (interval.get_date() >= interval.get_min_duration())
        m_endNodes.insert(end_node);
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
      // std::cerr << "BEGIN EVENT" << std::endl;
      for (const auto& timeEvent : m_maxReachedEvents)
      {
        time_event& ev = *timeEvent;
        auto& tn = ev.get_time_sync();
        // Propagate the remaining tick to the next intervals
        auto it = m_overticks.find(&tn);
        if (it == m_overticks.end())
        {
          // ossia::logger().info("scenario::state tick_dur not found");
          continue;
        }

        const time_value remaining_tick
            = (mode == PROGRESS_MAX) ? it->second.max : it->second.min;

        const auto offset = tick_offset + tick_ms - remaining_tick;
        const_cast<overtick&>(it->second).offset = offset;
        for (const auto& interval : ev.next_time_intervals())
        {
          run_interval(*interval, remaining_tick, offset);
        }
      }
      // std::cerr << "END EVENT" << std::endl;

      m_maxReachedEvents.clear();

      for (auto node : m_endNodes)
      {
        auto it = m_overticks.find(node);
        if (it != m_overticks.end())
        {
          process_this(
              *node, m_pendingEvents, m_maxReachedEvents, m_runningIntervals,
              m_runningIntervals, it->second.offset);
        }
        else
        {
          process_this(
              *node, m_pendingEvents, m_maxReachedEvents, m_runningIntervals,
              m_runningIntervals, tick_offset);
        }
      }

      m_endNodes.clear();

    } while (!m_maxReachedEvents.empty());
  }

  // ossia::logger().info("scenario::state ends");
}

scenario_graph::scenario_graph(scenario& sc) : scenar{sc}
{
}

small_sync_vec scenario_graph::get_roots() const
{
  update_components_cache();

  small_sync_vec res;

  int root_comp
      = m_components_cache[vertices.at(scenar.get_start_time_sync().get())];

  for (auto& tn : scenar.get_time_syncs())
  {
    if (scenar.is_root_sync(*tn)
        && m_components_cache[vertices.at(tn.get())] != root_comp)
    {
      res.push_back(tn.get());
    }
  }

  return res;
}

void scenario_graph::update_components_cache() const
{
  if (dirty)
  {
    m_components_cache.resize(boost::num_vertices(graph));
    boost::connected_components(graph, m_components_cache.data());
    dirty = false;
  }
}

ossia::small_vector<ossia::time_sync*, 4>
scenario_graph::sibling_roots(const time_sync& sync) const
{
  update_components_cache();

  ossia::small_vector<ossia::time_sync*, 4> res;
  auto comp = m_components_cache[vertices.at(&sync)];

  for (const auto& s : scenar.get_time_syncs())
  {
    if (m_components_cache[vertices.at(s.get())] == comp
        && scenar.is_root_sync(*s))
    {
      res.push_back(s.get());
    }
  }

  return res;
}

void scenario_graph::reset_component(time_sync& sync) const
{
  update_components_cache();

  std::vector<std::shared_ptr<ossia::time_sync>> to_disable_sync;
  std::vector<std::shared_ptr<ossia::time_interval>> to_disable_itv;
  auto comp = m_components_cache[vertices.at(&sync)];
  for (auto s : scenar.get_time_syncs())
  {
    auto this_comp = m_components_cache[vertices.at(s.get())];
    if (this_comp == comp)
    {
      to_disable_sync.push_back(s);
      for (auto& ev : s->get_time_events())
      {
        for (auto& cst : ev->previous_time_intervals())
        {
          to_disable_itv.push_back(cst);
        }
        for (auto& cst : ev->next_time_intervals())
        {
          to_disable_itv.push_back(cst);
        }
      }
    }
  }

  scenar.reset_subgraph(to_disable_sync, to_disable_itv, sync);
}
}
