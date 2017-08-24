#pragma once

#include <ossia/editor/scenario/clock.hpp>
#include <ossia/editor/scenario/time_process.hpp>
#include <ossia/editor/state/state.hpp>
#include <boost/container/flat_map.hpp>
#include <boost/container/flat_set.hpp>
#include <ossia_export.h>
#include <set>
namespace ossia
{
class time_event;
class time_constraint;
class time_sync;
using constraint_set = boost::container::flat_set<time_constraint*>;
struct overtick
{
  ossia::time_value min;
  ossia::time_value max;
};
using overtick_map = boost::container::flat_map<time_sync*, overtick>;
class OSSIA_EXPORT scenario final : public time_process
{
public:
  scenario();

  ~scenario();

  state_element offset(ossia::time_value, double pos) override;

  state_element state(ossia::time_value date, double pos) override;

  void start(ossia::state& st) override;
  void stop() override;
  void pause() override;
  void resume() override;

  /*! add a #time_constraint and its #time_syncs into the scenario if they
   don't
   already be added
   \param std::shared_ptr<#time_constraint> to add */
  void add_time_constraint(std::shared_ptr<time_constraint>);

  /*! remove a #time_constraint from the scenario without removing any
   #time_sync
   \param std::shared_ptr<#time_constraint> to remove */
  void remove_time_constraint(const std::shared_ptr<time_constraint>&);

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

  /*! get all TimeConstraints of the scenario
   \return #Container<#time_constraint> */
  const ptr_container<time_constraint>& get_time_constraints() const;

private:
  ptr_container<time_constraint> m_constraints;
  ptr_container<time_sync> m_nodes; // list of all TimeSyncs of the scenario
                                    // (the first is the start node, the
                                    // second is the end node)

  ossia::state m_lastState;

  constraint_set m_runningConstraints;
  constraint_set constraints_started, constraints_stopped;
  std::vector<time_sync*> m_waitingNodes;
  overtick_map m_overticks;
  boost::container::flat_set<time_sync*> m_endNodes;

  void process_this(
      time_sync& node, std::vector<time_event*>& statusChangedEvents,
      constraint_set& started, constraint_set& stopped, ossia::state& st);
  void make_happen(
      time_event& event, constraint_set& started, constraint_set& stopped, ossia::state& st);
  void make_dispose(time_event& event, constraint_set& stopped);
  void tick_constraint(time_constraint& c, time_value tick);
};
}
