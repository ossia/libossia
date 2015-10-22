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
        QVERIFY(zero == 0.);
        QVERIFY(zero == Zero);
        QVERIFY(zero.isInfinite() == false);

        TimeValue one(1.);
        QVERIFY(one == 1.);
        QVERIFY(one == One);
        QVERIFY(one.isInfinite() == false);

        TimeValue infinite(INFINITY);
        QVERIFY(infinite == Infinite);
        QVERIFY(infinite.isInfinite() == true);
    }
    
    /*! test =, +, +=, -, -= operators */
    void test_math()
    {
        TimeValue zero;
        TimeValue one(1.);
        TimeValue infinite(INFINITY);
        
        TimeValue copy = one;
        QVERIFY(copy == 1.);
        
        copy = infinite;
        QVERIFY(copy == INFINITY);
        
        QVERIFY(zero + one == 1.);
        QVERIFY(one + infinite == INFINITY);
        
        QVERIFY(one - one == 0.);
        QVERIFY(infinite - one == INFINITY);
    }
    
    /*! test comparison operators */
    void test_comparison()
    {
        TimeValue five(5.);
        TimeValue ten(10.);

        QVERIFY(five == five);
        QVERIFY(!(five == 10.));
        QVERIFY(!(five == ten));
        QVERIFY(!(five == Infinite));
        QVERIFY(!(Infinite == 10.));
        QVERIFY(!(Infinite == ten));

        QVERIFY(five != ten);
        QVERIFY(five != 10.);
        QVERIFY(five != Infinite);
        QVERIFY(Infinite != ten);
        QVERIFY(Infinite != 10.);
        QVERIFY(!(five != five));

        QVERIFY(five < 10.);
        QVERIFY(five < ten);
        QVERIFY(five <= 10.);
        QVERIFY(five <= ten);
        QVERIFY(ten > 5.);
        QVERIFY(ten > five);
        QVERIFY(ten >= 5.);
        QVERIFY(ten >= five);

        QVERIFY(Infinite > five);
        QVERIFY(Infinite > 5.);
        QVERIFY(Infinite >= five);
        QVERIFY(Infinite >= 5.);
        QVERIFY(five < Infinite);
        QVERIFY(five <= Infinite);

        QVERIFY(!(Infinite > Infinite));
        QVERIFY(Infinite >= Infinite);
        QVERIFY(!(Infinite < Infinite));
        QVERIFY(Infinite <= Infinite);

        QVERIFY(!(0. == Infinite));
        QVERIFY(0. != Infinite);
        QVERIFY(5. < Infinite);
        QVERIFY(5. <= Infinite);
    }
};

QTEST_APPLESS_MAIN(TimeValueTest)

#include "TimeValueTest.moc"
