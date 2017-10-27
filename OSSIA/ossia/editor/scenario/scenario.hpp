#pragma once

#include <ossia/editor/scenario/clock.hpp>
#include <ossia/editor/scenario/time_process.hpp>
#include <ossia/editor/state/state.hpp>
#include <boost/container/flat_map.hpp>
#include <boost/container/flat_set.hpp>
#include <ossia/dataflow/graph_node.hpp>
#include <ossia_export.h>
#include <set>
namespace ossia
{
class graph;
class time_event;
class time_interval;
class time_sync;
using interval_set = boost::container::flat_set<time_interval*>;
struct overtick
{
  ossia::time_value min;
  ossia::time_value max;
};
using overtick_map = boost::container::flat_map<time_sync*, overtick>;

class scenario_node : public ossia::graph_node
{
public:
  scenario_node();
  void run(ossia::token_request t, ossia::execution_state&) override;
};


class OSSIA_EXPORT scenario final : public time_process
{
public:
  scenario();

  ~scenario();

  state_element offset(ossia::time_value, double pos) override;

  state_element state(ossia::time_value date, double pos, ossia::time_value tick_offset) override;

  void start(ossia::state& st) override;
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

private:

  using small_sync_vec = chobo::small_vector<time_sync*, 4>;
  using small_event_vec = chobo::small_vector<time_event*, 4>;

  ptr_container<time_interval> m_intervals;
  ptr_container<time_sync> m_nodes; // list of all TimeSyncs of the scenario
                                    // (the first is the start node, the
                                    // second is the end node)

  ossia::state m_lastState;

  interval_set m_runningIntervals;
  interval_set intervals_started, intervals_stopped;
  small_sync_vec m_waitingNodes;
  overtick_map m_overticks;
  boost::container::flat_set<time_sync*> m_endNodes;
  void process_this(
      time_sync& node, small_event_vec& statusChangedEvents,
      interval_set& started, interval_set& stopped, ossia::state& st);
  void make_happen(
      time_event& event, interval_set& started, interval_set& stopped, ossia::state& st);
  void make_dispose(time_event& event, interval_set& stopped);
};

ossia::state_element tick_interval(time_interval& c, ossia::time_value tick, ossia::time_value offset);
}
