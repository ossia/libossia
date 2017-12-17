// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/editor/scenario/scenario.hpp>
#include <ossia/editor/scenario/time_interval.hpp>
#include <ossia/editor/scenario/time_event.hpp>
#include <ossia/editor/scenario/time_sync.hpp>
#include <ossia/editor/scenario/detail/scenario_graph.hpp>
#include <ossia/dataflow/graph.hpp>

#include <ossia/detail/algorithms.hpp>
#include <ossia/detail/logger.hpp>
#include <ossia/editor/exceptions.hpp>
#include <cassert>
#include <hopscotch_map.h>
#include <iostream>
#include <map>
#include <set>

namespace ossia
{
scenario_node::scenario_node()
{
  // todo maybe we can optimize by having m_outlets == m_inlets
  // this way no copy.
  m_inlets.push_back(ossia::make_inlet<ossia::audio_port>());
  m_outlets.push_back(ossia::make_outlet<ossia::audio_port>());
}

std::string_view scenario_node::label() const
{
  return "Scenario";
}


void scenario_node::run(token_request t, execution_state&)
{
  auto i = m_inlets[0]->data.target<ossia::audio_port>();
  auto o = m_outlets[0]->data.target<ossia::audio_port>();
  o->samples = i->samples;
}

scenario::scenario():
  m_sg{*this}
{
  // create the start TimeSync
  add_time_sync(std::make_shared<time_sync>());
  node = std::make_shared<scenario_node>();
}

scenario::~scenario()
{
  for (auto& timesync : m_nodes)
  {
    timesync->cleanup();
  }
}

bool scenario::is_root_sync(ossia::time_sync& sync) const
{
  auto bool_expr = sync.get_expression().target<ossia::expressions::expression_bool>();

  auto res = !bool_expr || !bool_expr->evaluate() || &sync == m_nodes[0].get();
  res &= ossia::all_of(
            sync.get_time_events(),
            [] (const std::shared_ptr<ossia::time_event>& ev) {
        return ev->previous_time_intervals().empty();
      });
  return res;
}
void scenario::start()
{
  for(auto& node : m_nodes)
  {
    if(is_root_sync(*node))
      m_waitingNodes.insert(node.get());
  }
  m_rootNodes = m_sg.get_roots();

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
      cst.tick();
    }
    // the interval is in the future
    else if (
        startStatus == time_event::status::NONE
        && endStatus == time_event::status::NONE)
    {
    }
    // error
    else
    {
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
  m_waitingNodes.clear();
  m_overticks.clear();
  m_itv_end_map.clear();
  m_lastDate = time_value{};
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

void scenario::add_time_interval(
    std::shared_ptr<time_interval> itv)
{
  // store the TimeInterval if it is not already stored
  if (!contains(m_intervals, itv))
  {
    m_sg.add_edge(itv.get());
    m_intervals.push_back(std::move(itv));
  }
}

void scenario::remove_time_interval(
    const std::shared_ptr<time_interval>& itv)
{
  if(itv)
  {
    m_sg.remove_edge(itv.get());
    auto it = ossia::find(m_runningIntervals, itv.get());
    if(it != m_runningIntervals.end())
      m_runningIntervals.erase(it);
    m_itv_end_map.erase(itv.get());
    remove_one(m_intervals, itv);
  }
}

void scenario::add_time_sync(std::shared_ptr<time_sync> timeSync)
{
  // store a TimeSync if it is not already stored
  if (!contains(m_nodes, timeSync))
  {
    m_sg.add_vertice(timeSync.get());
    m_nodes.push_back(std::move(timeSync));
  }
}

void scenario::remove_time_sync(const std::shared_ptr<time_sync>& timeSync)
{
  if(timeSync)
  {
    m_sg.remove_vertice(timeSync.get());
    m_waitingNodes.erase(timeSync.get());
    auto it = ossia::find(m_rootNodes, timeSync.get());
    if(it != m_rootNodes.end())
      m_rootNodes.erase(it);
    m_overticks.erase(timeSync.get());
    m_endNodes.erase(timeSync.get());

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
    const ptr_container<time_sync>& syncs
    , const ptr_container<time_interval>& itvs
    , time_sync& root)
{
  for(const std::shared_ptr<ossia::time_sync>& sync : syncs)
  {
    sync->reset();
  }

  for(const std::shared_ptr<ossia::time_interval>& itv : itvs)
  {
    itv->stop();
    m_runningIntervals.erase(itv.get());
    m_itv_end_map.erase(itv.get());
  }
}

void scenario_graph::add_vertice(scenario_graph_vertex timeSync)
{
  vertices[timeSync] = boost::add_vertex(timeSync, graph);
  dirty = true;
}

void scenario_graph::add_edge(scenario_graph_edge itv)
{
  edges[itv] = boost::add_edge(
        vertices[&itv->get_start_event().get_time_sync()],
      vertices[&itv->get_end_event().get_time_sync()],
      itv,
      graph).first;
  dirty = true;
}

void scenario_graph::remove_vertice(scenario_graph_vertex timeSync)
{
  auto it = vertices.find(timeSync);
  if(it != vertices.end())
  {
    boost::clear_vertex(it->second, graph);
    //boost::remove_vertex(it->second, graph);
    vertices.erase(it);
    dirty = true;
  }
}

void scenario_graph::remove_edge(scenario_graph_edge itv)
{
  auto it = edges.find(itv);
  if(it != edges.end())
  {
    boost::remove_edge(it->second, graph);
    edges.erase(it);
    dirty = true;
  }
}

}
