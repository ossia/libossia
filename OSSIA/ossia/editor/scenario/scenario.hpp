#pragma once

#include <ossia/editor/scenario/clock.hpp>
#include <ossia/editor/scenario/time_process.hpp>
#include <ossia/editor/state/state.hpp>
#include <boost/container/flat_map.hpp>
#include <boost/container/flat_set.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/detail/ptr_set.hpp>
#include <ossia_export.h>
#include <set>
namespace ossia
{
class graph;
class time_event;
class time_interval;
class time_sync;
using interval_set = boost::container::flat_set<time_interval*>;
using sync_set = boost::container::flat_set<time_sync*>;
using small_sync_vec = ossia::small_vector<time_sync*, 4>;
using small_event_vec = ossia::small_vector<time_event*, 4>;
struct overtick
{
  ossia::time_value min;
  ossia::time_value max;
  ossia::time_value offset;
};
using overtick_map = boost::container::flat_map<time_sync*, overtick>;

class scenario;
using scenario_graph_vertex = time_sync*;
using scenario_graph_edge = time_interval*;


struct scenario_graph
{
    using graph_t = boost::adjacency_list<
      boost::vecS,
      boost::vecS,
      boost::undirectedS,
      scenario_graph_vertex,
      scenario_graph_edge>;

    scenario& scenar;
    graph_t graph;

    scenario_graph(scenario& sc);

    small_sync_vec get_roots() const;

    void add_vertice(scenario_graph_vertex timeSync);
    void add_edge(scenario_graph_edge itv);
    void remove_vertice(scenario_graph_vertex timeSync);
    void remove_edge(scenario_graph_edge itv);

    void reset_component(ossia::time_sync& sync) const;
    ossia::small_vector<ossia::time_sync*, 4> sibling_roots(
        const ossia::time_sync& sync) const;

  private:
     void update_components_cache() const;
     mutable std::vector<int> m_components_cache;
     mutable bool dirty = false;
     tsl::hopscotch_map<
         const time_sync*,
         graph_t::vertex_descriptor> vertices;
     tsl::hopscotch_map<
         const time_interval*,
         graph_t::edge_descriptor> edges;

};

class scenario_node : public ossia::graph_node
{
public:
  scenario_node();
  std::string_view label() const override;
  void run(ossia::token_request t, ossia::execution_state&) override;
  // TODO: add here the states to play.
  // They have to work with inter-ticks.
};


class OSSIA_EXPORT scenario final : public time_process
{
public:
  scenario();
  ~scenario() override;

  void offset(ossia::time_value, double pos) override;

  void state(ossia::time_value date, double pos, ossia::time_value tick_offset, double gspeed) override;

  void start() override;
  void stop() override;
  void pause() override;
  void resume() override;

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

  void reset_subgraph(
      const ptr_container<time_sync>&
      , const ptr_container<time_interval>&
      , time_sync& root);

  bool is_root_sync(ossia::time_sync& sync) const;
private:


  ptr_container<time_interval> m_intervals;
  ptr_container<time_sync> m_nodes; // list of all TimeSyncs of the scenario
                                    // (the first is the start node, the
                                    // second is the end node)

  interval_set m_runningIntervals;
  sync_set m_waitingNodes;
  small_sync_vec m_rootNodes;
  overtick_map m_overticks; // used as cache
  boost::container::flat_map<time_interval*, time_value> m_itv_end_map;
  sync_set m_endNodes; // used as cache
  scenario_graph m_sg; // used as cache
  bool process_this(
      time_sync& node, small_event_vec& statusChangedEvents,
      interval_set& started, interval_set& stopped,
      ossia::time_value tick_offset);
  static void make_happen(
      time_event& event, interval_set& started, interval_set& stopped,
      ossia::time_value tick_offset);
  static void make_dispose(time_event& event, interval_set& stopped);
  bool trigger_sync(time_sync& node,
                    small_event_vec::iterator pendingBegin, small_event_vec::iterator pendingEnd,
                    interval_set& started, interval_set& stopped,
                    ossia::time_value tick_offset, bool maxReached);
};
}
