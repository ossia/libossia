/*!
 * \file publication.cpp
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

#include "Network/Address.h"
#include "Network/Device.h"
#include "Network/Protocol.h"

using namespace OSSIA;
using namespace std;

void printValueCallback(const Value * v);
void printValue(const Value * v);

int main()
{
    // declare this program "B" as Local device
    Local localDeviceParameters{};
    auto localDevice = Device::create(localDeviceParameters, "B");

    /* publish each feature of program "B" as address into a tree
     /test
     /test/my_bang
     /test/my_bool
     /test/my_int
     /test/my_float
     /test/my_string
     /test/my_destination
     /test/my_tuple
     */
    
    // attach each address to a callback
    OSSIA::Address::ValueCallback callback = printValueCallback;
    
    auto localTestNode = *(localDevice->emplace(localDevice->children().cend(), "test"));
    
    auto localImpulseNode = *(localTestNode->emplace(localTestNode->children().cend(), "my_impulse"));
    auto localImpulseAddress = localImpulseNode->createAddress(Value::Type::IMPULSE);
    localImpulseAddress->setValueCallback(callback);
    
    auto localBoolNode = *(localTestNode->emplace(localTestNode->children().cend(), "my_bool"));
    auto localBoolAddress = localBoolNode->createAddress(Value::Type::BOOL);
    localBoolAddress->setValueCallback(callback);
    
    auto localIntNode = *(localTestNode->emplace(localTestNode->children().cend(), "my_int"));
    auto localIntAddress = localIntNode->createAddress(Value::Type::INT);
    localIntAddress->setValueCallback(callback);
    
    auto localFloatNode = *(localTestNode->emplace(localTestNode->children().cend(), "my_float"));
    auto localFloatAddress = localFloatNode->createAddress(Value::Type::FLOAT);
    localFloatAddress->setValueCallback(callback);
    
    auto localStringNode = *(localTestNode->emplace(localTestNode->children().cend(), "my_string"));
    auto localStringAddress = localStringNode->createAddress(Value::Type::STRING);
    localStringAddress->setValueCallback(callback);
    
    auto localDestinationNode = *(localTestNode->emplace(localTestNode->children().cend(), "my_destination"));
    auto localDestinationAddress = localDestinationNode->createAddress(Value::Type::DESTINATION);
    localDestinationAddress->setValueCallback(callback);
    
    auto localTupleNode = *(localTestNode->emplace(localTestNode->children().cend(), "my_tuple"));
    auto localTupleAddress = localTupleNode->createAddress(Value::Type::TUPLE);
    localTupleAddress->setValueCallback(callback);
    
    // update tree value
    Impulse n;
    localImpulseAddress->sendValue(&n);
    
    Bool b(true);
    localBoolAddress->sendValue(&b);
    
    Int i(5);
    localIntAddress->sendValue(&i);
    
    Float f(0.5);
    localFloatAddress->sendValue(&f);
    
    String s("hello world !");
    localStringAddress->sendValue(&s);
    
    Destination d(localFloatNode);
    localDestinationAddress->sendValue(&d);
    
    Tuple t(new Float(0.1), new Float(0.2), new Float(0.3));
    localTupleAddress->sendValue(&t);
    
    // declare a distant program "A" as a Minuit device
    Minuit minuitDeviceParameters{"127.0.0.1", 9999, 6666};
    auto minuitDevice = Device::create(minuitDeviceParameters, "A");

    while (true)
        ;
}

void printValueCallback(const Value * v)
{
    printValue(v);
    cout << "\n";
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
