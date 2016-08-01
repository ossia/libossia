/*!
 * \file expression.cpp
 *
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#include <ossia/ossia.hpp>
#include <iostream>
#include <memory>
using namespace ossia;
using namespace ossia::expressions;
using namespace std;

int main()
{
    // Local device
    ossia::net::generic_device device{std::make_unique<ossia::net::local_protocol>(), "i-score"};

    // Local tree building
    auto localImpulseNode1 = device.createChild("my_impulse.1");
    localImpulseNode1->createAddress(val_type::IMPULSE);
    auto localImpulseNode2 = device.createChild("my_impulse.2");
    localImpulseNode2->createAddress(val_type::IMPULSE);

    auto localBoolNode1 = device.createChild("my_bool.1");
    auto localBoolAddress1 = localBoolNode1->createAddress(val_type::BOOL);
    auto localBoolNode2 = device.createChild("my_bool.2");
    auto localBoolAddress2 = localBoolNode2->createAddress(val_type::BOOL);

    auto localIntNode1 = device.createChild("my_int.1");
    auto localIntAddress1 = localIntNode1->createAddress(val_type::INT);
    auto localIntNode2 = device.createChild("my_int.2");
    auto localIntAddress2 = localIntNode2->createAddress(val_type::INT);

    auto localFloatNode1 = device.createChild("my_float.1");
    auto localFloatAddress1 = localFloatNode1->createAddress(val_type::FLOAT);
    auto localFloatNode2 = device.createChild("my_float.2");
    auto localFloatAddress2 = localFloatNode2->createAddress(val_type::FLOAT);

    auto localStringNode1 = device.createChild("my_string.1");
    auto localStringAddress1 = localStringNode1->createAddress(val_type::STRING);
    auto localStringNode2 = device.createChild("my_string.2");
    auto localStringAddress2 = localStringNode2->createAddress(val_type::STRING);

    auto localTupleNode1 = device.createChild("my_tuple.1");
    auto localTupleAddress1 = localTupleNode1->createAddress(val_type::TUPLE);
    auto localTupleNode2 = device.createChild("my_tuple.2");
    auto localTupleAddress2 = localTupleNode2->createAddress(val_type::TUPLE);

    auto localDestinationNode1 = device.createChild("my_destination.1");
    auto localDestinationAddress1 = localDestinationNode1->createAddress(val_type::DESTINATION);
    auto localDestinationNode2 = device.createChild("my_destination.2");
    auto localDestinationAddress2 = localDestinationNode2->createAddress(val_type::DESTINATION);

    // evaluate expression with Impulse
    auto testImpulseExprA = make_expression_atom(Impulse(),
                                                   expression_atom::Comparator::EQUAL,
                                                   Impulse());

    cout << boolalpha << "testImpulseExprA is " << evaluate(testImpulseExprA) << endl;

    auto testImpulseExprB = make_expression_atom(Impulse(),
                                                   expression_atom::Comparator::DIFFERENT,
                                                   Impulse());

    cout << boolalpha << "testImpulseExprB is " << evaluate(testImpulseExprB) << endl;

    // evaluate expression with Bool
    auto testBoolExprA = make_expression_atom(Bool(true),
                                                expression_atom::Comparator::EQUAL,
                                                Bool(true));

    cout << boolalpha << "testBoolExprA is " << evaluate(testBoolExprA) << endl;

    auto testBoolExprB = make_expression_atom(Bool(true),
                                                expression_atom::Comparator::DIFFERENT,
                                                Bool(true));

    cout << boolalpha << "testBoolExprB is " << evaluate(testBoolExprB) << endl;

    // evaluate expression with Int
    auto testIntExprA = make_expression_atom(Int(10),
                                               expression_atom::Comparator::GREATER_THAN,
                                               Int(5));

    cout << boolalpha << "testIntExprA is " << evaluate(testIntExprA) << endl;

    auto testIntExprB = make_expression_atom(Int(10),
                                               expression_atom::Comparator::LOWER_THAN,
                                               Int(5));

    cout << boolalpha << "testIntExprB is " << evaluate(testIntExprB) << endl;

    // evaluate expression with Float
    auto testFloatExprA = make_expression_atom(Float(10.),
                                                 expression_atom::Comparator::GREATER_THAN_OR_EQUAL,
                                                 Float(10.));

    cout << boolalpha << "testFloatExprA is " << evaluate(testFloatExprA) << endl;

    auto testFloatExprB = make_expression_atom(Float(10.),
                                                 expression_atom::Comparator::LOWER_THAN_OR_EQUAL,
                                                 Float(10.));

    cout << boolalpha << "testFloatExprB is " << evaluate(testFloatExprB) << endl;

    // evaluate expression with String
    auto testStringExprA = make_expression_atom(String("abc"),
                                                 expression_atom::Comparator::GREATER_THAN_OR_EQUAL,
                                                 String("bcd"));

    cout << boolalpha << "testStringExprA is " << evaluate(testStringExprA) << endl;

    auto testStringExprB = make_expression_atom(String("abc"),
                                                  expression_atom::Comparator::LOWER_THAN_OR_EQUAL,
                                                  String("bcd"));

    cout << boolalpha << "testStringExprB is " << evaluate(testStringExprB) << endl;

    // evaluate expression with Tuple
    Tuple value1 = {Float(0.1), Float(0.2), Float(0.3)};
    Tuple value2 = {Float(0.2), Float(0.3), Float(0.4)};

    auto testTupleExprA = make_expression_atom(value1,
                                                 expression_atom::Comparator::GREATER_THAN,
                                               value2);

    cout << boolalpha << "testTupleExprA is " << evaluate(testTupleExprA) << endl;

    auto testTupleExprB = make_expression_atom(value1,
                                                 expression_atom::Comparator::LOWER_THAN,
                                               value2);

    cout << boolalpha << "testTupleExprB is " << evaluate(testTupleExprB) << endl;

    // update node's value
    Bool b1(false);
    localBoolAddress1->setValue(b1);

    Bool b2(true);
    localBoolAddress2->setValue(b2);

    Int i1(5);
    localIntAddress1->setValue(i1);

    Int i2(10);
    localIntAddress2->setValue(i2);

    Float f1(0.5);
    localFloatAddress1->setValue(f1);

    Float f2(0.2);
    localFloatAddress2->setValue(f2);

    String s1("abc");
    localStringAddress1->setValue(s1);

    String s2("bcd");
    localStringAddress2->setValue(s2);

    Destination d1(*localFloatNode1);
    localDestinationAddress1->setValue(d1);

    Destination d2(*localFloatNode2);
    localDestinationAddress2->setValue(d2);

    Tuple t1(value1);
    localTupleAddress1->setValue(t1);

    Tuple t2(value2);
    localTupleAddress2->setValue(t2);

    // evaluate expression with Destination
    auto testDestinationExprA = make_expression_atom(Destination(*localBoolNode1),
                                                       expression_atom::Comparator::EQUAL,
                                                       Destination(*localBoolNode2));

    cout << boolalpha << "testDestinationExprA is " << evaluate(testDestinationExprA) << endl;

    auto testDestinationExprB = make_expression_atom(Destination(*localIntNode1),
                                                       expression_atom::Comparator::DIFFERENT,
                                                       Destination(*localIntNode2));

    cout << boolalpha << "testDestinationExprB is " << evaluate(testDestinationExprB) << endl;

    auto testDestinationExprC = make_expression_atom(Destination(*localFloatNode1),
                                                       expression_atom::Comparator::GREATER_THAN,
                                                       Destination(*localFloatNode2));

    cout << boolalpha << "testDestinationExprC is " << evaluate(testDestinationExprC) << endl;

    auto testDestinationExprD = make_expression_atom(Destination(*localStringNode1),
                                                       expression_atom::Comparator::LOWER_THAN,
                                                       Destination(*localStringNode2));

    cout << boolalpha << "testDestinationExprD is " << evaluate(testDestinationExprD) << endl;

    auto testDestinationExprE = make_expression_atom(Destination(*localDestinationNode1),
                                                       expression_atom::Comparator::GREATER_THAN_OR_EQUAL,
                                                       Destination(*localDestinationNode2));

    cout << boolalpha << "testDestinationExprE is " << evaluate(testDestinationExprE) << endl;

    auto testDestinationExprF = make_expression_atom(Destination(*localTupleNode1),
                                                       expression_atom::Comparator::LOWER_THAN_OR_EQUAL,
                                                       Destination(*localTupleNode2));

    cout << boolalpha << "testDestinationExprF is " << evaluate(testDestinationExprF) << endl;
}
