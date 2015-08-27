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
#include "Network/Protocol/Local.h"
#include "Network/Protocol/Minuit.h"

using namespace OSSIA;
using namespace std;

void printValueCallback(const Value * v);
void printValue(const Value * v);

int main()
{
    // declare this program "B" as Local device
    auto localProtocol = Local::create();
    auto localDevice = Device::create(localProtocol, "B");

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
    
    auto localTestNode = *(localDevice->emplace(localDevice->children().cend(), "test"));
    
    auto localImpulseNode = *(localTestNode->emplace(localTestNode->children().cend(), "my_impulse"));
    auto localImpulseAddress = localImpulseNode->createAddress(Value::Type::IMPULSE);
    localImpulseAddress->addCallback(printValueCallback);
    
    auto localBoolNode = *(localTestNode->emplace(localTestNode->children().cend(), "my_bool"));
    auto localBoolAddress = localBoolNode->createAddress(Value::Type::BOOL);
    localBoolAddress->addCallback(printValueCallback);
    
    auto localIntNode = *(localTestNode->emplace(localTestNode->children().cend(), "my_int"));
    auto localIntAddress = localIntNode->createAddress(Value::Type::INT);
    localIntAddress->addCallback(printValueCallback);
    
    auto localFloatNode = *(localTestNode->emplace(localTestNode->children().cend(), "my_float"));
    auto localFloatAddress = localFloatNode->createAddress(Value::Type::FLOAT);
    localFloatAddress->addCallback(printValueCallback);
    
    auto localStringNode = *(localTestNode->emplace(localTestNode->children().cend(), "my_string"));
    auto localStringAddress = localStringNode->createAddress(Value::Type::STRING);
    localStringAddress->addCallback(printValueCallback);
    
    auto localDestinationNode = *(localTestNode->emplace(localTestNode->children().cend(), "my_destination"));
    auto localDestinationAddress = localDestinationNode->createAddress(Value::Type::DESTINATION);
    localDestinationAddress->addCallback(printValueCallback);
    
    auto localTupleNode = *(localTestNode->emplace(localTestNode->children().cend(), "my_tuple"));
    auto localTupleAddress = localTupleNode->createAddress(Value::Type::TUPLE);
    localTupleAddress->addCallback(printValueCallback);
    
    // update tree value
    Impulse n;
    localImpulseAddress->pushValue(&n);
    
    Bool b(true);
    localBoolAddress->pushValue(&b);
    
    Int i(5);
    localIntAddress->pushValue(&i);
    
    Float f(0.5);
    localFloatAddress->pushValue(&f);
    
    String s("hello world !");
    localStringAddress->pushValue(&s);
    
    Destination d(localFloatNode);
    localDestinationAddress->pushValue(&d);
    
    Tuple t(new Float(0.1), new Float(0.2), new Float(0.3));
    localTupleAddress->pushValue(&t);
    
    // declare a distant program "A" as a Minuit device
    auto minuitProtocol = Minuit::create("127.0.0.1", 9999, 6666);
    auto minuitDevice = Device::create(minuitProtocol, "A");

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
