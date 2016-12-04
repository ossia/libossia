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

#include <ossia/ossia.hpp>

using namespace ossia;
using namespace std;

void local_play_callback(const value& v);
void local_test_callback(const value& v);

void main_constraint_callback(time_value position, time_value date, const state& element);
void first_constraint_callback(time_value position, time_value date, const state& element);
void second_constraint_callback(time_value position, time_value date, const state& element);
void event_callback(time_event::Status newStatus);

shared_ptr<time_constraint> main_constraint;

int main()
{
    /*
     Network setup
     */

    // create a Local device "i-score"
    ossia::net::generic_device device{std::make_unique<ossia::net::local_protocol>(), "i-score"};

    // add a /play address
    auto local_play_node = device.createChild("play");
    auto local_play_address = local_play_node->createAddress(val_type::BOOL);

    // attach /play address to a callback
    local_play_address->add_callback(local_play_callback);

    // add a /test address
    auto local_test_node = device.createChild("test");
    auto local_test_address = local_test_node->createAddress(val_type::TUPLE);

    // attach /test address to their callback
    local_test_address->add_callback(local_test_callback);

    // filter repetitions
    local_test_address->setRepetitionFilter(repetition_filter::ON);

    /*
     Main scenario setup
     */

    // create the start and the end TimeNodes
    auto main_start_node = std::make_shared<time_node>();
    auto main_end_node = std::make_shared<time_node>();

    // create time_events inside TimeNodes and make them interactive to the /play address
    auto main_start_event = *(main_start_node->emplace(main_start_node->timeEvents().begin(), &event_callback));
    auto main_end_event = *(main_end_node->emplace(main_end_node->timeEvents().begin(), &event_callback));

    // create the main scenario
    auto main_scenario_ptr = std::make_unique<scenario>();
    scenario* main_scenario = main_scenario_ptr.get();

    // create the main time_constraint
    time_value main_duration(5000.);
    main_constraint = std::make_shared<time_constraint>(
                             main_constraint_callback,
                             *main_start_event,
                             *main_end_event,
                             main_duration,
                             main_duration,
                             main_duration);

    // add the scenario to the main time_constraint
    main_constraint->addTimeProcess(std::move(main_scenario_ptr));

    /*
     Main scenario edition : creation of a two time_constraints
     */

    // get the start node of the main scenario
    auto scenario_start_node = main_scenario->getStartTimeNode();

    // create a TimeNode
    auto first_end_node = std::make_shared<time_node>();

    // create a time_event inside the scenario start node without Expression
    auto first_start_event = *(scenario_start_node->emplace(scenario_start_node->timeEvents().begin(), &event_callback));

    // create a time_event inside the end node without Expression
    auto first_end_event = *(first_end_node->emplace(first_end_node->timeEvents().begin(), &event_callback));

    // create a time_constraint between the two time_events
    time_value first_duration(1500.);
    std::shared_ptr<time_constraint> first_constraint = std::make_shared<time_constraint>(
                              first_constraint_callback,
                              *first_start_event,
                              *first_end_event,
                              first_duration,
                              first_duration,
                              first_duration);

    // add the first time_constraint to the main scenario
    main_scenario->addTimeConstraint(first_constraint);

    // create a TimeNode
    auto second_end_node = std::make_shared<time_node>();

    // create a time_event inside the end node without Expression
    auto second_end_event = *(second_end_node->emplace(second_end_node->timeEvents().begin(), &event_callback));

    // create a time_constraint between the two time_events
    time_value second_duration(2000.);
    auto second_constraint = std::make_shared<time_constraint>(
                               second_constraint_callback,
                               *first_end_event,
                               *second_end_event,
                               second_duration,
                               second_duration,
                               second_duration);

    // add the second time_constraint to the main scenario
    main_scenario->addTimeConstraint(second_constraint);

    /*
     Main scenario edition : make an event interactive
     */

    // create an expression : /i-score/test >= {0.7, 0.7, 0.7}
    auto make_expr = [&] () {
      return expressions::make_expression_atom(
        Destination(*local_test_address),
        expressions::expression_atom::Comparator::GREATER_THAN_OR_EQUAL,
        std::vector<ossia::value>{0.7, 0.7, 0.7});
    };

    // set first end event expression to make it interactive
    first_end_event->setExpression(make_expr());

    /*
     Main scenario edition : creation of two Automations
     */

    // create a linear curve to drive all element of the Tuple value from 0. to 1.
    auto first_curve = std::make_shared<curve<double, float>>();
    curve_segment_linear<float> first_linearSegment;

    first_curve->setInitialPointAbscissa(0.);
    first_curve->setInitialPointOrdinate(0.);
    first_curve->addPoint(first_linearSegment, 1., 1.);

    // create a power curve to drive all element of the Tuple value from 0. to 2.
    auto second_curve = std::make_shared<curve<double, float>>();
    auto second_powerSegment = curve_segment_power<float>{}(0.5);

    second_curve->setInitialPointOrdinate(1.);
    second_curve->addPoint(second_powerSegment, 1., 2.);

    // create a Tuple value of 3 behavior values based on the same curve
    std::vector<behavior> first_curves{first_curve, first_curve, first_curve};

    // create a Tuple value of 3 behavior values based on the same curve
    std::vector<behavior> second_curves{second_curve, second_curve, second_curve};

    // create a first Automation to drive /test address by the linear curve
    auto first_automation = std::make_unique<automation>(*local_test_address, first_curves);

    // create a second Automation to drive /test address by the power curve
    auto second_automation = std::make_unique<automation>(*local_test_address, second_curves);

    // add the first Automation to the first time_constraint
    first_constraint->addTimeProcess(std::move(first_automation));

    // add the second Automation to the second time_constraint
    second_constraint->addTimeProcess(std::move(second_automation));

    // add "/test 0. 0. 0." message to first time_constraint's start State
    message first_start_message{*local_test_address, std::vector<ossia::value>{0., 0., 0.}};
    first_constraint->getStartEvent().addState(first_start_message);

    // add "/test 1. 1. 1." message to first time_constraint's end State
    message first_end_message{*local_test_address, std::vector<ossia::value>{1., 1., 1.}};
    first_constraint->getEndEvent().addState(first_end_message);

    // add "/test 2. 2. 2." message to second time_constraint's end State
    message second_end_message{*local_test_address, std::vector<ossia::value>{2., 2., 2.}};
    second_constraint->getStartEvent().addState(second_end_message);

    /*
     Main scenario operation : miscellaneous
     */

    // display TimeNode's date
    cout << "first_start_node date = " << scenario_start_node->getDate() << endl;
    cout << "first_end_node date = " << first_end_node->getDate() << endl;
    cout << "second_end_node date = " << second_end_node->getDate() << endl;

    // change main time_constraint speed, granularity and offset
    main_constraint->setSpeed(1._tv);
    main_constraint->setGranularity(50._tv);

    // set minimal duration of the first constraint to 1000 ms
    first_constraint->setDurationMin(1000._tv);

    // change first and second time_constraint speed and granularity
    first_constraint->setSpeed(1._tv);
    first_constraint->setGranularity(50._tv);
    second_constraint->setSpeed(1._tv);
    second_constraint->setGranularity(50._tv);

    cout << "***** START *****" << endl;

    // play the main time_constraint
    //local_play_address->pushvalue(&True);
    main_constraint->start();

    // wait the main time_constraint end
    while (main_constraint->getRunning())
        ;

    cout << "***** END *****" << endl;

    // set minimal duration of the first constraint to 500 ms
    first_constraint->setDurationMin(750._tv);

    // set first end time node expression to make it interactive
    // (instead of first end event)
    first_end_node->setExpression(make_expr());
    first_end_event->setExpression(expressions::make_expression_true());

    cout << "***** START *****" << endl;

    // play it again faster
    main_constraint->setSpeed(2._tv);

    // start at 500 ms (and launch the state at this time)
    main_constraint->offset(500._tv).launch();

    local_play_address->pushValue(true);

    // wait the main time_constraint end
    while (main_constraint->getRunning())
        ;

    cout << "***** END *****" << endl;
}

