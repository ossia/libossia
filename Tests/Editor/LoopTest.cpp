// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <QtTest>
#include <ossia/ossia.hpp>
#include <functional>
#include <iostream>
#include <ossia/dataflow/audio_parameter.hpp>
#include <ossia/dataflow/node_process.hpp>

QDebug operator<<(QDebug d, ossia::token_request t)
{
  d << (int64_t)t.date << t.position << (int64_t)t.offset << t.start_discontinuous << t.end_discontinuous;
  return d;
}

QDebug operator<<(QDebug d, decltype(ossia::graph_node::requested_tokens) t)
{
  for(auto tk : t) d << tk << ", ";
  return d;
}
using namespace ossia;

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
        //using namespace std::placeholders;
        auto interval_callback = std::bind(&LoopTest::interval_callback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
        auto event_callback = std::bind(&LoopTest::event_callback, this, std::placeholders::_1);

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
        auto interval_callback = std::bind(&LoopTest::interval_callback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
        auto event_callback = std::bind(&LoopTest::event_callback, this, std::placeholders::_1);

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

    void test_loop_sound()
    {
      {
        loop l{4_tv, time_interval::exec_callback{}, time_event::exec_callback{},
               time_event::exec_callback{}};
        auto snd = std::make_shared<ossia::sound_node>();
        snd->set_sound(std::vector<std::vector<float>>{ {0.1, 0.2, 0.3, 0.4} });
        l.get_time_interval()->add_time_process(std::make_shared<ossia::node_process>(snd));

        ossia::state s;
        l.start(s);
        l.state(1_tv, 0, 0_tv);
        qDebug() << snd->requested_tokens[0];
        QVERIFY((snd->requested_tokens[0] == token_request{1_tv, 0.25, 0_tv, false, false}));
        l.stop();
      }

      {
        loop l{4_tv, time_interval::exec_callback{}, time_event::exec_callback{},
               time_event::exec_callback{}};
        auto snd = std::make_shared<ossia::sound_node>();
        snd->set_sound(std::vector<std::vector<float>>{ {0.1, 0.2, 0.3, 0.4} });
        l.get_time_interval()->add_time_process(std::make_shared<ossia::node_process>(snd));

        ossia::state s;
        l.start(s);
        l.state(5_tv, 0, 0_tv);
        qDebug() << snd->requested_tokens;
        QCOMPARE((int)snd->requested_tokens.size(), (int)3);
        QVERIFY((snd->requested_tokens[0] == token_request{4_tv, 1, 0_tv, false, false}));
        QVERIFY((snd->requested_tokens[1] == token_request{0_tv, 0, 0_tv, false, false}));
        QVERIFY((snd->requested_tokens[2] == token_request{1_tv, 0.25, 4_tv, false, false}));
        l.stop();
      }

      {
        loop l{4_tv, time_interval::exec_callback{}, time_event::exec_callback{},
               time_event::exec_callback{}};
        auto snd = std::make_shared<ossia::sound_node>();
        snd->set_sound(std::vector<std::vector<float>>{ {0.1, 0.2, 0.3, 0.4} });
        l.get_time_interval()->add_time_process(std::make_shared<ossia::node_process>(snd));

        ossia::state s;
        l.start(s);
        l.state(9_tv, 0, 0_tv);
        ossia::execution_state e;
        for(auto tk : snd->requested_tokens)
          ((ossia::graph_node*)snd.get())->run(tk, e);
        auto op = snd->outputs()[0]->data.target<audio_port>()->samples;
        for(auto v : op) for(auto val : v) qDebug() << val;
        audio_vector expected{audio_channel{0.1, 0.2, 0.3, 0.4, 0.1, 0.2, 0.3, 0.4, 0.1}};
        for(int i = 0; i < 9; i++)
        {
          qDebug() << expected[0][i] - op[0][i];
          QCOMPARE(expected[0][i], op[0][i]);
        }
        QCOMPARE(op, expected);
      }
      {
        /*
        l.state(2_tv, 0, 0_tv);
        qDebug() << snd->requested_tokens[1];
        QVERIFY((snd->requested_tokens[1] == token_request{1_tv, 0, 1_tv, false, false}));
        l.state(3_tv, 0, 0_tv);
        QVERIFY((snd->requested_tokens[2] == token_request{1_tv, 0, 2_tv, false, false}));
        l.state(4_tv, 0, 0_tv);
        QVERIFY((snd->requested_tokens[3] == token_request{1_tv, 0, 3_tv, false, false}));
        l.state(5_tv, 0, 0_tv);
        QVERIFY((snd->requested_tokens[4] == token_request{1_tv, 0, 4_tv, false, false}));
        l.state(6_tv, 0, 0_tv);
        QVERIFY((snd->requested_tokens[5] == token_request{1_tv, 0, 5_tv, false, false}));
        */
      }

    }
};

QTEST_APPLESS_MAIN(LoopTest)

#include "LoopTest.moc"
