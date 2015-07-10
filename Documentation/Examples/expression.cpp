/*!
 * \file expression.cpp
 *
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#include <iostream>
#include <memory>

#include "Editor/Expression.h"
#include "Editor/ExpressionAtom.h"
#include "Network/Address.h"
#include "Network/Device.h"
#include "Network/Protocol.h"

using namespace OSSIA;
using namespace std;

int main()
{
    // Local device
    Local localDeviceParameters{};
    auto localDevice = Device::create(localDeviceParameters, "i-score");

    // Local tree building
    auto localImpulseNode1 = *(localDevice->emplace(localDevice->children().cend(), "my_impulse.1"));
    auto localImpulseAddress1 = localImpulseNode1->createAddress(Value::Type::IMPULSE);
    auto localImpulseNode2 = *(localDevice->emplace(localDevice->children().cend(), "my_impulse.2"));
    auto localImpulseAddress2 = localImpulseNode2->createAddress(Value::Type::IMPULSE);
    
    auto localBoolNode1 = *(localDevice->emplace(localDevice->children().cend(), "my_bool.1"));
    auto localBoolAddress1 = localBoolNode1->createAddress(Value::Type::BOOL);
    auto localBoolNode2 = *(localDevice->emplace(localDevice->children().cend(), "my_bool.2"));
    auto localBoolAddress2 = localBoolNode2->createAddress(Value::Type::BOOL);
    
    auto localIntNode1 = *(localDevice->emplace(localDevice->children().cend(), "my_int.1"));
    auto localIntAddress1 = localIntNode1->createAddress(Value::Type::INT);
    auto localIntNode2 = *(localDevice->emplace(localDevice->children().cend(), "my_int.2"));
    auto localIntAddress2 = localIntNode2->createAddress(Value::Type::INT);
    
    auto localFloatNode1 = *(localDevice->emplace(localDevice->children().cend(), "my_float.1"));
    auto localFloatAddress1 = localFloatNode1->createAddress(Value::Type::FLOAT);
    auto localFloatNode2 = *(localDevice->emplace(localDevice->children().cend(), "my_float.2"));
    auto localFloatAddress2 = localFloatNode2->createAddress(Value::Type::FLOAT);
    
    auto localStringNode1 = *(localDevice->emplace(localDevice->children().cend(), "my_string.1"));
    auto localStringAddress1 = localStringNode1->createAddress(Value::Type::STRING);
    auto localStringNode2 = *(localDevice->emplace(localDevice->children().cend(), "my_string.2"));
    auto localStringAddress2 = localStringNode2->createAddress(Value::Type::STRING);
    
    auto localDestinationNode1 = *(localDevice->emplace(localDevice->children().cend(), "my_destination.1"));
    auto localDestinationAddress1 = localDestinationNode1->createAddress(Value::Type::DESTINATION);
    auto localDestinationNode2 = *(localDevice->emplace(localDevice->children().cend(), "my_destination.2"));
    auto localDestinationAddress2 = localDestinationNode2->createAddress(Value::Type::DESTINATION);
    
    auto localTupleNode1 = *(localDevice->emplace(localDevice->children().cend(), "my_tuple.1"));
    auto localTupleAddress1 = localTupleNode1->createAddress(Value::Type::TUPLE);
    auto localTupleNode2 = *(localDevice->emplace(localDevice->children().cend(), "my_tuple.2"));
    auto localTupleAddress2 = localTupleNode2->createAddress(Value::Type::TUPLE);
    
    // evaluate expression
    auto testImpulseExprA = ExpressionAtom::create(new Impulse(),
                                                   ExpressionAtom::Operator::EQUAL,
                                                   new Impulse());
    
    cout << "testImpulseExprA is " << testImpulseExprA->evaluate() << endl;
    
    auto testImpulseExprB = ExpressionAtom::create(new Impulse(),
                                                   ExpressionAtom::Operator::DIFFERENT,
                                                   new Impulse());
    
    cout << "testImpulseExprB is " << testImpulseExprB->evaluate() << endl;
    
    Bool b(true);
    localBoolAddress1->sendValue(&b);
    
    Int i(5);
    localIntAddress1->sendValue(&i);
    
    Float f(0.5);
    localFloatAddress1->sendValue(&f);
    
    String s("hello world !");
    localStringAddress1->sendValue(&s);
    
    Destination d(localFloatNode1);
    localDestinationAddress1->sendValue(&d);
    
    vector<const Value*> value = {new Float(0.1), new Float(0.2), new Float(0.3)};
    Tuple t(value);
    localTupleAddress1->sendValue(&t);
}