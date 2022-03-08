// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/dataflow/nodes/forward_node.hpp>
#include <ossia/editor/exceptions.hpp>
#include <ossia/editor/scenario/scenario.hpp>
#include <ossia/editor/scenario/time_event.hpp>
#include <ossia/editor/scenario/time_interval.hpp>
#include <ossia/editor/scenario/time_sync.hpp>
#include <iostream>

namespace ossia
{

scenario::scenario() : m_sg{*this}
{
  // create the start TimeSync
  node = std::make_shared<ossia::nodes::scenario>();
  add_time_sync(std::make_shared<time_sync>());
  this->m_nodes.front()->set_start(true);
}

scenario::~scenario()
{
  for (auto& timesync : m_nodes)
  {
    timesync->cleanup();
  }
}

void scenario::start()
{
  m_runningIntervals.container.reserve(m_intervals.size());
  m_pendingEvents.reserve(m_nodes.size() * 2);
  m_maxReachedEvents.reserve(m_nodes.size() * 2);
  m_overticks.container.reserve(m_nodes.size());
  m_itv_end_map.container.reserve(m_intervals.size());
  m_endNodes.container.reserve(m_nodes.size());
  m_retry_syncs.container.reserve(8);
  m_rootNodes = get_roots();

  for (auto node : m_rootNodes)
  {
    m_waitingNodes.insert(node);
  }

  for (auto& node: m_nodes)
  {
    node->reset();
  }

  // start each TimeInterval if possible
  for (const auto& timeInterval : m_intervals)
  {
    time_interval& cst = *timeInterval;
    time_event::status startStatus = cst.get_start_event().get_status();
    time_event::status endStatus = cst.get_end_event().get_status();

    // the interval is in the past
    if (startStatus == time_event::status::HAPPENED
        && endStatus == time_event::status::HAPPENED)
    {
    }
    // the start of the interval is pending
    else if (
        startStatus == time_event::status::PENDING
        && endStatus == time_event::status::NONE)
    {
    }
    // the interval is supposed to be running
    else if (
        startStatus == time_event::status::HAPPENED
        && endStatus == time_event::status::NONE)
    {
      m_runningIntervals.insert(&cst);
      cst.start();
      // TODO cst.tick_current();
    }
    // the interval starts in the void and ends on a timesync that did
    // execute
    else if (
        startStatus == time_event::status::NONE
        && endStatus == time_event::status::HAPPENED)
    {
    }
    // the end of the interval is pending
    else if (
        startStatus == time_event::status::HAPPENED
        && endStatus == time_event::status::PENDING)
    {
      m_runningIntervals.insert(&cst);
      cst.start();
      //const auto tok = ossia::token_request{};
      //cst.tick_current(0_tv, tok);
    }
    // the interval is in the future
    else if (
        startStatus == time_event::status::NONE
        && endStatus == time_event::status::NONE)
    {
    }
    else if (
        startStatus == time_event::status::DISPOSED
        && endStatus == time_event::status::DISPOSED)
    {
        // the interval is started by a false condition
    }
    // error
    else
    {
      std::cerr << "scenario_impl::start:  start: " << (int)startStatus << " ; end: " << (int)endStatus << std::endl;
      throw execution_error(
          "scenario_impl::start: "
          "TimeEvent's status configuration of the "
          "TimeInterval is not handled");
    }
  }
}

void scenario::stop()
{
  // stop each running TimeIntervals
  for (const std::shared_ptr<ossia::time_interval>& timeInterval : m_intervals)
  {
    time_interval& cst = *timeInterval;
    cst.stop();
  }

  for (const std::shared_ptr<ossia::time_sync>& node : m_nodes)
  {
    node->reset();
  }

  m_runningIntervals.clear();
  m_itv_to_start.clear();
  m_itv_to_stop.clear();
  m_waitingNodes.clear();
  m_pendingEvents.clear();
  m_maxReachedEvents.clear();
  m_overticks.clear();
  m_itv_end_map.clear();
  m_last_date = ossia::Infinite;
}

void scenario::pause()
{
  // pause all running TimeIntervals
  for (const auto& timeInterval : m_intervals)
  {
    auto& cst = *timeInterval;
    cst.pause();
  }
}

void scenario::resume()
{
  // resume all running TimeIntervals
  for (const auto& timeInterval : m_intervals)
  {
    auto& cst = *timeInterval;
    cst.resume();
  }
}

void scenario::add_time_interval(std::shared_ptr<time_interval> itv)
{
  // store the TimeInterval if it is not already stored
  if (!contains(m_intervals, itv))
  {
    time_sync* end_root{};

    if(m_last_date != ossia::Infinite)
    {
      auto& t = itv->get_end_event().get_time_sync();
      if (t.is_start())
        end_root = &t;
    }
    m_sg.add_edge(itv.get());
    if(node->muted())
      itv->mute(true);
    m_intervals.push_back(std::move(itv));

    if(end_root)
    {
      m_waitingNodes.erase(end_root);
      m_rootNodes = get_roots();
    }
  }
}

void scenario::remove_time_interval(const std::shared_ptr<time_interval>& itv)
{
  if (itv)
  {
    m_sg.remove_edge(itv.get());

    if(m_last_date != ossia::Infinite)
    {
      auto& t = itv->get_end_event().get_time_sync();
      if (t.is_start())
      {
        m_waitingNodes.insert(&t);
      }
      m_rootNodes = get_roots();
    }
    if (auto it = ossia::find(m_runningIntervals, itv.get()); it != m_runningIntervals.end())
      m_runningIntervals.erase(it);
    if (auto it = ossia::find(m_itv_to_start, itv.get()); it != m_itv_to_start.end())
      m_itv_to_start.erase(it);
    if (auto it = ossia::find(m_itv_to_stop, itv.get()); it != m_itv_to_stop.end())
      m_itv_to_stop.erase(it);

    m_itv_end_map.erase(itv.get());

    remove_one(m_intervals, itv);
  }
}

void scenario::add_time_sync(std::shared_ptr<time_sync> timeSync)
{
  // store a TimeSync if it is not already stored
  if (!contains(m_nodes, timeSync))
  {
    auto& t = *timeSync;
    m_sg.add_vertice(&t);
    if(node->muted())
      t.mute(true);
    m_nodes.push_back(std::move(timeSync));

    if(m_last_date != ossia::Infinite)
    {
      if (t.is_start())
      {
        m_waitingNodes.insert(&t);
        m_rootNodes = get_roots();
      }
    }
  }
}

void scenario::remove_time_sync(const std::shared_ptr<time_sync>& timeSync)
{
  if (timeSync)
  {
    m_sg.remove_vertice(timeSync.get());
    m_waitingNodes.erase(timeSync.get());
    auto it = ossia::find(m_rootNodes, timeSync.get());
    if (it != m_rootNodes.end())
      m_rootNodes.erase(it);
    m_overticks.erase(timeSync.get());
    m_endNodes.erase(timeSync.get());
    m_retry_syncs.erase(timeSync.get());

    remove_one(m_nodes, timeSync);
  }
}

const std::shared_ptr<time_sync>& scenario::get_start_time_sync() const
{
  return m_nodes[0];
}

const ptr_container<time_sync>& scenario::get_time_syncs() const
{
  return m_nodes;
}

const ptr_container<time_interval>& scenario::get_time_intervals() const
{
  return m_intervals;
}
void scenario::reset_subgraph(
    const ptr_container<time_sync>& syncs,
    const ptr_container<time_interval>& itvs, time_sync& root)
{
  for (const std::shared_ptr<ossia::time_sync>& sync : syncs)
  {
    sync->reset();
  }

  for (const std::shared_ptr<ossia::time_interval>& itv : itvs)
  {
    itv->stop();
    m_runningIntervals.erase(itv.get());
    m_itv_end_map.erase(itv.get());
  }
}

void scenario::mute_impl(bool m)
{
  for (auto& itv : get_time_intervals())
  {
    itv->mute(m);
  }
  for (auto& s : get_time_syncs())
  {
    s->mute(m);
  }
}

void scenario::start_interval(time_interval& itv, double ratio)
{
  m_itv_to_start.emplace_back(quantized_interval{&itv, ratio});
}
void scenario::stop_interval(time_interval& itv, double ratio)
{
  m_itv_to_stop.emplace_back(quantized_interval{&itv, ratio});
}

void scenario_graph::add_vertice(scenario_graph_vertex timeSync)
{
  vertices[timeSync] = boost::add_vertex(timeSync, graph);
  recompute_maps();
  dirty = true;
}

void scenario_graph::add_edge(scenario_graph_edge itv)
{
  edges[itv] = boost::add_edge(
                   vertices[&itv->get_start_event().get_time_sync()],
                   vertices[&itv->get_end_event().get_time_sync()], itv, graph)
                   .first;
  recompute_maps();
  dirty = true;
}

void scenario_graph::remove_vertice(scenario_graph_vertex timeSync)
{
  auto it = vertices.find(timeSync);
  if (it != vertices.end())
  {
    boost::clear_vertex(it->second, graph);
    boost::remove_vertex(it->second, graph);
    recompute_maps();
    dirty = true;
  }
}

void scenario_graph::remove_edge(scenario_graph_edge itv)
{
  auto it = edges.find(itv);
  if (it != edges.end())
  {
    boost::remove_edge(it->second, graph);
    recompute_maps();
    dirty = true;
  }
}


void scenario_graph::recompute_maps()
{
  vertices.clear();
  edges.clear();
  {
    auto [vbegin,vend] = boost::vertices(graph);
    for(auto it = vbegin; it != vend; ++it)
    {
      auto k = *it;
      auto n = graph[k];
      assert(n);

      vertices.insert({n, k});
    }
  }
  {
    auto [ebegin,eend] = boost::edges(graph);
    for(auto it = ebegin; it != eend; ++it)
    {
      auto k = *it;
      auto n = graph[k];
      assert(n);

      edges.insert({n, k});
    }
  }

}
}
