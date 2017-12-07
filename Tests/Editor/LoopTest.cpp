// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <QtTest>
#include <ossia/ossia.hpp>
#include <functional>
#include <iostream>

using namespace ossia;
using namespace std::placeholders;

class LoopTest : public QObject
{
    Q_OBJECT

    void interval_callback(double position, ossia::time_value date, const state_element& element)
    {
        ossia::launch(element);
    }

    void event_callback(time_event::status newStatus)
    {
        std::cout << "Event : " << "new status received" << std::endl;
    }

private Q_SLOTS:

    /*! test life cycle and accessors functions */
    void test_basic()
    {
        auto interval_callback = std::bind(&LoopTest::interval_callback, this, _1, _2, _3);
        auto event_callback = std::bind(&LoopTest::event_callback, this, _1);

        loop l(25._tv, interval_callback, event_callback, event_callback);

        QVERIFY(l.get_time_interval() != nullptr);
        QVERIFY(l.get_start_timesync() != nullptr);
        QVERIFY(l.get_end_timesync() != nullptr);

        //! \todo test clone()
    }

    /*! test execution functions */
    //! \todo test state()
    void test_execution()
    {
        auto interval_callback = std::bind(&LoopTest::interval_callback, this, _1, _2, _3);
        auto event_callback = std::bind(&LoopTest::event_callback, this, _1);

        auto start_node = std::make_shared<time_sync>();
        auto end_node = std::make_shared<time_sync>();

        auto start_event = *(start_node->emplace(start_node->get_time_events().begin(), event_callback));
        auto end_event = *(end_node->emplace(end_node->get_time_events().begin(), event_callback));

        auto interval = time_interval::create(interval_callback, *start_event, *end_event, 500._tv, 500._tv, 500._tv);

        ossia::clock c{*interval};
        c.set_granularity(std::chrono::milliseconds(20));
        interval->add_time_process(
              std::make_unique<loop>(25._tv, interval_callback, event_callback, event_callback));

        c.start();

        while (c.running())
            ;
    }
};

QTEST_APPLESS_MAIN(LoopTest)

#include "LoopTest.moc"
