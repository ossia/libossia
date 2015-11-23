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

#include "Editor/Clock.h"
#include "Editor/Loop.h"
#include "Editor/TimeConstraint.h"
#include "Editor/TimeEvent.h"
#include "Editor/TimeNode.h"

using namespace OSSIA;
using namespace std;

void main_constraint_callback(const TimeValue& position, const TimeValue& date, shared_ptr<StateElement> element);
void main_start_event_callback(TimeEvent::Status newStatus);
void main_end_event_callback(TimeEvent::Status newStatus);

void loop_pattern_constraint_callback(const TimeValue& position, const TimeValue& date, shared_ptr<StateElement> element);
void loop_pattern_start_event_callback(TimeEvent::Status newStatus);
void loop_pattern_end_event_callback(TimeEvent::Status newStatus);

void print_event_status(TimeEvent::Status newStatus, std::string name);

int main()
{
    // create start and the end TimeNodes
    auto start_node = TimeNode::create();
    auto end_node = TimeNode::create();
    
    /*
     Main Constraint setup
     */
    
    auto main_start_event = *(start_node->emplace(start_node->timeEvents().begin(), &main_start_event_callback));
    auto main_end_event = *(end_node->emplace(end_node->timeEvents().begin(), &main_end_event_callback));

    TimeValue main_duration(2000.);
    auto main_constraint = TimeConstraint::create(main_constraint_callback, main_start_event, main_end_event, main_duration);
    
    main_constraint->setGranularity(10.);
    
    /*
     Loop Process setup
     */
    
    auto loop = Loop::create(90., loop_pattern_constraint_callback, loop_pattern_start_event_callback, loop_pattern_end_event_callback);
    
    main_constraint->addTimeProcess(loop);

    /*
     Display TimeNode's date
     */
    
    cout << "start node date = " << start_node->getDate() << endl;
    cout << "end node date = " << end_node->getDate() << endl;
    cout << "loop start node date = " << loop->getPatternStartTimeNode()->getDate() << endl;
    cout << "loop end node date = " << loop->getPatternEndTimeNode()->getDate() << endl;
    
    /*
     Execute
     */
    
    start_node->happen();
    
    // look at the console to see how things are repeated during the main constraint duration
    while (main_constraint->getRunning())
        ;
}

void main_constraint_callback(const TimeValue& position, const TimeValue& date, shared_ptr<StateElement> element)
{
    cout << "Main Constraint : " << double(position) << ", " << double(date) << endl;
}

void main_start_event_callback(TimeEvent::Status newStatus)
{
    print_event_status(newStatus, "Main Start");
}

void main_end_event_callback(TimeEvent::Status newStatus)
{
    print_event_status(newStatus, "Main End");
}

void loop_pattern_constraint_callback(const TimeValue& position, const TimeValue& date, shared_ptr<StateElement> element)
{
    cout << "Loop Constraint : " << double(position) << ", " << double(date) << endl;
}

void loop_pattern_start_event_callback(TimeEvent::Status newStatus)
{
    print_event_status(newStatus, "Loop Pattern Start");
}

void loop_pattern_end_event_callback(TimeEvent::Status newStatus)
{
    print_event_status(newStatus, "Loop Pattern End");
}

void print_event_status(TimeEvent::Status newStatus, std::string name)
{
    switch (newStatus)
    {
        case TimeEvent::Status::NONE:
        {
            cout << name << " Event NONE" << endl;
            break;
        }
        case TimeEvent::Status::PENDING:
        {
            cout << name << " Event PENDING" << endl;
            break;
        }
        case TimeEvent::Status::HAPPENED:
        {
            cout << name << " Event HAPPENED" << endl;
            break;
        }
        case TimeEvent::Status::DISPOSED:
        {
            cout << name << " Event DISPOSED" << endl;
            break;
        }
    }
}
