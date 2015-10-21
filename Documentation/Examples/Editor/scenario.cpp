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
#include "Editor/Curve.h"
#include "Editor/CurveSegment.h"
#include "Editor/CurveSegment/CurveSegmentLinear.h"
#include "Editor/CurveSegment/CurveSegmentPower.h"
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
#include "Network/Protocol/Local.h"

using namespace OSSIA;
using namespace std;

void local_play_callback(const Value * v);
void local_test_callback(const Value * v);

void main_constraint_callback(const TimeValue& position, const TimeValue& date, shared_ptr<StateElement> element);
void first_constraint_callback(const TimeValue& position, const TimeValue& date, shared_ptr<StateElement> element);
void second_constraint_callback(const TimeValue& position, const TimeValue& date, shared_ptr<StateElement> element);
void event_callback(TimeEvent::Status newStatus);

shared_ptr<TimeConstraint> main_constraint;

int main()
{
    /*
     Network setup
     */

    // create a Local device "i-score"
    auto local_protocol = Local::create();
    auto local_device = Device::create(local_protocol, "i-score");

    // add a /play address
    auto local_play_node = *(local_device->emplace(local_device->children().cend(), "play"));
    auto local_play_address = local_play_node->createAddress(Value::Type::BOOL);

    // attach /play address to a callback
    local_play_address->addCallback(local_play_callback);

    // add a /test address
    auto local_test_node = *(local_device->emplace(local_device->children().cend(), "test"));
    auto local_test_address = local_test_node->createAddress(Value::Type::TUPLE);

    // attach /test address to their callback
    local_test_address->addCallback(local_test_callback);

    // filter repetitions
    local_test_address->setRepetitionFilter(true);

    /*
     Main Scenario setup
     */

    // create the start and the end TimeNodes
    auto main_start_node = TimeNode::create();
    auto main_end_node = TimeNode::create();

    // create TimeEvents inside TimeNodes and make them interactive to the /play address
    auto main_start_event = *(main_start_node->emplace(main_start_node->timeEvents().begin(), &event_callback));
    auto main_end_event = *(main_end_node->emplace(main_end_node->timeEvents().begin(), &event_callback));

    // create the main Scenario
    auto main_scenario = Scenario::create();

    // create the main TimeConstraint
    TimeValue main_duration(5000.);
    main_constraint = TimeConstraint::create(main_constraint_callback, main_start_event, main_end_event, main_duration);

    // add the scenario to the main TimeConstraint
    main_constraint->addTimeProcess(main_scenario);

    /*
     Main Scenario edition : creation of a two TimeConstraints
     */

    // get the start node of the main Scenario
    auto scenario_start_node = main_scenario->getStartTimeNode();

    // create a TimeNode
    auto first_end_node = TimeNode::create();

    // create a TimeEvent inside the scenario start node without Expression
    auto first_start_event = *(scenario_start_node->emplace(scenario_start_node->timeEvents().begin(), &event_callback));

    // create a TimeEvent inside the end node without Expression
    auto first_end_event = *(first_end_node->emplace(first_end_node->timeEvents().begin(), &event_callback));

    // create a TimeConstraint between the two TimeEvents
    TimeValue first_duration(1500.);
    auto first_constraint = TimeConstraint::create(first_constraint_callback, first_start_event, first_end_event, first_duration, first_duration, first_duration);

    // add the first TimeConstraint to the main Scenario
    main_scenario->addTimeConstraint(first_constraint);

    // create a TimeNode
    auto second_end_node = TimeNode::create();

    // create a TimeEvent inside the end node without Expression
    auto second_end_event = *(second_end_node->emplace(second_end_node->timeEvents().begin(), &event_callback));

    // create a TimeConstraint between the two TimeEvents
    TimeValue second_duration(2000.);
    auto second_constraint = TimeConstraint::create(second_constraint_callback, first_end_event, second_end_event, second_duration, second_duration, second_duration);

    // add the second TimeConstraint to the main Scenario
    main_scenario->addTimeConstraint(second_constraint);

    /*
     Main Scenario edition : make an event interactive
     */

    // create an expression : /i-score/test >= {0.7, 0.7, 0.7}
    Destination local_test(local_test_node);
    Tuple threshold1 = {new Float(0.7), new Float(0.7), new Float(0.7)};
    auto next_expression1 = ExpressionAtom::create(&local_test,
                                                  ExpressionAtom::Operator::GREATER_THAN_OR_EQUAL,
                                                  &threshold1);

    // set first end event expression to make it interactive
    first_end_event->setExpression(next_expression1);

    /*
     Main Scenario edition : creation of two Automations
     */

    // create a linear curve to drive all element of the Tuple value from 0. to 1.
    auto first_curve = Curve<double, float>::create();
    auto first_linearSegment = CurveSegmentLinear<float>::create(first_curve);

    first_curve->setInitialValue(0.);
    first_curve->addPoint(1., 1., first_linearSegment);

    // create a power curve to drive all element of the Tuple value from 0. to 2.
    auto second_curve = Curve<double, float>::create();
    auto second_powerSegment = CurveSegmentPower<float>::create(first_curve);
    second_powerSegment->setPower(0.5);

    second_curve->setInitialValue(1.);
    second_curve->addPoint(1., 2., second_powerSegment);

    // create a Tuple value of 3 Behavior values based on the same curve
    vector<const Value*> t_first_curves = {new Behavior(first_curve), new Behavior(first_curve), new Behavior(first_curve)};
    Tuple first_curves(t_first_curves);

    // create a Tuple value of 3 Behavior values based on the same curve
    vector<const Value*> t_second_curves = {new Behavior(second_curve), new Behavior(second_curve), new Behavior(second_curve)};
    Tuple second_curves(t_second_curves);

    // create a first Automation to drive /test address by the linear curve
    auto first_automation = Automation::create(local_test_address, &first_curves);

    // create a second Automation to drive /test address by the power curve
    auto second_automation = Automation::create(local_test_address, &second_curves);

    // add the first Automation to the first TimeConstraint
    first_constraint->addTimeProcess(first_automation);

    // add the second Automation to the second TimeConstraint
    second_constraint->addTimeProcess(second_automation);

    // add "/test 0. 0. 0." message to first Automation's start State
    Tuple zero = {new Float(0.), new Float(0.), new Float(0.)};
    auto first_start_message = Message::create(local_test_address, &zero);
    first_automation->getStartState()->stateElements().push_back(first_start_message);

    // add "/test 1. 1. 1." message to first Automation's end State
    Tuple one = {new Float(1.), new Float(1.), new Float(1.)};
    auto first_end_message = Message::create(local_test_address, &one);
    first_automation->getEndState()->stateElements().push_back(first_end_message);

    // add "/test 2. 2. 2." message to second Automation's end State
    Tuple two = {new Float(2.), new Float(2.), new Float(2.)};
    auto second_end_message = Message::create(local_test_address, &two);
    second_automation->getEndState()->stateElements().push_back(second_end_message);

    /*
     Main Scenario operation : miscellaneous
     */

    // display TimeNode's date
    cout << "first_start_node date = " << scenario_start_node->getDate() << endl;
    cout << "first_end_node date = " << first_end_node->getDate() << endl;
    cout << "second_end_node date = " << second_end_node->getDate() << endl;

    // change main TimeConstraint speed, granularity and offset
    main_constraint->setSpeed(1.);
    main_constraint->setGranularity(50.);
    
    // set minimal duration of the first constraint to 1000 ms
    first_constraint->setDurationMin(1000.);

    // change first and second TimeConstraint speed and granularity
    first_constraint->setSpeed(1.);
    first_constraint->setGranularity(150.);
    second_constraint->setSpeed(1.);
    second_constraint->setGranularity(150.);

    cout << "***** START *****" << endl;

    // play the main TimeConstraint
    local_play_address->pushValue(&True);

    // wait the main TimeConstraint end
    while (main_constraint->getRunning())
        ;

    cout << "***** END *****" << endl;

    // set minimal duration of the first constraint to 500 ms
    first_constraint->setDurationMin(750.);

    // set first end time node expression to make it interactive
    // (instead of first end event)
    first_end_node->setExpression(next_expression1);
    first_end_event->setExpression(ExpressionTrue);

    cout << "***** START *****" << endl;

    // play it again faster starting at 500 ms
    main_constraint->setSpeed(2.);
    main_constraint->setOffset(500.);
    local_play_address->pushValue(&True);

    // wait the main TimeConstraint end
    while (main_constraint->getRunning())
        ;

    cout << "***** END *****" << endl;
}

