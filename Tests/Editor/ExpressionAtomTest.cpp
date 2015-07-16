#include <QtTest>
#include "../ForwardDeclaration.h"
#include <iostream>

using namespace OSSIA;

class ExpressionAtomTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    
    /*! evaluate expressions with impulse values */
    void test_impulse()
    {
        // evaluate expressions with Impulse
        auto testImpulseExprA = ExpressionAtom::create(new Impulse(),
                                                       ExpressionAtom::Operator::EQUAL,
                                                       new Impulse());

        QVERIFY(testImpulseExprA->evaluate() == true);

        auto testImpulseExprB = ExpressionAtom::create(new Impulse(),
                                                       ExpressionAtom::Operator::DIFFERENT,
                                                       new Impulse());

        QVERIFY(testImpulseExprB->evaluate() == false);

        auto testImpulseExprC = ExpressionAtom::create(new Impulse(),
                                                       ExpressionAtom::Operator::EQUAL,
                                                       new Bool(true));

        QVERIFY(testImpulseExprC->evaluate() == true);

        auto testImpulseExprD = ExpressionAtom::create(new Impulse(),
                                                       ExpressionAtom::Operator::EQUAL,
                                                       new Int());

        QVERIFY(testImpulseExprD->evaluate() == true);

        auto testImpulseExprE = ExpressionAtom::create(new Impulse(),
                                                       ExpressionAtom::Operator::EQUAL,
                                                       new Float());

        QVERIFY(testImpulseExprE->evaluate() == true);

        auto testImpulseExprF = ExpressionAtom::create(new Impulse(),
                                                       ExpressionAtom::Operator::EQUAL,
                                                       new Char());

        QVERIFY(testImpulseExprF->evaluate() == true);

        auto testImpulseExprG = ExpressionAtom::create(new Impulse(),
                                                       ExpressionAtom::Operator::EQUAL,
                                                       new String());

        QVERIFY(testImpulseExprG->evaluate() == true);

        auto testImpulseExprH = ExpressionAtom::create(new Impulse(),
                                                       ExpressionAtom::Operator::EQUAL,
                                                       new Tuple());

        QVERIFY(testImpulseExprH->evaluate() == true);

        //! \todo test clone()
    }

    /*! evaluate expressions with bool values */
    void test_bool()
    {
        auto testBoolExprA = ExpressionAtom::create(new Bool(true),
                                                    ExpressionAtom::Operator::EQUAL,
                                                    new Bool(true));

        QVERIFY(testBoolExprA->evaluate() == true);

        auto testBoolExprB = ExpressionAtom::create(new Bool(true),
                                                    ExpressionAtom::Operator::DIFFERENT,
                                                    new Bool(true));

        QVERIFY(testBoolExprB->evaluate() == false);

        auto testBoolExprC = ExpressionAtom::create(new Bool(true),
                                                    ExpressionAtom::Operator::EQUAL,
                                                    new Impulse());

        QVERIFY(testBoolExprC->evaluate() == true);

        //! \todo test clone()
    }

    /*! evaluate expressions with int values */
    void test_int()
    {
        auto testIntExprA = ExpressionAtom::create(new Int(10),
                                                   ExpressionAtom::Operator::GREATER_THAN,
                                                   new Int(5));

        QVERIFY(testIntExprA->evaluate() == true);

        auto testIntExprB = ExpressionAtom::create(new Int(10),
                                                   ExpressionAtom::Operator::LOWER_THAN,
                                                   new Int(5));

        QVERIFY(testIntExprB->evaluate() == false);

        auto testIntExprC = ExpressionAtom::create(new Int(10),
                                                    ExpressionAtom::Operator::EQUAL,
                                                    new Impulse());

        QVERIFY(testIntExprC->evaluate() == true);

        //! \todo test clone()
    }

    /*! evaluate expressions with float values */
    void test_float()
    {
        auto testFloatExprA = ExpressionAtom::create(new Float(10.),
                                                     ExpressionAtom::Operator::GREATER_THAN_OR_EQUAL,
                                                     new Float(10.));

        QVERIFY(testFloatExprA->evaluate() == true);

        auto testFloatExprB = ExpressionAtom::create(new Float(10.),
                                                     ExpressionAtom::Operator::LOWER_THAN_OR_EQUAL,
                                                     new Float(10.));

        QVERIFY(testFloatExprB->evaluate() == true);

        auto testFloatExprC = ExpressionAtom::create(new Float(10.),
                                                     ExpressionAtom::Operator::EQUAL,
                                                     new Impulse());

        QVERIFY(testFloatExprC->evaluate() == true);

        //! \todo test clone()
    }

    /*! evaluate expressions with string values */
    void test_string()
    {
        auto testStringExprA = ExpressionAtom::create(new String("abc"),
                                                      ExpressionAtom::Operator::GREATER_THAN_OR_EQUAL,
                                                      new String("bcd"));

        QVERIFY(testStringExprA->evaluate() == false);

        auto testStringExprB = ExpressionAtom::create(new String("abc"),
                                                      ExpressionAtom::Operator::LOWER_THAN_OR_EQUAL,
                                                      new String("bcd"));

        QVERIFY(testStringExprB->evaluate() == true);

        auto testStringExprC = ExpressionAtom::create(new String("abc"),
                                                      ExpressionAtom::Operator::EQUAL,
                                                      new Impulse());

        QVERIFY(testStringExprC->evaluate() == true);

        //! \todo test clone()
    }

    /*! evaluate expressions with tuple values */
    void test_tuple()
    {
        std::vector<const Value*> value1 = {new Float(0.1), new Float(0.2), new Float(0.3)};
        std::vector<const Value*> value2 = {new Float(0.2), new Float(0.3), new Float(0.4)};

        auto testTupleExprA = ExpressionAtom::create(new Tuple(value1),
                                                     ExpressionAtom::Operator::GREATER_THAN,
                                                     new Tuple(value2));

        QVERIFY(testTupleExprA->evaluate() == false);

        auto testTupleExprB = ExpressionAtom::create(new Tuple(value1),
                                                     ExpressionAtom::Operator::LOWER_THAN,
                                                     new Tuple(value2));

        QVERIFY(testTupleExprB->evaluate() == true);

        auto testTupleExprC = ExpressionAtom::create(new Tuple(value1),
                                                     ExpressionAtom::Operator::EQUAL,
                                                     new Impulse());

        QVERIFY(testTupleExprC->evaluate() == true);

        //! \todo test clone()
    }

    /*! evaluate expressions with destination values */
    void test_destination()
    {
        // Local device
        Local device_parameters{};
        auto device = Device::create(device_parameters, "test");

        // Local tree building
        auto localImpulseNode1 = *(device->emplace(device->children().cend(), "my_impulse.1"));
        auto localImpulseAddress1 = localImpulseNode1->createAddress(Value::Type::IMPULSE);
        auto localImpulseNode2 = *(device->emplace(device->children().cend(), "my_impulse.2"));
        auto localImpulseAddress2 = localImpulseNode2->createAddress(Value::Type::IMPULSE);

        auto localBoolNode1 = *(device->emplace(device->children().cend(), "my_bool.1"));
        auto localBoolAddress1 = localBoolNode1->createAddress(Value::Type::BOOL);
        auto localBoolNode2 = *(device->emplace(device->children().cend(), "my_bool.2"));
        auto localBoolAddress2 = localBoolNode2->createAddress(Value::Type::BOOL);

        auto localIntNode1 = *(device->emplace(device->children().cend(), "my_int.1"));
        auto localIntAddress1 = localIntNode1->createAddress(Value::Type::INT);
        auto localIntNode2 = *(device->emplace(device->children().cend(), "my_int.2"));
        auto localIntAddress2 = localIntNode2->createAddress(Value::Type::INT);

        auto localFloatNode1 = *(device->emplace(device->children().cend(), "my_float.1"));
        auto localFloatAddress1 = localFloatNode1->createAddress(Value::Type::FLOAT);
        auto localFloatNode2 = *(device->emplace(device->children().cend(), "my_float.2"));
        auto localFloatAddress2 = localFloatNode2->createAddress(Value::Type::FLOAT);

        auto localStringNode1 = *(device->emplace(device->children().cend(), "my_string.1"));
        auto localStringAddress1 = localStringNode1->createAddress(Value::Type::STRING);
        auto localStringNode2 = *(device->emplace(device->children().cend(), "my_string.2"));
        auto localStringAddress2 = localStringNode2->createAddress(Value::Type::STRING);

        auto localTupleNode1 = *(device->emplace(device->children().cend(), "my_tuple.1"));
        auto localTupleAddress1 = localTupleNode1->createAddress(Value::Type::TUPLE);
        auto localTupleNode2 = *(device->emplace(device->children().cend(), "my_tuple.2"));
        auto localTupleAddress2 = localTupleNode2->createAddress(Value::Type::TUPLE);

        auto localDestinationNode1 = *(device->emplace(device->children().cend(), "my_destination.1"));
        auto localDestinationAddress1 = localDestinationNode1->createAddress(Value::Type::DESTINATION);
        auto localDestinationNode2 = *(device->emplace(device->children().cend(), "my_destination.2"));
        auto localDestinationAddress2 = localDestinationNode2->createAddress(Value::Type::DESTINATION);

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

        std::vector<const Value*> value1 = {new Float(0.1), new Float(0.2), new Float(0.3)};
        Tuple t1(value1);
        localTupleAddress1->sendValue(&t1);

        std::vector<const Value*> value2 = {new Float(0.2), new Float(0.3), new Float(0.4)};
        Tuple t2(value2);
        localTupleAddress2->sendValue(&t2);

        // evaluate expressions with Destination
        auto testDestinationExprA = ExpressionAtom::create(new Destination(localImpulseNode1),
                                                          ExpressionAtom::Operator::EQUAL,
                                                          new Destination(localImpulseNode2));

        QVERIFY(testDestinationExprA->evaluate() == true);

        auto testDestinationExprB = ExpressionAtom::create(new Destination(localBoolNode1),
                                                           ExpressionAtom::Operator::EQUAL,
                                                           new Destination(localBoolNode2));

        QVERIFY(testDestinationExprB->evaluate() == false);

        auto testDestinationExprC = ExpressionAtom::create(new Destination(localIntNode1),
                                                           ExpressionAtom::Operator::DIFFERENT,
                                                           new Destination(localIntNode2));

        QVERIFY(testDestinationExprC->evaluate() == true);

        auto testDestinationExprD = ExpressionAtom::create(new Destination(localFloatNode1),
                                                           ExpressionAtom::Operator::GREATER_THAN,
                                                           new Destination(localFloatNode2));

        QVERIFY(testDestinationExprD->evaluate() == true);

        auto testDestinationExprE = ExpressionAtom::create(new Destination(localStringNode1),
                                                           ExpressionAtom::Operator::LOWER_THAN,
                                                           new Destination(localStringNode2));

        QVERIFY(testDestinationExprE->evaluate() == true);

        auto testDestinationExprF = ExpressionAtom::create(new Destination(localDestinationNode1),
                                                           ExpressionAtom::Operator::GREATER_THAN_OR_EQUAL,
                                                           new Destination(localDestinationNode2));

        QVERIFY(testDestinationExprF->evaluate() == true);

        auto testDestinationExprG = ExpressionAtom::create(new Destination(localTupleNode1),
                                                           ExpressionAtom::Operator::LOWER_THAN_OR_EQUAL,
                                                           new Destination(localTupleNode2));

        QVERIFY(testDestinationExprG->evaluate() == true);

        //! \todo test clone()
    }
};

QTEST_APPLESS_MAIN(ExpressionAtomTest)

#include "ExpressionAtomTest.moc"
