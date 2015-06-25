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

#include "Editor/Automation.h"
#include "Editor/Clock.h"
#include "Editor/Expression.h"
#include "Editor/ExpressionAtom.h"
#include "Editor/Message.h"
#include "Editor/Scenario.h"
#include "Editor/State.h"
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

void local_play_callback(const Value * v);
void local_test_callback(const Value * v);

shared_ptr<TimeConstraint> main_constraint;

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
    
    // attach /play address to a callback
    local_play_address->setValueCallback(local_play_callback);
    
    // add a /test address
    auto local_test_node = *(local_device->emplace(local_device->children().cend(), "test"));
    auto local_test_address = local_test_node->createAddress(Value::Type::FLOAT);
    
    // attach /test address to a callback
    local_test_address->setValueCallback(local_test_callback);
    
    /*
     Main Scenario setup
     */
    
    // create the start and the end TimeNodes
    auto main_start_node = TimeNode::create();
    auto main_end_node = TimeNode::create();
    
    // create "/play == true" and "/play == false" Expressions
    Destination local_play(local_play_node);
    auto play_expression_start = ExpressionAtom::create(&local_play,
                                                        ExpressionAtom::Operator::EQUAL,
                                                        &True);
    
    auto play_expression_end = ExpressionAtom::create(&local_play,
                                                      ExpressionAtom::Operator::EQUAL,
                                                      &False);
    
    // create TimeEvents inside TimeNodes and make them interactive to the /play address
    auto main_start_event = *(main_start_node->emplace(main_start_node->timeEvents().begin(), play_expression_start));
    auto main_end_event = *(main_end_node->emplace(main_end_node->timeEvents().begin(), play_expression_end));

    // create the main Scenario
    auto main_scenario = Scenario::create();
    
    // create the main TimeConstraint
    TimeValue main_duration(5000.);
    main_constraint = TimeConstraint::create(main_start_event, main_end_event, main_duration);
    
    // add the scenario to the main TimeConstraint
    main_constraint->addTimeProcess(main_scenario);

    /* 
     Main Scenario edition : creation of a TimeConstraint
     */
    
    // get the start node of the main Scenario
    auto scenario_start_node = main_scenario->getStartNode();
    
    // create a TimeNode
    auto first_end_node = TimeNode::create();

    // create a TimeEvent inside the scenario start node without Expression
    auto first_start_event = *(scenario_start_node->emplace(scenario_start_node->timeEvents().begin()));
    
    // create a TimeEvent inside the end node without Expression
    auto first_end_event = *(first_end_node->emplace(first_end_node->timeEvents().begin()));
    
    // create a TimeConstraint between the two TimeEvents
    TimeValue first_duration(1500.);
    auto first_constraint = TimeConstraint::create(first_start_event, first_end_event, first_duration, first_duration, first_duration);
    
    // add the first TimeConstraint to the main Scenario
    main_scenario->addConstraint(first_constraint);

    /*
     Main Scenario edition : creation of an Automation
     */
    
    // create an Automation
    auto first_automation = Automation<double>::create();
    
    // add it to the first TimeConstraint
    first_constraint->addTimeProcess(first_automation);
    
    // add "/test 0." message to Automation's start State
    Float zero(0.);
    auto first_start_message = Message::create(local_test_address, &zero);
    first_automation->getStartState()->stateElements().push_back(first_start_message);
    
    // add "/test 1." message to Automation's end State
    Float one(1.);
    auto first_end_message = Message::create(local_test_address, &one);
    first_automation->getEndState()->stateElements().push_back(first_end_message);
    
/*
    //! \todo create a curve
    
    //! \todo create an independent state into an event
    //auto new_event = first_end_event->timeEvents()->begin();
    //event.addState(...);
*/
    
    /*
     Main Scenario operation : miscellaneous
     */
    
    // display TimeNode's date
    cout << "first_start_node date = " << scenario_start_node->getDate() << "\n";
    cout << "first_end_node date = " << first_end_node->getDate() << "\n";
    
    // change scenario speed
    main_scenario->getClock()->setSpeed(0.5);
    
    // play the scenario
    local_play_address->sendValue(&True);

    while (true)
        ;
}

void local_play_callback(const Value * v)
{
    if (v->getType() == Value::Type::BOOL)
    {
        Bool * b = (Bool*)v;
        if (b->value)
            main_constraint->play();
        else
            ; //! main_constraint->stop(); \todo how to stop execution ?
    }
}

void local_test_callback(const Value * v)
{
    if (v->getType() == Value::Type::FLOAT)
    {
        Float * f = (Float*)v;
        cout << "/i-score/test = "<< f->value << "\n";
    }
}
