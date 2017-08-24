// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
/*!
 * \file curve.cpp
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#include "Editor/Curve.h"
#include "Editor/CurveSegment.h"
#include "Editor/CurveSegment/CurveSegmentLinear.h"
#include "Editor/Mapper.h"
#include "Editor/State.h"
#include "Editor/TimeConstraint.h"
#include "Editor/TimeEvent.h"
#include "Editor/TimeSync.h"
#include "Editor/value.h"

#include "Network/Address.h"
#include "Network/Device.h"
#include "Network/Node.h"
#include "Network/Protocol/Local.h"

using namespace ossia;

#include <thread>
#include <iostream>

using namespace std;

void constraint_callback(ossia::time_value position, time_value date, std::shared_ptr<StateElement> element)
{
    element->launch();
}

void event_callback(TimeEvent::Status newStatus)
{}

void float_parameter_callback(const value& v)
{}

void int_parameter_callback(const value& v)
{}

int main()
{
    auto local_protocol = Local::create();
    auto local_device = Device::create(local_protocol, "test");
    
    local_device->emplace(local_device->children().begin(), "float");
    auto float_address = local_device->children().front()->create_parameter(Type::FLOAT);
    float_address->addCallback([&] (const value& v) { float_parameter_callback(v); });
    
    auto int_address = local_device->children().front()->create_parameter(Type::INT);
    int_address->addCallback([&] (const value& v) { int_parameter_callback(v); });
    
    auto curve = Curve<float, int>::create();
    auto linearSegment = CurveSegmentLinear<int>::create(curve);
    curve->setInitialvalue(0);
    curve->add_point(0.5, 5, linearSegment);
    curve->add_point(1., 10, linearSegment);
    Behavior b(curve);
    auto mapper = Mapper::create(float_address, int_address, &b);
    
    auto start_node = TimeSync::create();
    auto end_node = TimeSync::create();
    auto start_event = *(start_node->emplace(start_node->get_time_events().begin(), event_callback));
    auto end_event = *(end_node->emplace(end_node->get_time_events().begin(), &event_callback));
    auto constraint = TimeConstraint::create(&constraint_callback, start_event, end_event, 100.);
    constraint->add_time_process(mapper);
    
    constraint->set_granularity(10.);
    constraint->start();
    
    while (constraint->running())
    {
        double position = constraint->getPosition();
        const Float* current_float = static_cast<const Float*>(float_address->getvalue());
        const Int* current_int = static_cast<const Int*>(int_address->getvalue());

        std::cout << "at " << position << " : float = " << current_float->value << ", int = " << current_int->value << std::endl;
        
        std::this_thread::sleep_for( std::chrono::milliseconds(10));
        
        float_address->pushvalue(new Float(current_float->value + 0.1));
    }
}
