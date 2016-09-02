#pragma once

#include <ossia/editor/scenario/clock.hpp>
#include <ossia/editor/scenario/time_process.hpp>
#include <ossia/editor/state/state.hpp>
#include <list>
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

    state_element offset(time_value) override;

    state_element state() override;

    void start() override;
    void stop() override;
    void pause() override;
    void resume() override;


    /*! add a #TimeConstraint and its #TimeNodes into the scenario if they don't
     already be added
     \param std::shared_ptr<#TimeConstraint> to add */
    void addTimeConstraint(std::shared_ptr<time_constraint>);

    /*! remove a #TimeConstraint from the scenario without removing any #TimeNode
     \param std::shared_ptr<#TimeConstraint> to remove */
    void removeTimeConstraint(const std::shared_ptr<time_constraint>&);

    /*! add a #TimeNode into the scenario if it is not already added
     \param std::shared_ptr<#TimeNode> to add */
    void addTimeNode(std::shared_ptr<time_node>);

    /*! remove a #TimeNode from the scenario
     \param std::shared_ptr<#TimeNode> to remove */
    void removeTimeNode(const std::shared_ptr<time_node>&);

    /*! get the node from where the scenario starts
     \return std::shared_ptr<#TimeNode> start node */
    const std::shared_ptr<time_node>& getStartTimeNode() const;

    /*! get all TimeNodes of the scenario
     \return #Container<#TimeNode> */
    const ptr_container<time_node>& timeNodes() const;

    /*! get all TimeConstraints of the scenario
     \return #Container<#TimeConstraint> */
    const ptr_container<time_constraint>& timeConstraints() const;

    /*! order all HAPPENED TimeEvents into mOffetEventMap */
    void process_offset(time_node&, time_value);

  private:
    ptr_container<time_constraint> mTimeContraints;
    ptr_container<time_node> mTimeNodes; // list of all TimeNodes of the scenario
                                         // (the first is the start node, the
                                         // second is the end node)

    std::list<std::pair<time_value, std::shared_ptr<time_event>>>
        mPastEventList; // a temporary list to order all past events to build the
                        // offset state

    ossia::state mLastState;
};
}
