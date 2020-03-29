#pragma once
#include <ossia/detail/flat_map.hpp>
#include <ossia/detail/flat_set.hpp>
#include <ossia/detail/pod_vector.hpp>
#include <ossia/detail/ptr_container.hpp>
#include <ossia/detail/ptr_set.hpp>
#include <ossia/detail/small_vector.hpp>
#include <ossia/editor/scenario/time_process.hpp>
#include <ossia/editor/scenario/time_value.hpp>

#include <boost/graph/adjacency_list.hpp>

#include <ossia_export.h>
namespace ossia
{
class graph;
class time_event;
class time_interval;
class time_sync;
using interval_set = ossia::flat_set<time_interval*>;
using sync_set = ossia::flat_set<time_sync*>;
using small_sync_vec = ossia::small_vector<time_sync*, 4>;
using small_event_vec = ossia::small_vector<time_event*, 4>;
struct overtick
{
  ossia::time_value min;
  ossia::time_value max;
  ossia::time_value offset;
};
using overtick_map = ossia::flat_map<time_sync*, overtick>;

class scenario;
using scenario_graph_vertex = time_sync*;
using scenario_graph_edge = time_interval*;

struct scenario_graph
{
  using graph_t = boost::adjacency_list<
      boost::vecS, boost::vecS, boost::undirectedS, scenario_graph_vertex,
      scenario_graph_edge>;

  scenario& scenar;
  graph_t graph;

  scenario_graph(scenario& sc);

  void add_vertice(scenario_graph_vertex timeSync);
  void add_edge(scenario_graph_edge itv);
  void remove_vertice(scenario_graph_vertex timeSync);
  void remove_edge(scenario_graph_edge itv);

  void reset_component(ossia::time_sync& sync) const;
private:
  void recompute_maps();
  void update_components_cache() const;
  mutable ossia::int_vector m_components_cache;
  mutable bool dirty = false;
  ossia::ptr_map<const time_sync*, graph_t::vertex_descriptor> vertices;
  ossia::ptr_map<const time_interval*, graph_t::edge_descriptor> edges;
};

class OSSIA_EXPORT scenario final : public looping_process<scenario>
{
  friend class looping_process<scenario>;
public:
  scenario();
  ~scenario() override;

  /*! add a #time_interval and its #time_syncs into the scenario if they
   don't
   already be added
   \param std::shared_ptr<#time_interval> to add */
  void add_time_interval(std::shared_ptr<time_interval>);

  /*! remove a #time_interval from the scenario without removing any
   #time_sync
   \param std::shared_ptr<#time_interval> to remove */
  void remove_time_interval(const std::shared_ptr<time_interval>&);

  /*! add a #time_sync into the scenario if it is not already added
   \param std::shared_ptr<#time_sync> to add */
  void add_time_sync(std::shared_ptr<time_sync>);

  /*! remove a #time_sync from the scenario
   \param std::shared_ptr<#time_sync> to remove */
  void remove_time_sync(const std::shared_ptr<time_sync>&);

  /*! get the node from where the scenario starts
   \return std::shared_ptr<#time_sync> start node */
  const std::shared_ptr<time_sync>& get_start_time_sync() const;

  /*! get all TimeSyncs of the scenario
   \return #Container<#time_sync> */
  const ptr_container<time_sync>& get_time_syncs() const;

  /*! get all TimeIntervals of the scenario
   \return #Container<#time_interval> */
  const ptr_container<time_interval>& get_time_intervals() const;

  friend struct scenario_graph;
  void offset_impl(ossia::time_value) override;

  void state_impl(const ossia::token_request& req);

  void start() override;
  void stop() override;
  void pause() override;
  void resume() override;

  void transport_impl(ossia::time_value offset) override;
  void mute_impl(bool) override;

  small_sync_vec get_roots() const noexcept;

  void reset_subgraph(
      const ptr_container<time_sync>&, const ptr_container<time_interval>&,
      time_sync& root);

private:
  ptr_container<time_interval> m_intervals;
  ptr_container<time_sync> m_nodes; // list of all TimeSyncs of the scenario
                                    // (the first is the start node, the
                                    // second is the end node)

  interval_set m_runningIntervals;
  sync_set m_waitingNodes;
  small_sync_vec m_rootNodes;
  small_event_vec m_pendingEvents;
  small_event_vec m_maxReachedEvents;
  overtick_map m_overticks; // used as cache
  ossia::flat_map<time_interval*, time_value> m_itv_end_map;
  sync_set m_retry_syncs; // used as cache
  sync_set m_endNodes; // used as cache
  scenario_graph m_sg; // used as cache

  ossia::time_value m_lastDate{ossia::Infinite};

  static void make_happen(
      time_event& event, interval_set& started, interval_set& stopped,
      ossia::time_value tick_offset, const ossia::token_request& tok);

  static void make_dispose(time_event& event, interval_set& stopped);

  sync_status process_this(time_sync& node, small_event_vec& pendingEvents,
      small_event_vec& maxReachedEvents, interval_set& started,
      interval_set& stopped, ossia::time_value tick_offset, const token_request& tok);

  sync_status trigger_sync(time_sync& node, small_event_vec& pending, small_event_vec& maxReachedEv,
      interval_set& started, interval_set& stopped,
      ossia::time_value tick_offset, const token_request& req, bool maxReached);

  sync_status process_this_musical(
      time_sync& node, small_event_vec& pendingEvents,
      small_event_vec& maxReachedEvents, ossia::time_value tick_offset,
      const token_request& tok) noexcept;

  sync_status trigger_sync_musical(
      time_sync& node, small_event_vec& maxReachedEvents,
      ossia::time_value tick_offset, const token_request& req,
      bool maxReached) noexcept;

  sync_status quantify_time_sync(
      time_sync& sync, const ossia::token_request& tk) noexcept;

  sync_status trigger_quantified_time_sync(
      time_sync& sync, bool& maximalDurationReached) noexcept;

  void run_interval(
      ossia::time_interval& interval,
      const ossia::token_request& tk,
      const time_value& tick_ms,
      ossia::time_value tick,
      ossia::time_value offset);
};
}
