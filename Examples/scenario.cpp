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
    // create a scenario
    auto scenario = Scenario::create();

    // create first node of the scenario
    auto startScenarioNode = TimeNode::create();
    scenario->setStartNode(startScenarioNode);

    // create last node of the scenario
    auto endScenarioNode = TimeNode::create();
    scenario->setEndNode(endScenarioNode);

    // create a time constraint of 2000 ms
    TimeValue duration(2000);
    auto constraint = TimeConstraint::create(duration, duration, duration);

    // add time contraint from start to the end of the scenario
    scenario->addConstraint(constraint, scenario->getStartNode(), scenario->getEndNode());

    // tell the scenario to kill children processes when it ends
    scenario->setKiller(true);

    // play the scenario
    scenario->play();

    return 0;
}
