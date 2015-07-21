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
        QVERIFY(!(five == 10.));
        QVERIFY(!(five == ten));
        QVERIFY(!(5. == Infinite));
        QVERIFY(!(five == Infinite));
        QVERIFY(!(Infinite == 10.));
        QVERIFY(!(Infinite == ten));

        QVERIFY(five != ten);
        QVERIFY(five != 10.);
        QVERIFY(five != Infinite);
        QVERIFY(5. != Infinite);
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
        QVERIFY(5. < Infinite);
        QVERIFY(five <= Infinite);
        QVERIFY(5. <= Infinite);

        QVERIFY(!(Infinite > Infinite));
        QVERIFY(Infinite >= Infinite);
        QVERIFY(!(Infinite < Infinite));
        QVERIFY(Infinite <= Infinite);
    }
};

QTEST_APPLESS_MAIN(TimeValueTest)

#include "TimeValueTest.moc"
