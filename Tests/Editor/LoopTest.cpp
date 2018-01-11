// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <QtTest>
#include <ossia/ossia.hpp>
#include <functional>
#include <iostream>
#include <ossia/dataflow/audio_parameter.hpp>
#include <ossia/dataflow/node_process.hpp>
#include <ossia/dataflow/graph/graph.hpp>

namespace ossia
{

struct test_loop
{
    ossia::net::generic_device d;
    ossia::graph g;
    loop parent{7_tv, time_interval::exec_callback{}, time_event::exec_callback{}, time_event::exec_callback{}};

    ossia::audio_parameter* aparam{};
    test_loop()
    {
      auto& root = d.get_root_node();
      auto foo = root.create_child("foo");
      aparam = new audio_parameter{*foo};
      auto param = std::unique_ptr<ossia::net::parameter_base>{aparam};
      foo->set_parameter(std::move(param));

      parent.node->outputs()[0]->address = foo->get_parameter();
      auto scenar = std::make_shared<scenario>();
      parent.get_time_interval().add_time_process(scenar);

      auto ts0 = scenar->get_start_time_sync(); auto e0 = ts0->emplace(ts0->get_time_events().begin(), {});
      auto ts1 = std::make_shared<ossia::time_sync>(); scenar->add_time_sync(ts1); auto e1 = ts1->emplace(ts1->get_time_events().begin(), {});
      auto ts2 = std::make_shared<ossia::time_sync>(); scenar->add_time_sync(ts2); auto e2 = ts2->emplace(ts2->get_time_events().begin(), {});
      auto i1 = time_interval::create({}, **e0, **e1, 4_tv, 4_tv, 4_tv); scenar->add_time_interval(i1);
      auto i2 = time_interval::create({}, **e1, **e2, 3_tv, 3_tv, 3_tv); scenar->add_time_interval(i2);

      g.add_node(parent.node);
      g.add_node(scenar->node);
      g.add_node(i1->node);
      g.add_node(i2->node);

      {
        auto child = std::make_shared<loop>(1_tv, time_interval::exec_callback{}, time_event::exec_callback{}, time_event::exec_callback{});

        auto snd = std::make_shared<ossia::sound_node>();
        snd->set_sound(std::vector<std::vector<double>>{ {1., 2., 3., 4.} });
        child->get_time_interval().add_time_process(std::make_shared<ossia::node_process>(snd));
        i1->add_time_process(child);

        g.add_node(snd);
        g.connect(ossia::make_edge(ossia::immediate_glutton_connection{}, snd->outputs()[0], parent.node->inputs()[0], snd, parent.node));
      }

      {
        auto child = std::make_shared<loop>(2_tv, time_interval::exec_callback{}, time_event::exec_callback{}, time_event::exec_callback{});

        auto snd = std::make_shared<ossia::sound_node>();
        snd->set_sound(std::vector<std::vector<double>>{ {5.,6.,7.,8.} });
        child->get_time_interval().add_time_process(std::make_shared<ossia::node_process>(snd));
        i2->add_time_process(child);

        g.add_node(snd);
        g.connect(ossia::make_edge(ossia::immediate_glutton_connection{}, snd->outputs()[0], parent.node->inputs()[0], snd, parent.node));
      }

    }
};

}
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

    static void interval_callback(double position, ossia::time_value date)
    {
    }

    static void event_callback(time_event::status newStatus)
    {
        std::cout << "Event : " << "new status received" << std::endl;
    }

