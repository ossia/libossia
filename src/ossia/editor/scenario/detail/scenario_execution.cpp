// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/dataflow/nodes/forward_node.hpp>
#include <ossia/detail/algorithms.hpp>
#include <ossia/detail/flat_map.hpp>
#include <ossia/detail/flat_set.hpp>
#include <ossia/detail/hash_map.hpp>
#include <ossia/detail/logger.hpp>
#include <ossia/editor/exceptions.hpp>
#include <ossia/editor/scenario/detail/continuity.hpp>
#include <ossia/editor/scenario/scenario.hpp>
#include <ossia/editor/scenario/time_event.hpp>
#include <ossia/editor/scenario/time_interval.hpp>
#include <ossia/editor/scenario/time_sync.hpp>

#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/graph/labeled_graph.hpp>
#include <boost/graph/topological_sort.hpp>

namespace ossia
{
small_sync_vec scenario::get_roots() const noexcept
{
  small_sync_vec res;
  res.reserve(4);

  for(auto& tn : get_time_syncs())
  {
    if(tn->is_start())
    {
      res.push_back(tn.get());
    }
  }

  return res;
}

void scenario::make_happen(
    time_event& event, interval_set& started, interval_set& stopped,
    ossia::time_value tick_offset, const ossia::token_request& tok)
{
  event.m_status = time_event::status::HAPPENED;

  // stop previous TimeIntervals
  for(const std::shared_ptr<ossia::time_interval>& timeInterval :
      event.previous_time_intervals())
  {
    timeInterval->stop();
    mark_end_discontinuous{}(*timeInterval);
    stopped.erase(timeInterval.get());
  }

  event.tick(0_tv, tick_offset, tok.speed);

  // setup next TimeIntervals
  for(const std::shared_ptr<ossia::time_interval>& timeInterval :
      event.next_time_intervals())
  {
    timeInterval->set_parent_speed(tok.speed);
    timeInterval->start();
    // timeInterval->tick_current(tick_offset, tok);
    mark_start_discontinuous{}(*timeInterval);

    started.insert(timeInterval.get());
  }

  if(event.m_callback)
    (event.m_callback)(event.m_status);

  reinterpret_cast<uint8_t&>(event.m_status) |= uint8_t(time_event::status::FINISHED);
}

void scenario::make_dispose(time_event& event, interval_set& stopped)
{
  if(event.m_status == time_event::status::HAPPENED)
  {
    ossia_do_throw(
        execution_error,
        "time_event::dispose: "
        "HAPPENED event cannot be disposed");
    return;
  }

  event.m_status = time_event::status::DISPOSED;

  // stop previous TimeIntervals
  for(auto& timeInterval : event.previous_time_intervals())
  {
    timeInterval->stop();
    mark_end_discontinuous{}(*timeInterval);
    stopped.erase(timeInterval.get());
  }

  // dispose next TimeIntervals end event if everything is disposed before
  for(auto& nextTimeInterval : event.next_time_intervals())
  {
    bool dispose = true;

    for(auto& previousTimeInterval :
        nextTimeInterval->get_end_event().previous_time_intervals())
    {
      if(previousTimeInterval->get_start_event().get_status()
         != time_event::status::DISPOSED)
      {
        dispose = false;
        break;
      }
    }

    if(dispose && !nextTimeInterval->graphal)
      make_dispose(nextTimeInterval->get_end_event(), stopped);
  }

  if(event.m_callback)
    (event.m_callback)(event.m_status);

  reinterpret_cast<uint8_t&>(event.m_status) |= uint8_t(time_event::status::FINISHED);
}
enum progress_mode
{
  PROGRESS_MIN,
  PROGRESS_MAX
};

static const constexpr progress_mode mode{PROGRESS_MAX};

void scenario::run_interval(
    ossia::time_interval& interval, const ossia::token_request& tk,
    const time_value& tick_ms, ossia::time_value tick, ossia::time_value offset)
{
  const auto& cst_old_date = interval.get_date();
  auto cst_max_dur = interval.get_max_duration();
  const auto end_node = &interval.get_end_event().get_time_sync();

  auto it = m_itv_end_map.find(&interval);
  if(it != m_itv_end_map.end() && it->second < cst_max_dur)
  {
    cst_max_dur = it->second;
  }

  interval.set_parent_speed(tk.speed);

  // Tick without going over the max
  // so that the state is not 1.01*automation for instance.
  if(!cst_max_dur.infinite())
  {
    if(auto s = interval.get_speed(interval.get_date()); BOOST_LIKELY(s >= 0.))
    {
      auto max_tick = time_value{cst_max_dur - cst_old_date};
      double diff = s * tick.impl - max_tick.impl;
      if(diff <= 0.)
      {
        if(tick != 0_tv)
          interval.tick_offset(tick, offset, tk);
      }
      else
      {
        if(max_tick != 0_tv)
        {
          interval.tick_offset_speed_precomputed(max_tick, offset, tk);
        }
        else if(cst_max_dur == 0_tv)
        {
          interval.tick_offset_speed_precomputed(max_tick, offset, tk);
        }

        const auto ot = ossia::time_value{int64_t(diff)};
        const auto node_it = m_overticks.lower_bound(end_node);
        if(node_it != m_overticks.end() && (end_node == node_it->first))
        {
          auto& cur = const_cast<overtick&>(node_it->second);

          if(ot < cur.min)
            cur.min = ot;
          if(ot > cur.max)
          {
            cur.max = ot;
            cur.offset = tk.offset + tick_ms - cur.max;
          }
        }
        else
        {
          m_overticks.insert(
              node_it, {end_node, overtick{ot, ot, tk.offset + tick_ms - ot}});
        }
      }
    }
    else
    {
      if(tick * s < cst_old_date)
      {
        if(tick != 0_tv)
          interval.tick_offset(tick, offset, tk);
      }
      else
      {
        if(tick != 0_tv)
        {
          interval.tick_offset_speed_precomputed(
              ossia::time_value{-cst_old_date.impl}, offset, tk);
        }
      }
      // no overtick support for running backwards for now - we just stop at
      // zero
    }
  }
  else
  {
    interval.tick_offset(tick, offset, tk);
  }
  if(interval.get_date() >= interval.get_min_duration())
  {
    m_endNodes.insert(end_node);

    // Graph intervals always enable what's after them by definition
    if(interval.graphal)
    {
      for(const std::shared_ptr<ossia::time_event>& ev : end_node->get_time_events())
      {
        ev->set_status(ossia::time_event::status::PENDING);
      }
    }
  }
}

void scenario::stop_interval(time_interval& itv)
{
  itv.stop();

  m_runningIntervals.erase(&itv);
  m_itv_end_map.erase(&itv);
}

void scenario::state_impl(const ossia::token_request& tk)
{
#if defined(OSSIA_SCENARIO_DATAFLOW)
  node->request(tk);
#endif
  // ossia::logger().info("scenario::state starts");
  // if (date != m_lastDate)
  {
    auto prev_last_date = m_last_date;
    m_last_date = tk.date;

    // Duration of this tick.
    time_value tick_ms
        = (prev_last_date == Infinite) ? tk.date : (tk.date - prev_last_date);

    m_overticks.clear();
    m_endNodes.clear();
    m_retry_syncs.clear();

    m_retry_syncs.reserve(8);
    m_endNodes.reserve(m_nodes.size());
    m_overticks.reserve(m_nodes.size());

    for(auto it = m_runningIntervals.begin(); it != m_runningIntervals.end();)
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

    m_pendingEvents.clear();
    m_maxReachedEvents.clear();

    for(auto& n : m_rootNodes)
    {
      if(!n->is_observing_expression())
      {
        n->observe_expression(true, [n](bool b) {
          if(b)
          {
            n->start_trigger_request();
          }
        });
      }

      // Manual request (user clicked on the trigger)
      if(n->trigger_request)
      {
        if(m_waitingNodes.find(n) != m_waitingNodes.end())
        {
          // it will execute soon after
          continue;
        }
        else
        {
          reset_component(*n);
          if(n->is_autotrigger())
            n->m_evaluating = true;

          m_waitingNodes.insert(n);
        }
      }
    }

    for(auto it = m_waitingNodes.begin(); it != m_waitingNodes.end();)
    {
      auto n = *it;
      // Note: we pass m_runningIntervals as stopped because it does not
      // matter: by design, no interval could be stopped at this point since
      // it's the root scenarios. So this prevents initializing a dummy class.
      sync_status res = process_this(
          *n, m_pendingEvents, m_maxReachedEvents, m_runningIntervals,
          m_runningIntervals, tk.offset, tk);
      switch(res)
      {
        case sync_status::NOT_READY:
          ++it;
          break;
        case sync_status::RETRY:
          ++it;
          break;
        case sync_status::DONE:
          it = m_waitingNodes.erase(it);
          break;
      }
      m_pendingEvents.clear();
    }

    m_pendingEvents.clear();

    // Check intervals that have been quantized through manual interaction
    // (the little play / stop buttons)
    for(auto it = m_itv_to_start.begin(); it != m_itv_to_start.end();)
    {
      auto [itv, ratio] = *it;
      if(auto date = tk.get_quantification_date(ratio))
      {
        if(itv->running())
        {
          mark_end_discontinuous{}(*itv);
          itv->stop();
        }
        itv->set_parent_speed(tk.speed);
        itv->start();
        // itv->tick_current(*date, tk);
        // mark_start_discontinuous{}(*itv);

        m_runningIntervals.insert(itv);
        auto& start_ev = itv->get_start_event();
        start_ev.set_status(ossia::time_event::status::HAPPENED);
        auto& end_ev = itv->get_end_event();
        end_ev.set_status(ossia::time_event::status::NONE);

        if(m_exclusive)
        {
          for(auto& running : m_runningIntervals)
          {
            running->stop();
          }
          m_runningIntervals.clear();
          m_itv_to_start.clear();
          break;
        }
        else
        {
          it = m_itv_to_start.erase(it);
        }
      }
      else
      {
        ++it;
      }
    }
    for(auto it = m_itv_to_stop.begin(); it != m_itv_to_stop.end();)
    {
      auto [itv, ratio] = *it;
      if(auto date = tk.get_quantification_date(ratio))
      {
        if(itv->running())
        {
          // mark_end_discontinuous{}(*itv);
          itv->stop();
        }

        if(auto running_it = m_runningIntervals.find(itv);
           running_it != m_runningIntervals.end())
          m_runningIntervals.erase(running_it);

        it = m_itv_to_stop.erase(it);
      }
      else
      {
        ++it;
      }
    }

    // First check timesyncs already past their min
    // for any that may have a quantization setting

    for(time_interval* interval : m_runningIntervals)
    {
      if(interval->get_date() >= interval->get_min_duration())
      {
        const auto end_node = &interval->get_end_event().get_time_sync();
        if(end_node->has_sync_rate())
        {
          m_endNodes.insert(end_node);
          process_this_musical(
              *end_node, m_pendingEvents, m_maxReachedEvents, tk.offset, tk);
        }
      }
    }

    for(time_interval* interval : m_runningIntervals)
    {
      run_interval(*interval, tk, tick_ms, tick_ms, tk.offset);
    }

    // Handle time syncs / events... if they are not finished, intervals in
    // running_interval are in cur_cst
    // else, add the next intervals

    do
    {
      for(const auto& timeEvent : m_maxReachedEvents)
      {
        time_event& ev = *timeEvent;
        auto& tn = ev.get_time_sync();
        // Propagate the remaining tick to the next intervals
        auto it = m_overticks.find(&tn);
        if(it == m_overticks.end())
        {
          continue;
        }
        else
        {
          const time_value remaining_tick
              = (mode == PROGRESS_MAX) ? it->second.max : it->second.min;

          const auto offset = tk.offset + tick_ms - remaining_tick;
          const_cast<overtick&>(it->second).offset = offset;
          for(const auto& interval : ev.next_time_intervals())
          {
            run_interval(*interval, tk, tick_ms, remaining_tick, offset);
          }
        }
      }

      m_maxReachedEvents.clear();

      for(auto node : m_endNodes)
      {
        auto it = m_overticks.find(node);
        sync_status status{};
        if(it != m_overticks.end())
        {
          status = process_this(
              *node, m_pendingEvents, m_maxReachedEvents, m_runningIntervals,
              m_runningIntervals, it->second.offset, tk);
        }
        else
        {
          status = process_this(
              *node, m_pendingEvents, m_maxReachedEvents, m_runningIntervals,
              m_runningIntervals, tk.offset, tk);
        }

        switch(status)
        {
          case sync_status::DONE: {
            node->set_is_being_triggered(false);
            m_retry_syncs.erase(node);
            break;
          }
          case sync_status::NOT_READY:
            break;
          case sync_status::RETRY:
            m_retry_syncs.insert(node);
            break;
        }
      }

      m_endNodes.tree().get_sequence_ref().assign(
          m_retry_syncs.tree().get_sequence_cref().begin(),
          m_retry_syncs.tree().get_sequence_cref().end());
      m_retry_syncs.clear();

    } while(!m_maxReachedEvents.empty() || !m_endNodes.empty());
  }

  // ossia::logger().info("scenario::state ends");
}
/*
scenario_graph::scenario_graph(scenario& sc)
    : scenar{sc}
{
  graph.m_vertices.reserve(1024);

}

void scenario_graph::update_components_cache() const
{
  if(dirty)
  {
    m_components_cache.resize(boost::num_vertices(graph));
    boost::connected_components(graph, m_components_cache.data());
    dirty = false;
  }
}

void scenario_graph::reset_component(time_sync& sync) const
{
  update_components_cache();

  std::vector<std::shared_ptr<ossia::time_sync>> to_disable_sync;
  std::vector<std::shared_ptr<ossia::time_interval>> to_disable_itv;
  auto comp = m_components_cache[vertices.at(&sync)];
  for(auto s : scenar.get_time_syncs())
  {
    auto this_comp = m_components_cache[vertices.at(s.get())];
    if(this_comp == comp)
    {
      to_disable_sync.push_back(s);
      for(auto& ev : s->get_time_events())
      {
        for(auto& cst : ev->previous_time_intervals())
        {
          to_disable_itv.push_back(cst);
        }
        for(auto& cst : ev->next_time_intervals())
        {
          to_disable_itv.push_back(cst);
        }
      }
    }
  }

  scenar.reset_subgraph(to_disable_sync, to_disable_itv, sync);
}*/
}
