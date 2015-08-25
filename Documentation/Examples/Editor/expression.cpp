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
#include "Network/Protocol/Local.h"

using namespace OSSIA;
using namespace std;

int main()
{
    // Local device
    auto localProtocol = Local::create();
    auto localDevice = Device::create(localProtocol, "i-score");

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
    
    auto localTupleNode1 = *(localDevice->emplace(localDevice->children().cend(), "my_tuple.1"));
    auto localTupleAddress1 = localTupleNode1->createAddress(Value::Type::TUPLE);
    auto localTupleNode2 = *(localDevice->emplace(localDevice->children().cend(), "my_tuple.2"));
    auto localTupleAddress2 = localTupleNode2->createAddress(Value::Type::TUPLE);
    
    auto localDestinationNode1 = *(localDevice->emplace(localDevice->children().cend(), "my_destination.1"));
    auto localDestinationAddress1 = localDestinationNode1->createAddress(Value::Type::DESTINATION);
    auto localDestinationNode2 = *(localDevice->emplace(localDevice->children().cend(), "my_destination.2"));
    auto localDestinationAddress2 = localDestinationNode2->createAddress(Value::Type::DESTINATION);
    
    // evaluate expression with Impulse
    auto testImpulseExprA = ExpressionAtom::create(new Impulse(),
                                                   ExpressionAtom::Operator::EQUAL,
                                                   new Impulse());
    
    cout << boolalpha << "testImpulseExprA is " << testImpulseExprA->evaluate() << endl;
    
    auto testImpulseExprB = ExpressionAtom::create(new Impulse(),
                                                   ExpressionAtom::Operator::DIFFERENT,
                                                   new Impulse());
    
    cout << boolalpha << "testImpulseExprB is " << testImpulseExprB->evaluate() << endl;
    
    // evaluate expression with Bool
    auto testBoolExprA = ExpressionAtom::create(new Bool(true),
                                                ExpressionAtom::Operator::EQUAL,
                                                new Bool(true));
    
    cout << boolalpha << "testBoolExprA is " << testBoolExprA->evaluate() << endl;
    
    auto testBoolExprB = ExpressionAtom::create(new Bool(true),
                                                ExpressionAtom::Operator::DIFFERENT,
                                                new Bool(true));
    
    cout << boolalpha << "testBoolExprB is " << testBoolExprB->evaluate() << endl;
    
    // evaluate expression with Int
    auto testIntExprA = ExpressionAtom::create(new Int(10),
                                               ExpressionAtom::Operator::GREATER_THAN,
                                               new Int(5));
    
    cout << boolalpha << "testIntExprA is " << testIntExprA->evaluate() << endl;
    
    auto testIntExprB = ExpressionAtom::create(new Int(10),
                                               ExpressionAtom::Operator::LOWER_THAN,
                                               new Int(5));
    
    cout << boolalpha << "testIntExprB is " << testIntExprB->evaluate() << endl;
    
    // evaluate expression with Float
    auto testFloatExprA = ExpressionAtom::create(new Float(10.),
                                                 ExpressionAtom::Operator::GREATER_THAN_OR_EQUAL,
                                                 new Float(10.));
    
    cout << boolalpha << "testFloatExprA is " << testFloatExprA->evaluate() << endl;
    
    auto testFloatExprB = ExpressionAtom::create(new Float(10.),
                                                 ExpressionAtom::Operator::LOWER_THAN_OR_EQUAL,
                                                 new Float(10.));
    
    cout << boolalpha << "testFloatExprB is " << testFloatExprB->evaluate() << endl;
    
    // evaluate expression with String
    auto testStringExprA = ExpressionAtom::create(new String("abc"),
                                                 ExpressionAtom::Operator::GREATER_THAN_OR_EQUAL,
                                                 new String("bcd"));
    
    cout << boolalpha << "testStringExprA is " << testStringExprA->evaluate() << endl;
    
    auto testStringExprB = ExpressionAtom::create(new String("abc"),
                                                  ExpressionAtom::Operator::LOWER_THAN_OR_EQUAL,
                                                  new String("bcd"));
    
    cout << boolalpha << "testStringExprB is " << testStringExprB->evaluate() << endl;
    
    // evaluate expression with Tuple
    vector<const Value*> value1 = {new Float(0.1), new Float(0.2), new Float(0.3)};
    vector<const Value*> value2 = {new Float(0.2), new Float(0.3), new Float(0.4)};
    
    auto testTupleExprA = ExpressionAtom::create(new Tuple(value1),
                                                 ExpressionAtom::Operator::GREATER_THAN,
                                                 new Tuple(value2));
    
    cout << boolalpha << "testTupleExprA is " << testTupleExprA->evaluate() << endl;
    
    auto testTupleExprB = ExpressionAtom::create(new Tuple(value1),
                                                 ExpressionAtom::Operator::LOWER_THAN,
                                                 new Tuple(value2));
    
    cout << boolalpha << "testTupleExprB is " << testTupleExprB->evaluate() << endl;
    
    // update node's value
    Bool b1(false);
    localBoolAddress1->sendValue(&b1);
    
    Bool b2(true);
    localBoolAddress2->sendValue(&b2);
    
    Int i1(5);
    localIntAddress1->sendValue(&i1);
    
    Int i2(10);
    localIntAddress2->sendValue(&i2);
    
    Float f1(0.5);
    localFloatAddress1->sendValue(&f1);
    
    Float f2(0.2);
    localFloatAddress2->sendValue(&f2);
    
    String s1("abc");
    localStringAddress1->sendValue(&s1);
    
    String s2("bcd");
    localStringAddress2->sendValue(&s2);
    
    Destination d1(localFloatNode1);
    localDestinationAddress1->sendValue(&d1);
    
    Destination d2(localFloatNode2);
    localDestinationAddress2->sendValue(&d2);
    
    Tuple t1(value1);
    localTupleAddress1->sendValue(&t1);
    
    Tuple t2(value2);
    localTupleAddress2->sendValue(&t2);
    
    // evaluate expression with Destination
    auto testDestinationExprA = ExpressionAtom::create(new Destination(localBoolNode1),
                                                       ExpressionAtom::Operator::EQUAL,
                                                       new Destination(localBoolNode2));
    
    cout << boolalpha << "testDestinationExprA is " << testDestinationExprA->evaluate() << endl;
    
    auto testDestinationExprB = ExpressionAtom::create(new Destination(localIntNode1),
                                                       ExpressionAtom::Operator::DIFFERENT,
                                                       new Destination(localIntNode2));
    
    cout << boolalpha << "testDestinationExprB is " << testDestinationExprB->evaluate() << endl;
    
    auto testDestinationExprC = ExpressionAtom::create(new Destination(localFloatNode1),
                                                       ExpressionAtom::Operator::GREATER_THAN,
                                                       new Destination(localFloatNode2));
    
    cout << boolalpha << "testDestinationExprC is " << testDestinationExprC->evaluate() << endl;
    
    auto testDestinationExprD = ExpressionAtom::create(new Destination(localStringNode1),
                                                       ExpressionAtom::Operator::LOWER_THAN,
                                                       new Destination(localStringNode2));
    
    cout << boolalpha << "testDestinationExprD is " << testDestinationExprD->evaluate() << endl;
    
    auto testDestinationExprE = ExpressionAtom::create(new Destination(localDestinationNode1),
                                                       ExpressionAtom::Operator::GREATER_THAN_OR_EQUAL,
                                                       new Destination(localDestinationNode2));
    
    cout << boolalpha << "testDestinationExprE is " << testDestinationExprE->evaluate() << endl;
    
    auto testDestinationExprF = ExpressionAtom::create(new Destination(localTupleNode1),
                                                       ExpressionAtom::Operator::LOWER_THAN_OR_EQUAL,
                                                       new Destination(localTupleNode2));
    
    cout << boolalpha << "testDestinationExprF is " << testDestinationExprF->evaluate() << endl;
}