private Q_SLOTS:


    /*! test life cycle and accessors functions */
    void test_basic()
    {
        //using namespace std::placeholders;
        loop l(25._tv, &interval_callback, event_callback, event_callback);

        //! \todo test clone()
    }

    /*! test execution functions */
    //! \todo test state()
    void test_execution()
    {
        auto start_node = std::make_shared<time_sync>();
        auto end_node = std::make_shared<time_sync>();

        auto start_event = *(start_node->emplace(start_node->get_time_events().begin(), event_callback));
        auto end_event = *(end_node->emplace(end_node->get_time_events().begin(), event_callback));

        auto interval = time_interval::create(&interval_callback, *start_event, *end_event, 500._tv, 500._tv, 500._tv);

        ossia::clock c{*interval};
        c.set_granularity(std::chrono::milliseconds(20));
        interval->add_time_process(
              std::make_unique<loop>(25._tv, &interval_callback, event_callback, event_callback));

        c.start_and_tick();

        while (c.running())
            ;
    }

    void test_inf()
    {
      bool b = false;
      try {
      loop l{0_tv, time_interval::exec_callback{}, time_event::exec_callback{},
             time_event::exec_callback{}};
      l.start();
      l.state(1_tv, 0, 0_tv, 1.);
      } catch(...) {
        b = true;
      }
      QVERIFY(b);
    }

    void test_loop_sound()
    {
      {
        loop l{4_tv, time_interval::exec_callback{}, time_event::exec_callback{},
               time_event::exec_callback{}};
        auto snd = std::make_shared<ossia::sound_node>();
        snd->set_sound(std::vector<std::vector<double>>{ {0.1, 0.2, 0.3, 0.4} });
        l.get_time_interval().add_time_process(std::make_shared<ossia::node_process>(snd));

        l.start();
        l.state(1_tv, 0, 0_tv, 1.);
        qDebug() << snd->requested_tokens.size();
        QCOMPARE((int)snd->requested_tokens.size(), (int)2);
        qDebug() << snd->requested_tokens[0];
        qDebug() << snd->requested_tokens[1];
        QVERIFY((snd->requested_tokens[0] == token_request{0_tv, 0., 0_tv, 1., false, false}));
        QVERIFY((snd->requested_tokens[1] == token_request{1_tv, 0.25, 0_tv, 1., false, false}));
        l.stop();
      }

      {
        loop l{4_tv, time_interval::exec_callback{}, time_event::exec_callback{},
               time_event::exec_callback{}};
        auto snd = std::make_shared<ossia::sound_node>();
        snd->set_sound(std::vector<std::vector<double>>{ {0.1, 0.2, 0.3, 0.4} });
        l.get_time_interval().add_time_process(std::make_shared<ossia::node_process>(snd));

        l.start();
        l.state(5_tv, 0, 0_tv, 1.);
        qDebug() << snd->requested_tokens;
        QCOMPARE((int)snd->requested_tokens.size(), (int)4);
        QVERIFY((snd->requested_tokens[0] == token_request{0_tv, 0, 0_tv, 1., false, false}));
        QVERIFY((snd->requested_tokens[1] == token_request{4_tv, 1, 0_tv, 1., false, false}));
        QVERIFY((snd->requested_tokens[2] == token_request{0_tv, 0, 4_tv, 1., false, false}));
        QVERIFY((snd->requested_tokens[3] == token_request{1_tv, 0.25, 4_tv, 1., false, false}));
        l.stop();
      }

      {
        loop l{4_tv, time_interval::exec_callback{}, time_event::exec_callback{},
               time_event::exec_callback{}};
        auto snd = std::make_shared<ossia::sound_node>();
        snd->set_sound(std::vector<std::vector<double>>{ {0.1, 0.2, 0.3, 0.4} });
        l.get_time_interval().add_time_process(std::make_shared<ossia::node_process>(snd));

        l.start();
        l.state(9_tv, 0, 0_tv, 1.);
        ossia::execution_state e;
        for(auto tk : snd->requested_tokens)
          ((ossia::graph_node*)snd.get())->run(tk, e);
        auto op = snd->outputs()[0]->data.target<audio_port>()->samples;
        audio_vector expected{audio_channel{0.1, 0.2, 0.3, 0.4, 0.1, 0.2, 0.3, 0.4, 0.1}};
        for(int i = 0; i < 9; i++)
        {
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

    void test_subloop()
    {
      loop parent{7_tv, time_interval::exec_callback{}, time_event::exec_callback{}, time_event::exec_callback{}};
      auto child = std::make_shared<loop>(3_tv, time_interval::exec_callback{}, time_event::exec_callback{}, time_event::exec_callback{});
      parent.get_time_interval().add_time_process(child);

      auto snd = std::make_shared<ossia::sound_node>();
      snd->set_sound(std::vector<std::vector<double>>{ {0.1, 0.2, 0.3, 0.4} });
      child->get_time_interval().add_time_process(std::make_shared<ossia::node_process>(snd));

      parent.start();
      parent.state(14_tv, 0., 0_tv, 1.);

      ossia::execution_state e;
      for(auto tk : snd->requested_tokens)
        ((ossia::graph_node*)snd.get())->run(tk, e);


      audio_vector expected{audio_channel{0.1, 0.2, 0.3, 0.1, 0.2, 0.3, 0.1, 0.1, 0.2, 0.3, 0.1, 0.2, 0.3, 0.1}};

      auto op = snd->outputs()[0]->data.target<audio_port>()->samples;

      for(int i = 0; i < 14; i++)
      {
        QCOMPARE(expected[0][i], op[0][i]);
      }
      QCOMPARE(op, expected);
    }


    void test_subloops_in_scenario()
    {
      test_loop l;
      l.parent.start();
      l.parent.state(14_tv, 0., 0_tv, 1.);


      audio_vector expected{ossia::audio_channel{1., 1., 1., 1., 5., 6., 5., 1., 1., 1., 1., 5., 6., 5.}};

      std::vector<float> res(64, 0.);
      l.aparam->audio.resize(1);
      l.aparam->audio[0] = gsl::span<float>{res.data(), res.size()};
      ossia::execution_state e;
      l.g.state(e);
      e.commit();

      QVERIFY(e.m_audioState.size() > 0);
      auto op = l.aparam->audio;

      QVERIFY(op.size() > 0);
      QVERIFY(op[0].size() >= 14);

      for(int i = 0; i < 14; i++)
      {
        QCOMPARE(expected[0][i], op[0][i]);
      }
      for(int i = 14; i < op[0].size(); i++)
      {
        QCOMPARE(0.f, op[0][i]);
      }
    }

    void test_subloops_in_scenario_1by1()
    {
      test_loop l;

      float audio_data[2][64] {{}};
      l.parent.start();
      ossia::execution_state e;
      e.valueDevices = {&l.d};
      for(int i = 1; i <= 14; i++) {

        float* chan = audio_data[0] + i - 1 ;
        l.aparam->audio = {{chan, 64 - i}};
        l.parent.state(time_value{i}, 0., time_value{0}, 1.);
        e.clear_local_state();
        l.g.state(e);
        e.commit();
      }


      audio_vector expected{ossia::audio_channel{1., 1., 1., 1., 5., 6., 5., 1., 1., 1., 1., 5., 6., 5.}};

      for(int i = 0; i < 14; i++)
      {
        QCOMPARE( audio_data[0][i], expected[0][i]);
      }
    }

};

QTEST_APPLESS_MAIN(LoopTest)

#include "LoopTest.moc"
