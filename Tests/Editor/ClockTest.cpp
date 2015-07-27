#include <QtTest>
#include "../ForwardDeclaration.h"
#include <functional>
#include <thread>
#include <iostream>

using namespace OSSIA;
using namespace std::chrono;
using namespace std::placeholders;

class ClockTest : public QObject
{
    Q_OBJECT

    std::vector<TimeValue> m_clock_positions;
    std::vector<TimeValue> m_clock_dates;
    int m_dropped_ticks;

    steady_clock::time_point m_clock_start_date;
    steady_clock::time_point m_clock_end_date;

    TimeValue m_last_frame_duration;

    bool display_frames = false;

    void clock_callback_light(const TimeValue& position, const TimeValue& date, unsigned char droppedTicks)
    {
        steady_clock::time_point frame_start_date = steady_clock::now();

        m_clock_positions.push_back(position);
        m_clock_dates.push_back(date);
        m_dropped_ticks += droppedTicks;

        if (display_frames)
            std::cout << "= " << (double)date << "(" << m_clock_positions.size() << " + " << m_dropped_ticks << " = " << m_clock_positions.size() + m_dropped_ticks << ")" << std::endl;

        if (position >= One)
            m_last_frame_duration = duration_cast<microseconds>(steady_clock::now() - frame_start_date).count();
    }

    void clock_callback_heavy(const TimeValue& position, const TimeValue& date, unsigned char droppedTicks)
    {
        steady_clock::time_point frame_start_date = steady_clock::now();

        m_clock_positions.push_back(position);
        m_clock_dates.push_back(date);
        m_dropped_ticks += droppedTicks;

        if (display_frames)
            std::cout << m_clock_positions.size() << " : " << (double)date << ", " << m_dropped_ticks << ", " << m_clock_positions.size() + m_dropped_ticks << std::endl;

        // an heavy processing ...
        long r;
        for (int i = 0; i < 1000; i++)
            for (int j = 0; j < 1000; j++)
                r++;

        if (position >= One)
            m_last_frame_duration = duration_cast<microseconds>(steady_clock::now() - frame_start_date).count();
    }

    void make_clock_test(const TimeValue& duration,
                         const TimeValue& granularity,
                         const TimeValue& offset,
                         float speed,
                         Clock::ExecutionCallback callback,
                         bool display = false)
    {
        display_frames = display;

        std::cout << "make_clock_test(" << duration << ", " << granularity << ", " << offset << ", " << speed << ") : ";

        if (display_frames)
            std::cout << std::endl;

        // setup clock
        auto clock = Clock::create(callback, duration, granularity, offset, speed);

        // clear frame vectors
        m_clock_positions.clear();
        m_clock_dates.clear();
        m_dropped_ticks = 0;

        // launch the clock and check running status : it have to be true after the launch
        QVERIFY(clock->getRunning() == false);
        m_clock_start_date = steady_clock::now();
        clock->start();
        QVERIFY(clock->getRunning() == true);

        // wait the clock end
        while (clock->getRunning())
            ;

        // how long the clock ran ?
        // it shouldn't be longer than duration and a certain amount of time that depends of offset and grain duration
        m_clock_end_date = steady_clock::now();
        long long effective_duration = duration_cast<microseconds>(m_clock_end_date - m_clock_start_date).count() - m_last_frame_duration;
        double grain_duration = granularity * speed;
        double duration_in_grain = ceil(duration / grain_duration);
        double offset_in_grain = floor(offset / grain_duration);
        long long expected_duration = (((duration_in_grain - offset_in_grain) * grain_duration) / speed) * 1000;

        // how many frames ?
        int effective_nbFrame = m_clock_positions.size();
        int expected_nbFrame = duration_in_grain - offset_in_grain - m_dropped_ticks;

        // display test summary before verifications
        if (effective_nbFrame == expected_nbFrame)
            std::cout << expected_duration / 1000. << " + " << m_last_frame_duration / 1000. << " + " << (effective_duration - expected_duration) / 1000. << " ms (in " << effective_nbFrame << " frames)" << std::endl;
        else if (effective_nbFrame > expected_nbFrame)
            std::cout << expected_duration / 1000. << " + " << m_last_frame_duration / 1000. << " + " << (effective_duration - expected_duration) / 1000. << " ms (+" << effective_nbFrame - expected_nbFrame << " frames)" << std::endl;
        else if (effective_nbFrame < expected_nbFrame)
            std::cout << expected_duration / 1000. << " + " << m_last_frame_duration / 1000. << " + " << (effective_duration - expected_duration) / 1000. << " ms (" << effective_nbFrame - expected_nbFrame << " frames)" << std::endl;

        // check duration
        QVERIFY(effective_duration >= expected_duration);
        QVERIFY(effective_duration <= expected_duration + (granularity * speed * 1000));

        // ckeck frame info
        QVERIFY(effective_nbFrame == expected_nbFrame);
        QVERIFY(m_clock_positions[effective_nbFrame-1] >= One);
        QVERIFY(m_clock_dates[0] >= (floor(offset / (granularity*speed)) + 1) * (granularity*speed));
        QVERIFY(m_clock_dates[effective_nbFrame-1] >= duration);

        // check time info after execution : they have to be the same
        QVERIFY(clock->getDuration() == duration);
        QVERIFY(clock->getGranularity() == granularity);
        QVERIFY(clock->getOffset() == offset);
        QVERIFY(clock->getSpeed() == speed);

        if (display_frames)
            std::cout << std::endl;

        display_frames = false;
    }

private Q_SLOTS:
    
