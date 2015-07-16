#include <QtTest>
#include "../ForwardDeclaration.h"
#include <iostream>

using namespace OSSIA;

class ExpressionCompositionTest : public QObject
{
    Q_OBJECT

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
        QVERIFY(composition1->evaluate() == true);

        auto composition2 = ExpressionComposition::create(exprA,
                                                          ExpressionComposition::Operator::AND,
                                                          exprC);
        QVERIFY(composition2 != nullptr);
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
        QVERIFY(composition1->evaluate() == true);

        auto composition2 = ExpressionComposition::create(exprA,
                                                          ExpressionComposition::Operator::OR,
                                                          exprC);
        QVERIFY(composition2 != nullptr);
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
        QVERIFY(composition1->evaluate() == false);

        auto composition2 = ExpressionComposition::create(exprA,
                                                          ExpressionComposition::Operator::XOR,
                                                          exprC);
        QVERIFY(composition2 != nullptr);
        QVERIFY(composition2->evaluate() == true);

        //! \todo test clone()
    }
};

QTEST_APPLESS_MAIN(ExpressionCompositionTest)

#include "ExpressionCompositionTest.moc"