void local_play_callback(const Value * v)
{
    if (v->getType() == Value::Type::BOOL)
    {
        Bool * b = (Bool*)v;
        if (b->value)
            main_constraint->start();
        else
            main_constraint->stop();
    }
}

void local_test_callback(const Value * v)
{
    cout << "/i-score/test = ";

    if (v->getType() == Value::Type::TUPLE)
    {
        Tuple * t = (Tuple*)v;

        for (auto e : t->value)
        {
            if (e->getType() == Value::Type::FLOAT)
            {
                Float * f = (Float*)e;
                cout << f->value << " ";
            }
        }
    }

    cout << endl;
}

void main_constraint_callback(const TimeValue& position, const TimeValue& date, shared_ptr<StateElement> element)
{
    element->launch();
    cout << "Main Constraint : " << double(position) << ", " << double(date) << endl;
}

void first_constraint_callback(const TimeValue& position, const TimeValue& date, shared_ptr<StateElement> element)
{
    cout << "First Constraint : " << double(position) << ", " << double(date) << endl;

    // don't launch element here as the element produced by the first TimeConstraint is handled by the main TimeConstraint
}

void second_constraint_callback(const TimeValue& position, const TimeValue& date, shared_ptr<StateElement> element)
{
    cout << "Second Constraint : " << double(position) << ", " << double(date) << endl;

    // don't launch element here as the element produced by the second TimeConstraint is handled by the main TimeConstraint
}

void event_callback(TimeEvent::Status newStatus)
{
    switch (newStatus)
    {
        case TimeEvent::Status::NONE:
        {
            cout << "Event NONE" << endl;
            break;
        }
        case TimeEvent::Status::PENDING:
        {
            cout << "Event PENDING" << endl;
            break;
        }
        case TimeEvent::Status::HAPPENED:
        {
            cout << "Event HAPPENED" << endl;
            break;
        }
        case TimeEvent::Status::DISPOSED:
        {
            cout << "Event DISPOSED" << endl;
            break;
        }
    }
}
