/*!
 * \file Scenario_Impl.h
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#ifndef SCENARIO_IMPL_H_
#define SCENARIO_IMPL_H_

#include "Editor/Scenario.h"

#include "TTScore.h"

namespace OSSIA
{
  class Scenario_Impl : public Scenario
  {
  public:
    
    // Constructors, destructor, assignment
    Scenario_Impl();
    virtual ~Scenario_Impl();
    
    // Feature
    virtual void play() const;
    
    // Navigation
    virtual std::set<TimeBox*> getTimeBoxes() const;
    virtual std::set<TimeNode*> getTimeNodes() const;
    
    // Edition
    virtual void addTimeBox(const TimeBox&, const TimeNode & startNode);
    virtual void addTimeBox(
                    const TimeBox&,
                    const TimeNode & startNode,
                    const TimeNode & endNode);
    
    // Accessors
    // internal TimeNodes
    virtual TimeNode & getStartNode() const;
    virtual void setStartNode(const TimeNode&);
    virtual TimeNode & getEndNode() const;
    virtual void setEndNode(const TimeNode&);
    
    /** Implementation Specific
     @details use mutable members to break constness of the API because Jamoma doesn't take care of it.
     */
  };
}

#endif /* SCENARIO_IMPL_H_ */
