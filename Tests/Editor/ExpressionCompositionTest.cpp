#include <QtTest>
#include <ossia/OSSIA.hpp>
#include <iostream>

using namespace ossia;
using namespace std::placeholders;

class ExpressionCompositionTest : public QObject
{
    Q_OBJECT

    bool m_result;
    bool m_result_callback_called;

    void result_callback(bool result)
    {
        m_result = result;
        m_result_callback_called = true;
    }

private Q_SLOTS:

    /*! test AND operator */
    void test_AND()
    {
        auto exprA = ExpressionAtom::create(new Bool(true),
                                            ExpressionAtom::Operator::EQUAL,
                                            new Bool(true));

        auto exprB = ExpressionAtom::create(new Bool(false),
                                            ExpressionAtom::Operator::EQUAL,
                                            new Bool(false));

        auto exprC = ExpressionAtom::create(new Bool(false),
                                            ExpressionAtom::Operator::DIFFERENT,
                                            new Bool(false));

        auto composition1 = ExpressionComposition::create(exprA,
                                                          ExpressionComposition::Operator::AND,
                                                          exprB);
        QVERIFY(composition1 != nullptr);
        QVERIFY(composition1->getType() == Expression::Type::COMPOSITION);
        QVERIFY(composition1->evaluate() == true);

        auto composition2 = ExpressionComposition::create(exprA,
                                                          ExpressionComposition::Operator::AND,
                                                          exprC);
        QVERIFY(composition2 != nullptr);
        QVERIFY(composition2->getType() == Expression::Type::COMPOSITION);
        QVERIFY(composition2->evaluate() == false);

        //! \todo test clone()
    }

    /*! test OR operator */
    void test_OR()
    {
        auto exprA = ExpressionAtom::create(new Bool(true),
                                            ExpressionAtom::Operator::EQUAL,
                                            new Bool(true));

        auto exprB = ExpressionAtom::create(new Bool(false),
                                            ExpressionAtom::Operator::EQUAL,
                                            new Bool(false));

        auto exprC = ExpressionAtom::create(new Bool(false),
                                            ExpressionAtom::Operator::DIFFERENT,
                                            new Bool(false));

        auto composition1 = ExpressionComposition::create(exprA,
                                                          ExpressionComposition::Operator::OR,
                                                          exprB);
        QVERIFY(composition1 != nullptr);
        QVERIFY(composition1->getType() == Expression::Type::COMPOSITION);
        QVERIFY(composition1->evaluate() == true);

        auto composition2 = ExpressionComposition::create(exprA,
                                                          ExpressionComposition::Operator::OR,
                                                          exprC);
        QVERIFY(composition2 != nullptr);
        QVERIFY(composition2->getType() == Expression::Type::COMPOSITION);
        QVERIFY(composition2->evaluate() == true);

        //! \todo test clone()
    }

    /*! test XOR operator */
    void test_XOR()
    {
        auto exprA = ExpressionAtom::create(new Bool(true),
                                            ExpressionAtom::Operator::EQUAL,
                                            new Bool(true));

        auto exprB = ExpressionAtom::create(new Bool(false),
                                            ExpressionAtom::Operator::EQUAL,
                                            new Bool(false));

        auto exprC = ExpressionAtom::create(new Bool(false),
                                            ExpressionAtom::Operator::DIFFERENT,
                                            new Bool(false));

        auto composition1 = ExpressionComposition::create(exprA,
                                                          ExpressionComposition::Operator::XOR,
                                                          exprB);
        QVERIFY(composition1 != nullptr);
        QVERIFY(composition1->getType() == Expression::Type::COMPOSITION);
        QVERIFY(composition1->evaluate() == false);

        auto composition2 = ExpressionComposition::create(exprA,
                                                          ExpressionComposition::Operator::XOR,
                                                          exprC);
        QVERIFY(composition2 != nullptr);
        QVERIFY(composition2->getType() == Expression::Type::COMPOSITION);
        QVERIFY(composition2->evaluate() == true);

        //! \todo test clone()
    }

    /*! test comparison operator */
    void test_comparison()
    {
        auto exprA = ExpressionAtom::create(new Bool(true),
                                            ExpressionAtom::Operator::EQUAL,
                                            new Bool(true));

        auto exprB = ExpressionAtom::create(new Bool(false),
                                            ExpressionAtom::Operator::EQUAL,
                                            new Bool(false));

        auto exprC = ExpressionAtom::create(new Bool(false),
                                            ExpressionAtom::Operator::DIFFERENT,
                                            new Bool(false));

        auto composition1 = ExpressionComposition::create(exprA,
                                                          ExpressionComposition::Operator::XOR,
                                                          exprB);

        auto composition2 = ExpressionComposition::create(exprA,
                                                          ExpressionComposition::Operator::XOR,
                                                          exprC);

        QVERIFY(expressions::expression_false != *composition1);
        QVERIFY(expressions::expression_true != *composition1);

        QVERIFY(*composition1 != *composition2);
        QVERIFY(!(*composition1 == *composition2));
    }

    /*! test callback managment */
    void test_callback()
    {
        // Local device
        auto local_protocol = Local::create();
        auto device = Device::create(local_protocol, "test");

        auto localIntNode1 = *(device->emplace(device->children().cend(), "my_int.1"));
        auto localIntAddress1 = localIntNode1->createAddress(Type::INT);
        auto localIntNode2 = *(device->emplace(device->children().cend(), "my_int.2"));
        auto localIntAddress2 = localIntNode2->createAddress(Type::INT);
        auto localIntNode3 = *(device->emplace(device->children().cend(), "my_int.3"));
        auto localIntAddress3 = localIntNode3->createAddress(Type::INT);

        auto testDestinationExprA = ExpressionAtom::create(new Destination(localIntNode1),
                                                           ExpressionAtom::Operator::LOWER_THAN,
                                                           new Destination(localIntNode2));

        auto testDestinationExprB = ExpressionAtom::create(new Destination(localIntNode2),
                                                           ExpressionAtom::Operator::LOWER_THAN,
                                                           new Destination(localIntNode3));

        auto testDestinationComposition = ExpressionComposition::create(testDestinationExprA,
                                                                        ExpressionComposition::Operator::AND,
                                                                        testDestinationExprB);

        ResultCallback callback = std::bind(&ExpressionCompositionTest::result_callback, this, _1);
        auto callback_index = testDestinationComposition->addCallback(callback);

        QVERIFY(testDestinationComposition->callbacks().size() == 1);

        m_result = false;
        m_result_callback_called = false;

        Int i1(5);
        localIntAddress1->pushValue(&i1);

        QVERIFY(m_result_callback_called == true && m_result == false);

        m_result = false;
        m_result_callback_called = false;

        Int i2(6);
        localIntAddress2->pushValue(&i2);

        QVERIFY(m_result_callback_called == true && m_result == false);

        m_result = false;
        m_result_callback_called = false;

        Int i3(7);
        localIntAddress3->pushValue(&i3);

        QVERIFY(m_result_callback_called == true && m_result == true);

        testDestinationComposition->removeCallback(callback_index);

        QVERIFY(testDestinationComposition->callbacks().size() == 0);

        m_result = false;
        m_result_callback_called = false;

        Int i4(10);
        localIntAddress2->pushValue(&i4);

        QVERIFY(m_result_callback_called == false && m_result == false);
    }
};

QTEST_APPLESS_MAIN(ExpressionCompositionTest)

#include "ExpressionCompositionTest.moc"