void local_play_callback(const value& v)
{
    if (v.getType() == val_type::BOOL)
    {
        auto b = v.get<bool>();
        if (b)
            main_constraint->start();
        else
            main_constraint->stop();
    }
}

void local_test_callback(const value& v)
{
    cout << "/i-score/test = ";

    if (v.getType() == val_type::TUPLE)
    {
      auto t = v.get<std::vector<ossia::value>>();

        for (auto e : t)
        {
            if (e.getType() == val_type::FLOAT)
            {
                auto f = e.get<float>();
                cout << f << " ";
            }
        }
    }

    cout << endl;
}

void main_constraint_callback(time_value position, time_value date, const state& element)
{
    element.launch();
    cout << "Main Constraint : " << double(position) << ", " << double(date) << endl;
}

void first_constraint_callback(time_value position, time_value date, const state& element)
{
    cout << "First Constraint : " << double(position) << ", " << double(date) << endl;

    // don't launch element here as the element produced by the first time_constraint is handled by the main time_constraint
}

void second_constraint_callback(time_value position, time_value date, const state& element)
{
    cout << "Second Constraint : " << double(position) << ", " << double(date) << endl;

    // don't launch element here as the element produced by the second time_constraint is handled by the main time_constraint
}

void event_callback(time_event::Status newStatus)
{
    switch (newStatus)
    {
        case time_event::Status::NONE:
        {
            cout << "Event NONE" << endl;
            break;
        }
        case time_event::Status::PENDING:
        {
            cout << "Event PENDING" << endl;
            break;
        }
        case time_event::Status::HAPPENED:
        {
            cout << "Event HAPPENED" << endl;
            break;
        }
        case time_event::Status::DISPOSED:
        {
            cout << "Event DISPOSED" << endl;
            break;
        }
    }
}
