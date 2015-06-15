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
    /* 
     Network setup
     */
    
    // create a Local device "i-score"
    Local local_device_parameters{};
    auto local_device = Device::create(local_device_parameters, "i-score");
    
    // add a /play address
    auto local_play_node = localDevice->emplace(localDevice->children().cend(), "play");
    auto local_play_address = (*local_play_node)->createAddress(AddressValue::Type::BOOL);
    
    /* 
     Main Scenario setup
     */
    
    // create two TimeNodes for the start and the end
    TimeNode main_start = TimeNode::create(); // implicit creation of a first event into the node
    TimeNode main_end = TimeNode::create(); // implicit creation of an end event into the node
    
    // get TimeEvents of the start and end TimeNodes to make them interactive to /play address
    TimeEvent main_start_event =  main_start->timeEvents()->begin();
    TimeEvent main_end_event =  main_end->timeEvents()->begin();
    
    // create "/play == true" and "/play == false" expressions
    auto play_expression_start = ExpressionAtom::create(ExpressionValue::create(local_play_address),
                                                        ExpressionAtom::Operator::EQUAL,
                                                        ExpressionValue::create(Bool(true)));
    
    auto play_expression_end = ExpressionAtom::create(ExpressionValue::create(local_play_address),
                                                      ExpressionAtom::Operator::EQUAL,
                                                      ExpressionValue::create(Bool(false)));
    
    // make start event to be interactive to "/play == true" and end event to be interactive to "/play == false"
    main_start_event->setExpression(play_expression_start);
    main_end_event->setExpression(play_expression_end);
    
    // create the main TimeConstraint passing the first event of each TimeNode
    TimeValue main_duration(30000);
    auto main_constraint = TimeConstraint::create(main_duration, main_start_event, main_end_event);
    
    // create the main Scenario and add it to the main TimeConstraint
    auto main_scenario = Scenario::create(); // implicit creation of a start state and end state, implicit creation of first and last TimeNode
    main_constraint->timeProcesses().push_back(main_scenario);
    
    /* 
     Main Scenario edition
     */

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

    return 0;
}