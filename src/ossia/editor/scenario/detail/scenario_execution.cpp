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
    ossia::time_value tick_offset, const ossia::token_request& tok)
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

  event.tick(0_tv, tick_offset);

  // setup next TimeIntervals
  for (const std::shared_ptr<ossia::time_interval>& timeInterval :
       event.next_time_intervals())
  {
    timeInterval->start();
    timeInterval->tick_current(tick_offset, tok);
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

scenario::sync_status scenario::trigger_sync(
    time_sync& sync, small_event_vec& pending, small_event_vec& maxReachedEv,
    interval_set& started, interval_set& stopped,
    ossia::time_value tick_offset, const ossia::token_request& tk, bool maximalDurationReached)
{
  if (!sync.m_evaluating)
  {
    sync.m_evaluating = true;
    sync.trigger_request = false;
    sync.entered_evaluation.send();
  }

  // update the expression one time
  // then observe and evaluate TimeSync's expression before to trig
  // only if no maximal duration have been reached
  if (*sync.m_expression != expressions::expression_true()
      && !maximalDurationReached)
  {
    if (!sync.has_trigger_date() && !sync.m_is_being_triggered)
    {
      if (!sync.is_observing_expression())
        expressions::update(*sync.m_expression);

      sync.observe_expression(true);

      if (sync.trigger_request)
        sync.trigger_request = false;
      else if (!expressions::evaluate(*sync.m_expression))
        return sync_status::NOT_READY;
    }

    // at this point we can assume we are going to trigger.
    sync.m_is_being_triggered = true;

    if (sync.has_trigger_date())
    {
      bool ok = true;
      for (const auto& ev : sync.get_time_events())
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
        return sync_status::NOT_READY;
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
      }
    }
    else if (sync.has_sync_rate())
    {
      const auto rate = sync.get_sync_rate();
      // we are asked to execute, now we must quantize to the next step

      optional<time_value> next_bar_start{};
      if(rate <= 1.)
      {
        // Quantize relative to bars
        if(tk.musical_end_last_bar != tk.musical_start_last_bar)
        {
          // There is a bar change in this tick
          double musical_tick_duration = tk.musical_end_position - tk.musical_start_position;
          double musical_bar_start = tk.musical_end_last_bar - tk.musical_start_position;

          double ratio = musical_bar_start / musical_tick_duration;
          time_value dt = tk.date - tk.prev_date; // TODO should be tick_offset

          next_bar_start = tk.prev_date + dt * ratio;
        }
      }
      else
      {
        // Quantize relative to quarter divisions
        // note ! if there is a bar change,
        // and no prior quantization date before that, we have to quantize to the bar change

        double start_quarter = (tk.musical_start_position - tk.musical_start_last_bar);
        double end_quarter = (tk.musical_end_position - tk.musical_start_last_bar);

        // rate = 2 -> half
        // rate = 4 -> quarter
        // rate = 8 -> 8th..

        // duration of what we quantify in terms of quarters
        double musical_quant_dur = rate / 4.;
        double start_quant = std::floor(start_quarter * musical_quant_dur);
        double end_quant = std::floor(end_quarter * musical_quant_dur);

        if(start_quant != end_quant)
        {
          // Date to quantify is the next one :
          auto quant_date = tk.musical_start_last_bar + (start_quant + 1) * 4. / rate;
          std::cerr << start_quant << " " << end_quant <<  " => " << quant_date << std::endl;


          double musical_tick_duration = tk.musical_end_position - tk.musical_start_position;
          double ratio = quant_date / musical_tick_duration;
          time_value dt = tk.date - tk.prev_date; // TODO should be tick_offset
          next_bar_start = tk.prev_date + dt * ratio;
        }
      }

      if(next_bar_start)
      {
        std::cerr << "next bar start: " << *next_bar_start << std::endl;
        sync.set_trigger_date(*next_bar_start);
        auto diff_date = *next_bar_start - tk.prev_date;
        // compute the "fake max" date at which intervals must end for this to
        // work
        for (auto& ev : sync.get_time_events())
        {
          maxReachedEv.push_back(ev.get());
          for (const auto& cst : ev->previous_time_intervals())
          {
            m_itv_end_map.insert(
                std::make_pair(cst.get(), cst->get_date() + diff_date));
          }

          std::cerr << "adding overtick : " << diff_date << " => " << tk.date - tk.prev_date - diff_date << std::endl;
          m_overticks.insert(std::make_pair(&sync, overtick{0, tk.date - tk.prev_date - diff_date, 0}));
        }
        sync.observe_expression(false);
        return sync_status::RETRY;
      }

      return sync_status::NOT_READY;
    }
  }

  // trigger the time sync
  sync.m_is_being_triggered = false;

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

scenario::sync_status scenario::process_this(
    time_sync& sync, small_event_vec& pendingEvents,
    small_event_vec& maxReachedEvents, interval_set& started,
    interval_set& stopped, ossia::time_value tick_offset,
    const ossia::token_request& req)
{
  // prepare to remember which event changed its status to PENDING
  // because it is needed in time_sync::trigger
  pendingEvents.clear();
  auto activeCount = sync.get_time_events().size();
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
    if (sync.m_evaluating)
    {
      sync.m_evaluating = false;
      sync.trigger_request = false;
      sync.left_evaluation.send();
    }

    return scenario::sync_status::NOT_READY;
  }

  return trigger_sync(
      sync, pendingEvents, maxReachedEvents, started, stopped, tick_offset, req,
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

void scenario::state_impl(const ossia::token_request& req)
{
  std::cerr << "start tick: " << req.prev_date << " -> " << req.date << std::endl;
  node->request(req);
  // ossia::logger().info("scenario::state starts");
  // if (date != m_lastDate)
  {
    auto prev_last_date = m_lastDate;
    m_lastDate = req.date;

    // Duration of this tick.
    time_value tick_ms
        = (prev_last_date == Infinite) ? req.date : (req.date - prev_last_date);

    m_overticks.clear();
    m_endNodes.clear();
    m_retry_syncs.clear();

    m_retry_syncs.container.reserve(8);
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

          if(n->is_autotrigger())
            n->m_evaluating = true;

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
      scenario::sync_status res = process_this(
          *n, m_pendingEvents, m_maxReachedEvents, m_runningIntervals,
          m_runningIntervals, req.offset, req);
      switch (res)
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

      interval.set_parent_speed(req.speed);

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
              interval.tick_offset(tick, offset, req);
          }
          else
          {
            if (max_tick != 0)
            {
              interval.tick_offset_speed_precomputed(max_tick, offset, req);
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
                cur.offset = req.offset + tick_ms - cur.max;
              }
            }
            else
            {
              m_overticks.insert(
                  node_it,
                  std::make_pair(
                      end_node, overtick{ot, ot, req.offset + tick_ms - ot}));
            }
          }
        }
        else
        {
          if (tick * s < cst_old_date)
          {
            if (tick != 0)
              interval.tick_offset(tick, offset, req);
          }
          else
          {
            if (tick != 0)
            {
              interval.tick_offset_speed_precomputed(ossia::time_value{-cst_old_date.impl}, offset, req);
            }
          }
          // no overtick support for running backwards for now - we just stop at zero
        }
      }
      else
      {
        interval.tick_offset(tick, offset, req);
      }
      if (interval.get_date() >= interval.get_min_duration())
        m_endNodes.insert(end_node);
    };


    // First check timesyncs already past their min
    // for any that may have a quantization setting

    for (time_interval* interval : m_runningIntervals)
    {
      if (interval->get_date() >= interval->get_min_duration())
      {
        const auto end_node = &interval->get_end_event().get_time_sync();
        if(end_node->has_sync_rate())
        {
          m_endNodes.insert(end_node);
          process_this(
                *end_node, m_pendingEvents, m_maxReachedEvents, m_runningIntervals,
                m_runningIntervals, req.offset, req);
        }
      }
    }
    for (time_interval* interval : m_runningIntervals)
    {
      run_interval(*interval, tick_ms, req.offset);
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
          continue;
        }
        else
        {
          const time_value remaining_tick
              = (mode == PROGRESS_MAX) ? it->second.max : it->second.min;

          const auto offset = req.offset + tick_ms - remaining_tick;
          const_cast<overtick&>(it->second).offset = offset;
          for (const auto& interval : ev.next_time_intervals())
          {
            std::cerr << "executing overtick: " <<remaining_tick << " ; " << offset << std::endl;
            run_interval(*interval, remaining_tick, offset);
          }
        }
      }
      // std::cerr << "END EVENT" << std::endl;

      m_maxReachedEvents.clear();

      for (auto node : m_endNodes)
      {
        auto it = m_overticks.find(node);
        sync_status status{};
        if (it != m_overticks.end())
        {
          status = process_this(
              *node, m_pendingEvents, m_maxReachedEvents, m_runningIntervals,
              m_runningIntervals, it->second.offset, req);
        }
        else
        {
          status = process_this(
              *node, m_pendingEvents, m_maxReachedEvents, m_runningIntervals,
              m_runningIntervals, req.offset, req);
        }

        switch(status)
        {
          case sync_status::DONE:
            m_retry_syncs.erase(node);
            break;
          case sync_status::NOT_READY:
            break;
          case sync_status::RETRY:
            m_retry_syncs.insert(node);
            break;
        }
      }

      m_endNodes = std::move(m_retry_syncs);


    } while (!m_maxReachedEvents.empty() || !m_endNodes.empty());
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
