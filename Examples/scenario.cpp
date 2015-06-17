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

#include "Editor/Expression.h"
#include "Editor/ExpressionAtom.h"
#include "Editor/Scenario.h"
#include "Editor/TimeConstraint.h"
#include "Editor/TimeEvent.h"
#include "Editor/TimeNode.h"
#include "Editor/TimeValue.h"
#include "Editor/Value.h"

#include "Network/Address.h"
#include "Network/Device.h"
#include "Network/Node.h"
#include "Network/Protocol.h"

using namespace OSSIA;
using namespace std;

int main()
{
    /* 
     Network setup
     */
    
    // create a Local device "i-score"
    Local local_device_parameters{};
    auto local_device = Device::create(local_device_parameters, "i-score");
    
    // add a /play address
    auto local_play_node = *(local_device->emplace(local_device->children().cend(), "play"));
    auto local_play_address = local_play_node->createAddress(Value::Type::BOOL);
    
    /* 
     Main Scenario setup
     */
    
    // create the start and the end TimeNodes
    auto main_start = TimeNode::create();
    auto main_end = TimeNode::create();
    
    // create "/play == true" and "/play == false" Expressions
    Destination local_play(local_play_node);
    Bool _true(true); //! \todo create Bool::true
    Bool _false(false); //! \todo create Bool::false
    auto play_expression_start = ExpressionAtom::create(&local_play,
                                                        ExpressionAtom::Operator::EQUAL,
                                                        &_true);
    
    auto play_expression_end = ExpressionAtom::create(&local_play,
                                                      ExpressionAtom::Operator::EQUAL,
                                                      &_false);
    
    // create TimeEvents inside TimeNodes whitout state but interactive to the /play address
    auto main_start_event = *(main_start->emplace(main_start->timeEvents().begin(), nullptr, play_expression_start));
    auto main_end_event = *(main_end->emplace(main_end->timeEvents().begin(), nullptr, play_expression_end));

    // create the main TimeConstraint
    TimeValue main_duration(30000);
    auto main_constraint = TimeConstraint::create(main_duration, main_start_event, main_end_event, main_duration, main_duration);
    
    // create the main Scenario and add it to the main TimeConstraint
    auto main_scenario = Scenario::create(); // implicit creation of a start state and end state, implicit creation of first and last TimeNode
    main_constraint->timeProcesses().push_back(main_scenario);

    /* 
     Main Scenario edition
     */
/*
    // get the start TimeEvent of the main Scenario
    auto first_start_event = scenario->getStartNode()->timeEvents()->begin();
    
    // create a TimeNode and get its TimeEvent
    TimeNode first_end_node = TimeNode::create();
    auto first_end_event = first_start_node->timeEvents()->begin();
    
    // create a TimeConstraint between the two TimeEvents
    TimeValue first_duration(3000);
    auto first_constraint = TimeConstraint::create(first_duration, first_start_event, first_end_event);
    
    // add the new TimeConstraint to the main Scenario
    main_scenario->addConstraint(main_scenario->getStartNode(), first_constraint, first_end_node);
    
    // create an automation into the new TimeConstraint
    auto first_automation = Automation<double>::create();
    first_constraint->timeProcesses().push_back(first_automation);
    
    //! \todo create a curve
    
    //! \todo create an independent state into an event
    //auto new_event = first_end_event->timeEvents()->begin();
    //event.addState(...);
    
    // play the scenario
    scenario->play();
*/
    return 0;
}