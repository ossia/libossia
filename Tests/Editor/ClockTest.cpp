#include <QtTest>
#include "../ForwardDeclaration.h"
#include <iostream>

using namespace OSSIA;

void clock_callback(const TimeValue&, const TimeValue&)
{
    ;
}

class ClockTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    
    /*! test life cycle and accessors functions */
    void test_basic()
    {
        auto clock = Clock::create();
        auto callback = Clock::ExecutionCallback(&clock_callback);

        QVERIFY(clock->getDuration() == 0.);
        QVERIFY(clock->getGranularity() == 1.);
        QVERIFY(clock->getOffset() == 0.);
        QVERIFY(clock->getSpeed() == 1.);
        QVERIFY(clock->getExternal() == false);
        QVERIFY(clock->getExecutionCallback() == nullptr);

        clock->setDuration(1000.);
        clock->setGranularity(50.);
        clock->setOffset(500.);
        clock->setSpeed(2.);
        clock->setExternal(true);
        clock->setExecutionCallback(callback);

        QVERIFY(clock->getDuration() == 1000.);
        QVERIFY(clock->getGranularity() == 50.);
        QVERIFY(clock->getOffset() == 500.);
        QVERIFY(clock->getSpeed() == 2.);
        QVERIFY(clock->getExternal() == true);
        QVERIFY(clock->getExecutionCallback() != nullptr);

        QVERIFY(clock->getRunning() == false);
        QVERIFY(clock->getPosition() == 0.5);
        QVERIFY(clock->getDate() == 500.);

        //! \todo test clone()
    }
    
    /*! test execution functions */
    void test_execution()
    {
        //! \todo test go()
        //! \todo test stop()
        //! \todo test pause()
        //! \todo test resume()
    }
};

QTEST_APPLESS_MAIN(ClockTest)

#include "ClockTest.moc"
