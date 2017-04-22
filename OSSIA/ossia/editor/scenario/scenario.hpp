#pragma once

#include <ossia/editor/scenario/clock.hpp>
#include <ossia/editor/scenario/time_process.hpp>
#include <ossia/editor/state/state.hpp>
#include <boost/container/flat_map.hpp>
#include <ossia_export.h>

namespace ossia
{
class time_event;
class time_constraint;
class time_node;

class OSSIA_EXPORT scenario final :
    public time_process
{
  public:
    scenario();

    ~scenario();

    state_element offset(ossia::time_value) override;

    state_element state() override;

    void start() override;
    void stop() override;
    void pause() override;
    void resume() override;


    /*! add a #time_constraint and its #time_nodes into the scenario if they don't
     already be added
     \param std::shared_ptr<#time_constraint> to add */
    void add_time_constraint(std::shared_ptr<time_constraint>);

    /*! remove a #time_constraint from the scenario without removing any #time_node
     \param std::shared_ptr<#time_constraint> to remove */
    void remove_time_constraint(const std::shared_ptr<time_constraint>&);

    /*! add a #time_node into the scenario if it is not already added
     \param std::shared_ptr<#time_node> to add */
    void add_time_node(std::shared_ptr<time_node>);

    /*! remove a #time_node from the scenario
     \param std::shared_ptr<#time_node> to remove */
    void remove_time_node(const std::shared_ptr<time_node>&);

    /*! get the node from where the scenario starts
     \return std::shared_ptr<#time_node> start node */
    const std::shared_ptr<time_node>& get_start_time_node() const;

    /*! get all TimeNodes of the scenario
     \return #Container<#time_node> */
    const ptr_container<time_node>& get_time_nodes() const;

    /*! get all TimeConstraints of the scenario
     \return #Container<#time_constraint> */
    const ptr_container<time_constraint>& get_time_constraints() const;

    /*! order all HAPPENED TimeEvents into mOffetEventMap */
    void process_offset(time_node&, time_value);

  private:
    ptr_container<time_constraint> m_constraints;
    ptr_container<time_node> m_nodes; // list of all TimeNodes of the scenario
                                         // (the first is the start node, the
                                         // second is the end node)

    // a temporary list to order all past events to build the
    // offset state
    boost::container::flat_map<time_value, std::shared_ptr<time_event>> m_pastEvents;

    ossia::state m_lastState;
};
}
