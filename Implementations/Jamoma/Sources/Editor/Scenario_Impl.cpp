/*!
 * \file Scenario_Impl.cpp
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#include "Scenario_Impl.h"

namespace OSSIA
{
  Scenario* Scenario::create()
  {
    // todo : we shouldn't init each time we create an object ...
    TTFoundationInit("/usr/local/jamoma/");
    TTModularInit("/usr/local/jamoma/");
    TTScoreInit("/usr/local/jamoma/");
    
    return new Scenario_Impl();
  }
  
  Scenario_Impl::Scenario_Impl()
  {}
  
  Scenario_Impl::~Scenario_Impl()
  {}
  
  void Scenario_Impl::play() const
  {}
  
  std::set<TimeBox*> Scenario_Impl::getTimeBoxes() const
  {}
  
  std::set<TimeNode*> Scenario_Impl::getTimeNodes() const
  {}
  
  void Scenario_Impl::addTimeBox(const TimeBox&, const TimeNode & startNode)
  {}
  
  void Scenario_Impl::addTimeBox(const TimeBox&,
                  const TimeNode & startNode,
                  const TimeNode & endNode)
  {}
  
  TimeNode & Scenario_Impl::getStartNode() const
  {}
  
  void Scenario_Impl::setStartNode(const TimeNode&)
  {}
  
  TimeNode & Scenario_Impl::getEndNode() const
  {}
  
  void Scenario_Impl::setEndNode(const TimeNode&)
  {}
}