    /*! test life cycle and accessors functions */
    void test_basic()
    {
        auto callback = std::bind(&ClockTest::clock_callback_light, this, _1, _2, _3);
        auto clock = Clock::create(callback);

        QVERIFY(clock->getDuration() == Infinite);
        QVERIFY(clock->getGranularity() == 1.);
        QVERIFY(clock->getOffset() == 0.);
        QVERIFY(clock->getSpeed() == 1.);

        clock->setDuration(1000.);
        clock->setGranularity(50.);
        clock->setOffset(500.);
        clock->setSpeed(2.);

        QVERIFY(clock->getDuration() == 1000.);
        QVERIFY(clock->getGranularity() == 50.);
        QVERIFY(clock->getOffset() == 500.);
        QVERIFY(clock->getSpeed() == 2.);

        QVERIFY(clock->getRunning() == false);
        QVERIFY(clock->getPosition() == 0.5);
        QVERIFY(clock->getDate() == 500.);

        //! \todo test clone()
    }
    
    /*! test execution functions */
    //! \todo test stop()
    //! \todo test pause()
    //! \todo test resume()
    void test_execution_light()
    {
        auto callback = std::bind(&ClockTest::clock_callback_light, this, _1, _2, _3);

        // 30 ms time grain accuracy
        make_clock_test(100., 30., 0., 1., callback);
        make_clock_test(100., 30., 0., 2., callback);
        make_clock_test(100., 30., 0., 0.5, callback);

        make_clock_test(100., 30., 50., 1., callback);
        make_clock_test(100., 30., 50., 2., callback);
        make_clock_test(100., 30., 50., 0.5, callback);

        // 10 ms time grain accuracy
        make_clock_test(100., 10., 0., 1., callback);
        make_clock_test(100., 10., 0., 2., callback);
        make_clock_test(100., 10., 0., 0.5, callback);

        make_clock_test(100., 10., 50., 1., callback);
        make_clock_test(100., 10., 50., 2., callback);
        make_clock_test(100., 10., 50., 0.5, callback);

        // 1 ms time grain accuracy
        make_clock_test(100., 1., 0., 1., callback);
        make_clock_test(100., 1., 0., 2., callback);
        make_clock_test(100., 1., 0., 0.5, callback);

        make_clock_test(100., 1., 50., 1., callback);
        make_clock_test(100., 1., 50., 2., callback);
        make_clock_test(100., 1., 50., 0.5, callback);
    }

    void test_execution_heavy()
    {
        auto callback = std::bind(&ClockTest::clock_callback_heavy, this, _1, _2, _3);

        // 30 ms time grain accuracy
        make_clock_test(100., 30., 0., 1., callback);
        make_clock_test(100., 30., 0., 2., callback);
        make_clock_test(100., 30., 0., 0.5, callback);

        make_clock_test(100., 30., 50., 1., callback);
        make_clock_test(100., 30., 50., 2., callback);
        make_clock_test(100., 30., 50., 0.5, callback);

        // 10 ms time grain accuracy
        make_clock_test(100., 10., 0., 1., callback);
        make_clock_test(100., 10., 0., 2., callback);
        make_clock_test(100., 10., 0., 0.5, callback);

        make_clock_test(100., 10., 50., 1., callback);
        make_clock_test(100., 10., 50., 2., callback);
        make_clock_test(100., 10., 50., 0.5, callback);

        // 1 ms time grain accuracy
        make_clock_test(100., 1., 0., 1., callback);
        make_clock_test(100., 1., 0., 2., callback);
        make_clock_test(100., 1., 0., 0.5, callback);

        make_clock_test(100., 1., 50., 1., callback);
        make_clock_test(100., 1., 50., 2., callback);
        make_clock_test(100., 1., 50., 0.5, callback);
    }
};

QTEST_APPLESS_MAIN(ClockTest)

#include "ClockTest.moc"
