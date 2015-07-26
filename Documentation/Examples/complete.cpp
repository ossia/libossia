/*!
 * \file complete.cpp
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#include <iostream>
#include <memory>
#include <functional>

#include "Editor/Automation.h"
#include "Editor/Clock.h"
#include "Editor/Curve.h"
#include "Editor/CurveSegment.h"
#include "Editor/CurveSegment/CurveSegmentLinear.h"
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

void explore(const shared_ptr<Node> node);
void printValue(const Value * v);
void printValueCallback(const Value * v);
void constraintCallback(const TimeValue& position, const TimeValue& date, shared_ptr<State> state);
void eventCallback(TimeEvent::Status newStatus);

int main()
{
    // Local device
    cout << "\nLocal device example\n";
    Local localDeviceParameters{};
    auto localDevice = Device::create(localDeviceParameters, "i-score");
    
    // Minuit device
    cout << "\nMinuit device example\n";
    Minuit minuitDeviceParameters{"127.0.0.1", 9998, 13579};
    auto minuitDevice = Device::create(minuitDeviceParameters, "newDevice");
    minuitDevice->updateNamespace();
    
    // find the /deg/bitdepth address
    shared_ptr<Address> bitdepthAddress;
    for (const auto& module : minuitDevice->children())
    {
        if (module->getName() == "deg")
        {
            for (const auto& parameter : module->children())
            {
                string parameter_name = parameter->getName();
                
                if (parameter_name == "bitdepth")
                {
                    cout << "\n/deg/bitdepth node found\n";
                    bitdepthAddress = parameter->getAddress();
                }
            }
        }
    }

    // create one curve to drive /deg/bitdepth
    auto curve = Curve<int>::create();
    auto linearSegment = CurveSegmentLinear<int>::create(curve);
    
    curve->setInitialValue(1);
    curve->addPoint(0.5, 24, linearSegment);
    curve->addPoint(1., 1, linearSegment);
    
    // create an Automation for /test address drived by one curve
    auto automation = Automation::create(bitdepthAddress, new Behavior(curve));
    
    // create the start and the end TimeNodes
    auto start_node = TimeNode::create();
    auto end_node = TimeNode::create();
    
    // create TimeEvents inside TimeNodes
    auto start_event = *(start_node->emplace(start_node->timeEvents().begin(), &eventCallback));
    auto end_event = *(end_node->emplace(end_node->timeEvents().begin(), &eventCallback));
    
    // create a TimeConstraint
    TimeValue duration(5000.);
    auto constraint = TimeConstraint::create(constraintCallback, start_event, end_event, duration);
    
    // add the Automation to the TimeConstraint
    constraint->addTimeProcess(automation);
    
    // go !
    constraint->play();
    
    // wait the TimeConstraint ends
    while (constraint->getRunning())
        ;
}

void printValue(const Value * v)
{
    switch (v->getType())
    {
        case Value::Type::IMPULSE :
        {
            cout << "-";
            break;
        }
        case Value::Type::BOOL :
        {
            Bool * b = (Bool*)v;
            cout << b->value;
            break;
        }
        case Value::Type::INT :
        {
            Int * i = (Int*)v;
            cout << i->value;
            break;
        }
        case Value::Type::FLOAT :
        {
            Float * f = (Float*)v;
            cout << f->value;
            break;
        }
        case Value::Type::CHAR :
        {
            Char * c = (Char*)v;
            cout << c->value;
            break;
        }
        case Value::Type::STRING :
        {
            String * s = (String*)v;
            cout << s->value;
            break;
        }
        case Value::Type::DESTINATION :
        {
            Destination * d = (Destination*)v;
            cout << d->value;
            break;
        }
        case Value::Type::TUPLE :
        {
            Tuple * t = (Tuple*)v;
            bool first = true;
            for (const auto & e : t->value)
            {
                if (!first) cout << " ";
                printValue(e);
                first = false;
            }
            break;
        }
        case Value::Type::GENERIC :
        {
            // todo
            break;
        }
        default:
            break;
    }
}

void printValueCallback(const Value * v)
{
    printValue(v);
    cout << "\n";
}

void constraintCallback(const TimeValue& position, const TimeValue& date, shared_ptr<State> state)
{
    cout << "Constraint : " << double(position) << ", " << double(date) << "\n";
    state->launch();
}

void eventCallback(TimeEvent::Status newStatus)
{
    cout << "Event : " << "new status received" << "\n";
}
