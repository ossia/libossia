/*!
 * \file scenario.cpp
 *
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#include <iostream>
#include <memory>

#include "Editor/Scenario.h"
#include "Editor/TimeConstraint.h"
#include "Editor/TimeNode.h"
#include "Editor/TimeValue.h"

using namespace OSSIA;
using namespace std;

int main()
{
    // creae a scenario
    auto scenario = Scenario::create();
    
    // create a time constraint
    TimeValue t;
    auto constraint = TimeConstraint::create(t, t, t);
    
    auto startNode = TimeNode::create();
    scenario->setStartNode(startNode);
  
    scenario->addConstraint(*constraint, *(scenario->getStartNode()));

    auto endNode = TimeNode::create();
    
    scenario->addConstraint(*constraint, *scenario->getStartNode(), *endNode);

    scenario->setEndNode(endNode);
    scenario->setKiller(true);
    scenario->play();

    return 0;
}
