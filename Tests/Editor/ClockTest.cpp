// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/detail/config.hpp>
#include <QtTest>
#include <ossia/ossia.hpp>
#include <functional>
#include <thread>
#include <iostream>

using namespace ossia;
using namespace std::chrono;
using namespace std::placeholders;


class ClockTest : public QObject
{
    Q_OBJECT

//    std::vector<time_value> m_clock_positions;
//    std::vector<time_value> m_clock_dates;
//    int m_dropped_ticks;

//    steady_clock::time_point m_clock_start_date;
//    steady_clock::time_point m_clock_end_date;

//    time_value m_last_frame_duration;

//    bool display_frames = false;

//    void clock_callback_light(ossia::time_value position, ossia::time_value date, unsigned char droppedTicks)
//    {
//        steady_clock::time_point frame_start_date = steady_clock::now();

//        m_clock_positions.push_back(position);
//        m_clock_dates.push_back(date);
//        m_dropped_ticks += droppedTicks;

//        if (display_frames)
//            std::cout << "= " << (double)date << " (" << m_clock_positions.size() << " + " << m_dropped_ticks << " = " << m_clock_positions.size() + m_dropped_ticks << ")" << std::endl;

//        if (position >= One)
//            m_last_frame_duration = duration_cast<microseconds>(steady_clock::now() - frame_start_date).count();
//    }

//    void clock_callback_heavy(ossia::time_value position, ossia::time_value date, unsigned char droppedTicks)
//    {
//        steady_clock::time_point frame_start_date = steady_clock::now();

//        m_clock_positions.push_back(position);
//        m_clock_dates.push_back(date);
//        m_dropped_ticks += droppedTicks;

//        if (display_frames)
//            std::cout << m_clock_positions.size() << " : " << (double)date << ", " << m_dropped_ticks << ", " << m_clock_positions.size() + m_dropped_ticks << std::endl;

//        // an heavy processing ...
//        long r = 0;
//        for (int i = 0; i < 1000; i++)
//            for (int j = 0; j < 1000; j++)
//                r++;

//        if (position >= One)
//            m_last_frame_duration = duration_cast<microseconds>(steady_clock::now() - frame_start_date).count();
//    }

//    void make_clock_test(double duration,
//                         double granularity,
//                         double offset,
//                         float speed,
//                         bool display = false)
//    {
//        make_clock_test(ossia::time_value(duration), ossia::time_value(granularity), ossia::time_value(offset), speed, display);
//    }

//    void make_clock_test(ossia::time_value duration,
//                         time_value granularity,
//                         time_value offset,
//                         float speed,
//                         bool display = false)
//    {
//        display_frames = display;

//        std::cout << "make_clock_test(" << duration << ", " << granularity << ", " << offset << ", " << speed << ") : ";

//        if (display_frames)
//            std::cout << std::endl;

//        // setup clock
//        time_interval t({});
//        ossia::clock c(t, duration, granularity, offset);

//        // clear frame vectors
//        m_clock_positions.clear();
//        m_clock_dates.clear();
//        m_dropped_ticks = 0;

//        // launch the clock and check running status : it have to be true after the launch
//        QVERIFY(c.running() == false);
//        m_clock_start_date = steady_clock::now();
//        c.start();
//        QVERIFY(c.running() == true);

//        // wait the clock end
//        while (c.running())
//            ;

//        // how long the clock ran ?
//        // it shouldn't be longer than duration and a certain amount of time that depends of offset and grain duration
//        m_clock_end_date = steady_clock::now();
//        long long effective_duration = duration_cast<microseconds>(m_clock_end_date - m_clock_start_date).count() - m_last_frame_duration;
//        double grain_duration = granularity * speed;
//        double duration_in_grain = ceil(duration / grain_duration);
//        double offset_in_grain = floor(offset / grain_duration);
//        long long expected_duration = (((duration_in_grain - offset_in_grain) * grain_duration) / speed) * 1000;

//        // how many frames ?
//        int effective_nbFrame = m_clock_positions.size();
//        int expected_nbFrame = 1 + duration_in_grain - offset_in_grain - m_dropped_ticks;

//        // display test summary before verifications
//        if (effective_nbFrame == expected_nbFrame)
//            std::cout << expected_duration / 1000. << " + " << m_last_frame_duration / 1000. << " + " << (effective_duration - expected_duration) / 1000. << " ms (in " << effective_nbFrame << " frames)" << std::endl;
//        else if (effective_nbFrame > expected_nbFrame)
//            std::cout << expected_duration / 1000. << " + " << m_last_frame_duration / 1000. << " + " << (effective_duration - expected_duration) / 1000. << " ms (+" << effective_nbFrame - expected_nbFrame << " frames)" << std::endl;
//        else if (effective_nbFrame < expected_nbFrame)
//            std::cout << expected_duration / 1000. << " + " << m_last_frame_duration / 1000. << " + " << (effective_duration - expected_duration) / 1000. << " ms (" << effective_nbFrame - expected_nbFrame << " frames)" << std::endl;

//        // check duration
//        QVERIFY(effective_duration >= expected_duration);
//        QVERIFY(effective_duration <= expected_duration + (granularity * speed * 1000));

//        // ckeck frame info
//        QCOMPARE(effective_nbFrame, expected_nbFrame);
//        QVERIFY(m_clock_positions[effective_nbFrame-1] >= One);
//        QVERIFY(m_clock_dates[0] == floor(offset / (granularity*speed)) * (granularity * speed));
//        QVERIFY(m_clock_dates[1] >= (floor(offset / (granularity*speed)) + 1) * (granularity*speed));
//        QVERIFY(m_clock_dates[effective_nbFrame-1] >= duration);

//        // check time info after execution : they have to be the same
//        QVERIFY(c.get_duration() == duration);
//        QVERIFY(c.get_granularity() == granularity);
//        QVERIFY(c.get_offset() == offset);

//        if (display_frames)
//            std::cout << std::endl;

//        display_frames = false;
//    }

private Q_SLOTS:

//    /*! test life cycle and accessors functions */
//    void test_basic()
//    {
//        using namespace ossia;
//        auto callback = std::bind(&ClockTest::clock_callback_light, this, _1, _2, _3);
//        ossia::clock c(callback);

//        QVERIFY(c.get_duration() == Infinite);
//        QVERIFY(c.get_granularity() == 10._tv);
//        QVERIFY(c.get_offset() == 0._tv);
//        QVERIFY(c.get_speed() == 1._tv);

//        c.set_duration(1000._tv);
//        c.set_granularity(50._tv);
//        c.set_offset(500._tv);
//        c.set_speed(2._tv);

//        QVERIFY(c.get_duration() == 1000._tv);
//        QVERIFY(c.get_granularity() == 50._tv);
//        QVERIFY(c.get_offset() == 500._tv);
//        QVERIFY(c.get_speed() == 2._tv);

//        QVERIFY(c.running() == false);
//        QVERIFY(c.get_position() == 0.5_tv);
//        QVERIFY(c.get_date() == 500._tv);

//        //! \todo test clone()
//    }

//    /*! test execution functions */
//    //! \todo test stop()
//    //! \todo test pause()
//    //! \todo test resume()
//    void test_execution_light()
//    {
//        auto callback = std::bind(&ClockTest::clock_callback_light, this, _1, _2, _3);

//        // 30 ms time grain accuracy
//        make_clock_test(100., 30., 0., 1., callback);
//        make_clock_test(100., 30., 0., 2., callback);
//        make_clock_test(100., 30., 0., 0.5, callback);

//        make_clock_test(100., 30., 50., 1., callback);
//        make_clock_test(100., 30., 50., 2., callback);
//        make_clock_test(100., 30., 50., 0.5, callback);

//        // 10 ms time grain accuracy
//        make_clock_test(100., 10., 0., 1., callback);
//        make_clock_test(100., 10., 0., 2., callback);
//        make_clock_test(100., 10., 0., 0.5, callback);

//        make_clock_test(100., 10., 50., 1., callback);
//        make_clock_test(100., 10., 50., 2., callback);
//        make_clock_test(100., 10., 50., 0.5, callback);

//        // 1 ms time grain accuracy
//        make_clock_test(100., 1., 0., 1., callback);
//        make_clock_test(100., 1., 0., 2., callback);
//        make_clock_test(100., 1., 0., 0.5, callback);

//        make_clock_test(100., 1., 50., 1., callback);
//        make_clock_test(100., 1., 50., 2., callback);
//        make_clock_test(100., 1., 50., 0.5, callback);
//    }

//    void test_execution_heavy()
//    {
//        auto callback = std::bind(&ClockTest::clock_callback_heavy, this, _1, _2, _3);

//        // 30 ms time grain accuracy
//        make_clock_test(100., 30., 0., 1., callback);
//        make_clock_test(100., 30., 0., 2., callback);
//        make_clock_test(100., 30., 0., 0.5, callback);

//        make_clock_test(100., 30., 50., 1., callback);
//        make_clock_test(100., 30., 50., 2., callback);
//        make_clock_test(100., 30., 50., 0.5, callback);

//        // 10 ms time grain accuracy
//        make_clock_test(100., 10., 0., 1., callback);
//        make_clock_test(100., 10., 0., 2., callback);
//        make_clock_test(100., 10., 0., 0.5, callback);

//        make_clock_test(100., 10., 50., 1., callback);
//        make_clock_test(100., 10., 50., 2., callback);
//        make_clock_test(100., 10., 50., 0.5, callback);

//        // 1 ms time grain accuracy
//        //! \todo make_clock_test(100., 1., 0., 1., callback);
//        //! \todo make_clock_test(100., 1., 0., 2., callback);
//        //! \todo make_clock_test(100., 1., 0., 0.5, callback);

//        //! \todo make_clock_test(100., 1., 50., 1., callback);
//        //! \todo make_clock_test(100., 1., 50., 2., callback);
//        //! \todo make_clock_test(100., 1., 50., 0.5, callback);
//    }

//    void test_transport()
//    {
//        //display_frames = true;

//        if (display_frames)
//            std::cout << std::endl;

//        // setup clock
//        auto callback = std::bind(&ClockTest::clock_callback_light, this, _1, _2, _3);
//        ossia::clock c(callback, 100._tv, 10._tv, 0._tv, 1.);

//        // clear frame vectors
//        m_clock_positions.clear();
//        m_clock_dates.clear();
//        m_dropped_ticks = 0;

//        // launch the clock
//        c.start();

//        // wait the clock to pass 50 ms
//        while (c.get_date() < 50._tv) std::cerr << c.get_date() << "\n";
//            ;

//        // then pause: the clock should be still running
//        c.pause();
//        qDebug() << "after pause" << m_clock_positions.size();
//        QVERIFY(c.running() == true);

//        // wait a little bit before to resume ...
//        if (display_frames)
//            std::cout << "clock paused for 20 ms" << std::endl;

//        std::this_thread::sleep_for( std::chrono::milliseconds(20));

//        // then resume
//        qDebug() << "before resume" << m_clock_positions.size();
//        c.resume();
//        QVERIFY(c.running() == true);

//        // wait the clock to pass 80 ms
//        while (c.get_date() < 80._tv) ;
//        // then stop
//        c.stop();
//        QVERIFY(c.running() == false);

//        qDebug() << "after stop" << m_clock_positions.size();
//        std::this_thread::sleep_for( std::chrono::milliseconds(20));
//        qDebug() << "after stop and wait" << m_clock_positions.size();
//        // check number of frames
//        QVERIFY((int)m_clock_positions.size() >= 9);
//        QVERIFY((int)m_clock_positions.size() <= 11);

//        // clear frame vectors
//        m_clock_positions.clear();
//        m_clock_dates.clear();
//        m_dropped_ticks = 0;

//        // launch the clock
//        c.start();

//        // wait the clock to end
//        while (c.running())
//            ;

//        // check number of frames
//        qDebug() << "after restart" << m_clock_positions.size();
//        QVERIFY(m_clock_positions.size() >= 11);

//        if (display_frames)
//            std::cout << std::endl;

//        display_frames = false;
//    }
};

QTEST_APPLESS_MAIN(ClockTest)

#include "ClockTest.moc"
