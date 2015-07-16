#include <QtTest>
#include "../ForwardDeclaration.h"
#include <iostream>

using namespace OSSIA;

class TimeValueTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    
    /*! test life cycle and accessors functions */
    void test_basic()
    {
        TimeValue zero;
        QVERIFY(zero == Zero);
        QVERIFY(zero.isInfinite() == false);

        TimeValue one(1.);
        QVERIFY(one == One);
        QVERIFY(one.isInfinite() == false);

        TimeValue infinite(true);
        QVERIFY(infinite == Infinite);
        QVERIFY(infinite.isInfinite() == true);
    }
    
    /*! test comparison operators */
    void test_comparison()
    {
        TimeValue five(5.);
        TimeValue ten(10.);

        QVERIFY(five == five);
        QVERIFY(!(five == ten));
        QVERIFY(!(five == Infinite));
        QVERIFY(!(Infinite == ten));

        QVERIFY(five != ten);
        QVERIFY(five != Infinite);
        QVERIFY(Infinite != ten);
        QVERIFY(!(five != five));

        QVERIFY(five < ten);
        QVERIFY(five <= ten);
        QVERIFY(ten > five);
        QVERIFY(ten >= five);

        QVERIFY(Infinite > five);
        QVERIFY(Infinite >= five);
        QVERIFY(five < Infinite);
        QVERIFY(five <= Infinite);

        QVERIFY(!(Infinite > Infinite));
        QVERIFY(Infinite >= Infinite);
        QVERIFY(!(Infinite < Infinite));
        QVERIFY(Infinite <= Infinite);
    }
};

QTEST_APPLESS_MAIN(TimeValueTest)

#include "TimeValueTest.moc"
