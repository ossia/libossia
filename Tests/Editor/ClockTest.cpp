#include <QtTest>
#include "../ForwardDeclaration.h"
#include <functional>
#include <thread>
#include <iostream>

using namespace OSSIA;
using namespace std::placeholders;

class ClockTest : public QObject
{
    Q_OBJECT

    std::vector<TimeValue> m_clock_positions;
    std::vector<TimeValue> m_clock_dates;

    void clock_callback(const TimeValue& position, const TimeValue& date)
    {
        m_clock_positions.push_back(position);
        m_clock_dates.push_back(date);

        std::cout << (double)position << ", " << (double)date << std::endl;
    }

private Q_SLOTS:
    
    /*! test life cycle and accessors functions */
    void test_basic()
    {
        auto clock = Clock::create();
        auto callback = std::bind(&ClockTest::clock_callback, this, _1, _2);

        QVERIFY(clock->getDuration() == Infinite);
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
        auto clock = Clock::create(1000., 20.);

        auto callback = std::bind(&ClockTest::clock_callback, this, _1, _2);
        clock->setExecutionCallback(callback);

        m_clock_positions.clear();
        m_clock_dates.clear();

        QVERIFY(clock->getRunning() == false);
        clock->go();
        QVERIFY(clock->getRunning() == true);

        std::cout << "test_execution() : waiting 1 seconds for the clock end" << std::endl;
        while (clock->getRunning())
            ;

        QVERIFY(m_clock_positions.size() == 50);
        QVERIFY(m_clock_positions[49] == One);

        QVERIFY(m_clock_dates.size() == 50);
        QVERIFY(m_clock_dates[0] >= clock->getOffset() + clock->getGranularity());
        QVERIFY(m_clock_dates[49] >= clock->getDuration());

        clock->setOffset(500.);
        clock->setSpeed(2.);

        m_clock_positions.clear();
        m_clock_dates.clear();

        QVERIFY(clock->getRunning() == false);
        clock->go();
        QVERIFY(clock->getRunning() == true);

        std::cout << "test_execution() : waiting 0.5 seconds for the clock end" << std::endl;
        while (clock->getRunning())
            ;
        std::cout << "test_execution() : nb frame = " << m_clock_positions.size() << std::endl;

        QVERIFY(m_clock_positions.size() == 25);
        QVERIFY(m_clock_positions[24] == One);

        QVERIFY(m_clock_dates.size() == 25);
        QVERIFY(m_clock_dates[0] >= clock->getOffset() + clock->getGranularity());
        QVERIFY(m_clock_dates[24] >= clock->getDuration());

        //! \todo test stop()
        //! \todo test pause()
        //! \todo test resume()
    }
};

QTEST_APPLESS_MAIN(ClockTest)

#include "ClockTest.moc"
