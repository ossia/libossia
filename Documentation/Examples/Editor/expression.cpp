// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
    ossia::net::generic_device device{std::make_unique<ossia::net::multiplex_protocol>(), "i-score"};

    // Local tree building
    auto localImpulseNode1 = device.create_child("my_impulse.1");
    localImpulseNode1->create_parameter(val_type::IMPULSE);
    auto localImpulseNode2 = device.create_child("my_impulse.2");
    localImpulseNode2->create_parameter(val_type::IMPULSE);

    auto localBoolNode1 = device.create_child("my_bool.1");
    auto localBoolAddress1 = localBoolNode1->create_parameter(val_type::BOOL);
    auto localBoolNode2 = device.create_child("my_bool.2");
    auto localBoolAddress2 = localBoolNode2->create_parameter(val_type::BOOL);

    auto localIntNode1 = device.create_child("my_int.1");
    auto localIntAddress1 = localIntNode1->create_parameter(val_type::INT);
    auto localIntNode2 = device.create_child("my_int.2");
    auto localIntAddress2 = localIntNode2->create_parameter(val_type::INT);

    auto localFloatNode1 = device.create_child("my_float.1");
    auto localFloatAddress1 = localFloatNode1->create_parameter(val_type::FLOAT);
    auto localFloatNode2 = device.create_child("my_float.2");
    auto localFloatAddress2 = localFloatNode2->create_parameter(val_type::FLOAT);

    auto localStringNode1 = device.create_child("my_string.1");
    auto localStringAddress1 = localStringNode1->create_parameter(val_type::STRING);
    auto localStringNode2 = device.create_child("my_string.2");
    auto localStringAddress2 = localStringNode2->create_parameter(val_type::STRING);

    auto localTupleNode1 = device.create_child("my_tuple.1");
    auto localTupleAddress1 = localTupleNode1->create_parameter(val_type::TUPLE);
    auto localTupleNode2 = device.create_child("my_tuple.2");
    auto localTupleAddress2 = localTupleNode2->create_parameter(val_type::TUPLE);

    // evaluate expression with Impulse
    auto testImpulseExprA = make_expression_atom(impulse(),
                                                   comparator::EQUAL,
                                                   impulse());

    cout << boolalpha << "testImpulseExprA is " << evaluate(testImpulseExprA) << endl;

    auto testImpulseExprB = make_expression_atom(impulse(),
                                                   comparator::DIFFERENT,
                                                   impulse());

    cout << boolalpha << "testImpulseExprB is " << evaluate(testImpulseExprB) << endl;

    // evaluate expression with Bool
    auto testBoolExprA = make_expression_atom(true,
                                                comparator::EQUAL,
                                                true);

    cout << boolalpha << "testBoolExprA is " << evaluate(testBoolExprA) << endl;

    auto testBoolExprB = make_expression_atom(true,
                                                comparator::DIFFERENT,
                                                true);

    cout << boolalpha << "testBoolExprB is " << evaluate(testBoolExprB) << endl;

    // evaluate expression with Int
    auto testIntExprA = make_expression_atom(10,
                                               comparator::GREATER,
                                               5);

    cout << boolalpha << "testIntExprA is " << evaluate(testIntExprA) << endl;

    auto testIntExprB = make_expression_atom(10,
                                               comparator::LOWER,
                                               5);

    cout << boolalpha << "testIntExprB is " << evaluate(testIntExprB) << endl;

    // evaluate expression with Float
    auto testFloatExprA = make_expression_atom(10.,
                                                 comparator::GREATER_EQUAL,
                                                 10.);

    cout << boolalpha << "testFloatExprA is " << evaluate(testFloatExprA) << endl;

    auto testFloatExprB = make_expression_atom(10.,
                                                 comparator::LOWER_EQUAL,
                                                 10.);

    cout << boolalpha << "testFloatExprB is " << evaluate(testFloatExprB) << endl;

    // evaluate expression with String
    auto testStringExprA = make_expression_atom("abc"s,
                                                 comparator::GREATER_EQUAL,
                                                 "bcd"s);

    cout << boolalpha << "testStringExprA is " << evaluate(testStringExprA) << endl;

    auto testStringExprB = make_expression_atom("abc"s,
                                                  comparator::LOWER_EQUAL,
                                                  "bcd"s);

    cout << boolalpha << "testStringExprB is " << evaluate(testStringExprB) << endl;

    // evaluate expression with Tuple
    std::vector<ossia::value> value1{0.1, 0.2, 0.3};
    std::vector<ossia::value> value2{0.2, 0.3, 0.4};

    auto testTupleExprA = make_expression_atom(value1,
                                                 comparator::GREATER,
                                               value2);

    cout << boolalpha << "testTupleExprA is " << evaluate(testTupleExprA) << endl;

    auto testTupleExprB = make_expression_atom(value1,
                                                 comparator::LOWER,
                                               value2);

    cout << boolalpha << "testTupleExprB is " << evaluate(testTupleExprB) << endl;

    // update node's value
    bool b1(false);
    localBoolAddress1->set_value(b1);

    bool b2(true);
    localBoolAddress2->set_value(b2);

    int i1(5);
    localIntAddress1->set_value(i1);

    int i2(10);
    localIntAddress2->set_value(i2);

    float f1(0.5);
    localFloatAddress1->set_value(f1);

    float f2(0.2);
    localFloatAddress2->set_value(f2);

    std::string s1("abc");
    localStringAddress1->set_value(s1);

    std::string s2("bcd");
    localStringAddress2->set_value(s2);

    std::vector<ossia::value> t1(value1);
    localTupleAddress1->set_value(t1);

    std::vector<ossia::value> t2(value2);
    localTupleAddress2->set_value(t2);

    // evaluate expression with Destination
    auto testDestinationExprA = make_expression_atom(*localBoolAddress1,
                                                       comparator::EQUAL,
                                                       *localBoolAddress2);

    cout << boolalpha << "testDestinationExprA is " << evaluate(testDestinationExprA) << endl;

    auto testDestinationExprB = make_expression_atom(*localIntAddress1,
                                                       comparator::DIFFERENT,
                                                       *localIntAddress2);

    cout << boolalpha << "testDestinationExprB is " << evaluate(testDestinationExprB) << endl;

    auto testDestinationExprC = make_expression_atom(*localFloatAddress1,
                                                       comparator::GREATER,
                                                       *localFloatAddress2);

    cout << boolalpha << "testDestinationExprC is " << evaluate(testDestinationExprC) << endl;

    auto testDestinationExprD = make_expression_atom(*localStringAddress1,
                                                       comparator::LOWER,
                                                       *localStringAddress2);

    cout << boolalpha << "testDestinationExprD is " << evaluate(testDestinationExprD) << endl;

    auto testDestinationExprF = make_expression_atom(*localTupleAddress1,
                                                       comparator::LOWER_EQUAL,
                                                       *localTupleAddress2);

    cout << boolalpha << "testDestinationExprF is " << evaluate(testDestinationExprF) << endl;
}
