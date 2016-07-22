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
#include "Editor/TimeNode.h"
#include "Editor/Value.h"

#include "Network/Address.h"
#include "Network/Device.h"
#include "Network/Node.h"
#include "Network/Protocol/Local.h"

using namespace OSSIA;

#include <thread>
#include <iostream>

using namespace std;

void constraint_callback(TimeValue position, TimeValue date, std::shared_ptr<StateElement> element)
{
    element->launch();
}

void event_callback(TimeEvent::Status newStatus)
{}

void float_address_callback(const Value * v)
{}

void int_address_callback(const Value * v)
{}

int main()
{
    auto local_protocol = Local::create();
    auto local_device = Device::create(local_protocol, "test");
    
    local_device->emplace(local_device->children().begin(), "float");
    auto float_address = local_device->children().front()->createAddress(Type::FLOAT);
    float_address->addCallback([&] (const Value* v) { float_address_callback(v); });
    
    auto int_address = local_device->children().front()->createAddress(Type::INT);
    int_address->addCallback([&] (const Value* v) { int_address_callback(v); });
    
    auto curve = Curve<float, int>::create();
    auto linearSegment = CurveSegmentLinear<int>::create(curve);
    curve->setInitialValue(0);
    curve->addPoint(0.5, 5, linearSegment);
    curve->addPoint(1., 10, linearSegment);
    Behavior b(curve);
    auto mapper = Mapper::create(float_address, int_address, &b);
    
    auto start_node = TimeNode::create();
    auto end_node = TimeNode::create();
    auto start_event = *(start_node->emplace(start_node->timeEvents().begin(), event_callback));
    auto end_event = *(end_node->emplace(end_node->timeEvents().begin(), &event_callback));
    auto constraint = TimeConstraint::create(&constraint_callback, start_event, end_event, 100.);
    constraint->addTimeProcess(mapper);
    
    constraint->setGranularity(10.);
    constraint->start();
    
    while (constraint->getRunning())
    {
        double position = constraint->getPosition();
        const Float* current_float = static_cast<const Float*>(float_address->getValue());
        const Int* current_int = static_cast<const Int*>(int_address->getValue());

        std::cout << "at " << position << " : float = " << current_float->value << ", int = " << current_int->value << std::endl;
        
        std::this_thread::sleep_for( std::chrono::milliseconds(10));
        
        float_address->pushValue(new Float(current_float->value + 0.1));
    }
}
