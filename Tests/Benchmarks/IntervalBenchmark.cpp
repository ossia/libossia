// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <QtTest>
#include <ossia/ossia.hpp>
#include <ossia/editor/scenario/time_value.hpp>
#include <iostream>
#include <thread>
#include <atomic>
#include "../Editor/TestUtils.hpp"
#if __has_include(<valgrind/callgrind.h>)
#include <valgrind/callgrind.h>
#define USE_CALLGRIND 1
#endif
using namespace ossia;
class IntervalBenchmark : public QObject
{
  Q_OBJECT

    std::vector<std::vector<double>> data{4};
    static const constexpr int benchmark_range[] = {0, 1, 2, 5, 10, 50, 100, 200, 300, 400, 500, 600,
                                                    700, 800, 900, 1000, 2000, 3000, 4000, 5000
                                                     , 6000, 7000, 8000, 9000, 10000
                                                    // , 100000

                                                    };
    void print_states(const ossia::scenario& s)
    {
      int i = 0;
      for(const auto& node : s.get_time_syncs())
      {
        std::cout << "Node " << i << "(" << node->is_evaluating() << ")";
        int j = 0;
        for(auto& ev : node->get_time_events())
        {
          std::cout << ":  Event: " << j << " => " << ev->get_status() << "\n";
          j++;
        }
        i++;
      }
    }

  void add_interval_parallel(
      ossia::scenario& s,
      ossia::time_value def = 100._tv,
      ossia::time_value min = 100._tv,
      ossia::time_value max = 100._tv)
  {
    using namespace ossia;
    auto sn = s.get_start_time_sync();
    auto se = *sn->get_time_events().begin();
    auto en = std::make_shared<ossia::time_sync>();
    s.add_time_sync(en);
    en->set_expression(ossia::expressions::make_expression_false());
    auto ee = std::make_shared<ossia::time_event>(ossia::time_event::exec_callback{}, *en, ossia::expressions::make_expression_true());
    en->insert(en->get_time_events().end(), ee);
    auto c = ossia::time_interval::create([] (auto&&...) {}, *se, *ee, def, min, max);
    s.add_time_interval(c);
  }

  ossia::time_event* add_interval_serial(
      ossia::scenario& s,
      ossia::time_event& se,
      ossia::time_value def = 100._tv,
      ossia::time_value min = 100._tv,
      ossia::time_value max = 100._tv)
  {
    using namespace ossia;
    auto en = std::make_shared<ossia::time_sync>();
    s.add_time_sync(en);
    en->set_expression(ossia::expressions::make_expression_false());
    auto ee = std::make_shared<ossia::time_event>(ossia::time_event::exec_callback{}, *en, ossia::expressions::make_expression_true());
    en->insert(en->get_time_events().end(), ee);
    auto c = ossia::time_interval::create([] (auto&&...) {}, se, *ee, def, min, max);
    s.add_time_interval(c);
    return ee.get();
  }

private Q_SLOTS:
  void test_states_rigid()
  {
      root_scenario root;
      add_interval_parallel(*root.scenario, 2_tv, 2_tv, 2_tv);

      std::cout << "\nBefore start:\n";
      print_states(*root.scenario);

      root.interval->start_and_tick();

      std::cout << "\nAfter start:\n";
      print_states(*root.scenario);

      root.interval->tick(1000_tv);
      std::cout << "\nAfter tick 1:\n";
      print_states(*root.scenario);

      root.interval->tick(1000_tv);
      std::cout << "\nAfter tick 2:\n";
      print_states(*root.scenario);

      root.interval->tick(1000_tv);
      std::cout << "\nAfter tick 3:\n";
      print_states(*root.scenario);

      root.interval->tick(1000_tv);
      std::cout << "\nAfter tick 4:\n";
      print_states(*root.scenario);
  }


  void test_states_flexible()
  {
      root_scenario root;
      add_interval_parallel(*root.scenario, 3_tv, 2_tv, 4_tv);

      std::cout << "\nBefore start:\n";
      print_states(*root.scenario);

      root.interval->start_and_tick();

      std::cout << "\nAfter start:\n";
      print_states(*root.scenario);

      root.interval->tick(1000_tv);
      std::cout << "\nAfter tick 1:\n";
      print_states(*root.scenario);

      root.interval->tick(1000_tv);
      std::cout << "\nAfter tick 2:\n";
      print_states(*root.scenario);

      root.interval->tick(1000_tv);
      std::cout << "\nAfter tick 3:\n";
      print_states(*root.scenario);

      root.interval->tick(1000_tv);
      std::cout << "\nAfter tick 4:\n";
      print_states(*root.scenario);

      root.interval->tick(1000_tv);
      std::cout << "\nAfter tick 5:\n";
      print_states(*root.scenario);
  }

  void test_states_flexible_no_min()
  {
      root_scenario root;
      add_interval_parallel(*root.scenario, 3_tv, 0_tv, 4_tv);

      std::cout << "\nBefore start:\n";
      print_states(*root.scenario);

      root.interval->start_and_tick();

      std::cout << "\nAfter start:\n";
      print_states(*root.scenario);

      root.interval->tick(1000_tv);
      std::cout << "\nAfter tick 1:\n";
      print_states(*root.scenario);

      root.interval->tick(1000_tv);
      std::cout << "\nAfter tick 2:\n";
      print_states(*root.scenario);

      root.interval->tick(1000_tv);
      std::cout << "\nAfter tick 3:\n";
      print_states(*root.scenario);

      root.interval->tick(1000_tv);
      std::cout << "\nAfter tick 4:\n";
      print_states(*root.scenario);

      root.interval->tick(1000_tv);
      std::cout << "\nAfter tick 5:\n";
      print_states(*root.scenario);
  }

  void test_states_flexible_no_max()
  {
      root_scenario root;
      add_interval_parallel(*root.scenario, 3_tv, 2_tv, ossia::Infinite);

      std::cout << "\nBefore start:\n";
      print_states(*root.scenario);

      root.interval->start_and_tick();

      std::cout << "\nAfter start:\n";
      print_states(*root.scenario);

      root.interval->tick(1000_tv);
      std::cout << "\nAfter tick 1:\n";
      print_states(*root.scenario);

      root.interval->tick(1000_tv);
      std::cout << "\nAfter tick 2:\n";
      print_states(*root.scenario);

      root.interval->tick(1000_tv);
      std::cout << "\nAfter tick 3:\n";
      print_states(*root.scenario);

      root.interval->tick(1000_tv);
      std::cout << "\nAfter tick 4:\n";
      print_states(*root.scenario);

      root.interval->tick(1000_tv);
      std::cout << "\nAfter tick 5:\n";
      print_states(*root.scenario);
  }

  void cleanup_tokens(root_scenario& rt)
  {
    rt.interval->node->requested_tokens.clear();
    rt.scenario->node->requested_tokens.clear();
    for(auto& itv : rt.scenario->get_time_intervals())
      itv->node->requested_tokens.clear();
  }

  void test_basic()
  {
    root_scenario root;

    for(int i = 0; i < 10000; i++)
      add_interval_parallel(*root.scenario);

    const int N = 1000;
    root.interval->start_and_tick();
    int64_t total = 0;
    for(int i = 0; i < N; i++)
    {
      auto t0 = std::chrono::high_resolution_clock::now();
      root.interval->tick(1000_tv);
      auto t1 = std::chrono::high_resolution_clock::now();
      total += std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
      cleanup_tokens(root);
    }

    cleanup_tokens(root);
    auto tick_us = total / double(N);
    qDebug() << tick_us;
  }

  void test_graph_parallel_fixed()
  {
    std::map<int, double> dur;
    for(auto k : benchmark_range)
    {
      root_scenario root;

      for(int i = 0; i < k; i++)
        add_interval_parallel(*root.scenario, 10000000_tv, 10000000_tv, 10000000_tv);

      const int N = 1000;
      root.interval->start_and_tick();
      int64_t total = 0;
#if USE_CALLGRIND
      CALLGRIND_START_INSTRUMENTATION;
#endif
      for(int i = 0; i < N; i++)
      {
        auto t0 = std::chrono::high_resolution_clock::now();
        root.interval->tick(1_tv);
        auto t1 = std::chrono::high_resolution_clock::now();
        total += std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();

        cleanup_tokens(root);
      }
#if USE_CALLGRIND
      CALLGRIND_STOP_INSTRUMENTATION;
#endif

      auto tick_us = total / double(N);
      dur.insert({k, tick_us});
      qDebug() << tick_us;
    }
#if USE_CALLGRIND
    CALLGRIND_DUMP_STATS;
#endif


    for(auto e : dur)
    {
      data[0].push_back(e.second);
      std::cerr << e.first << " " << e.second << '\n';
    }
  }


  void test_graph_parallel_infinite()
  {
    std::map<int, double> dur;
    for(auto k : benchmark_range)
    {
      root_scenario root;

      for(int i = 0; i < k; i++)
        add_interval_parallel(*root.scenario, 0_tv, 0_tv, ossia::Infinite);

      const int N = 1000;
      root.interval->start_and_tick();
      int64_t total = 0;
#if USE_CALLGRIND
      CALLGRIND_START_INSTRUMENTATION;
#endif
      for(int i = 0; i < N; i++)
      {
        auto t0 = std::chrono::high_resolution_clock::now();
        root.interval->tick(1_tv);
        auto t1 = std::chrono::high_resolution_clock::now();
        total += std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();

        cleanup_tokens(root);
      }
#if USE_CALLGRIND
      CALLGRIND_STOP_INSTRUMENTATION;
#endif

      auto tick_us = total / double(N);
      dur.insert({k, tick_us});
      qDebug() << tick_us;
    }
#if USE_CALLGRIND
    CALLGRIND_DUMP_STATS;
#endif


    for(auto e : dur)
    {
      data[1].push_back(e.second);
      std::cerr << e.first << " " << e.second << '\n';
    }
  }

  void test_graph_serial()
  {
    std::map<int, double> dur;
    for(auto k : benchmark_range)
    {
      root_scenario root;

      ossia::time_event* start{root.start_event.get()};
      for(int i = 0; i < k; i++)
        start = add_interval_serial(*root.scenario, *start);

      const int N = 1000;
      root.interval->start_and_tick();
      int64_t total = 0;

#if USE_CALLGRIND
      CALLGRIND_START_INSTRUMENTATION;
#endif
      for(int i = 0; i < N; i++)
      {
        auto t0 = std::chrono::high_resolution_clock::now();
        root.interval->tick(1000_tv);
        auto t1 = std::chrono::high_resolution_clock::now();
        total += std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
      }
#if USE_CALLGRIND
      CALLGRIND_STOP_INSTRUMENTATION;
#endif

      auto tick_us = total / double(N);
      dur.insert({k, tick_us});
      qDebug() << tick_us;
    }
#if USE_CALLGRIND
    CALLGRIND_DUMP_STATS;
#endif

    for(auto e : dur)
    {
      data[2].push_back(e.second);
      std::cerr << e.first << " " << e.second << '\n';
    }
  }


  void test_graph_random()
  {
    std::map<int, double> dur;
    for(auto k : benchmark_range)
    {
      root_scenario root;

      for(int i = 0; i < k; i++)
      {
        auto t = rand() % 2;
        if(t == 0)
            add_interval_parallel(*root.scenario);
        else
        {
            auto N = root.scenario->get_time_syncs().size();
            auto e = root.scenario->get_time_syncs()[rand() % N]->get_time_events()[0];

            add_interval_serial(*root.scenario, *e);
        }
      }
      const int N = 1000;
      int64_t total = 0;
      root.interval->start_and_tick();
#if USE_CALLGRIND
      CALLGRIND_START_INSTRUMENTATION;
#endif
      for(int i = 0; i < N; i++)
      {
        auto t0 = std::chrono::high_resolution_clock::now();
        root.interval->tick(10_tv);
        auto t1 = std::chrono::high_resolution_clock::now();
        total += std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
      }
#if USE_CALLGRIND
      CALLGRIND_STOP_INSTRUMENTATION;
#endif

      auto tick_us = total / double(N);
      dur.insert({k, tick_us});
      qDebug() << tick_us;
    }
#if USE_CALLGRIND
    CALLGRIND_DUMP_STATS;
#endif

    for(auto e : dur)
    {
      data[3].push_back(e.second);
      std::cerr << e.first << " " << e.second << '\n';
    }
  }

  void output()
  {
    int i = 0;

    std::cerr << std::endl;
    std::cerr << "count parfixed parinf serial random\n";
    for(auto k : benchmark_range)
    {
      std::cerr << k << ' '
                << data[0][i] << ' '
                << data[1][i] << ' '
                << data[2][i] << ' '
                << data[3][i] << '\n';
      i++;

    }
    std::cerr << std::endl;
  }

};


QTEST_APPLESS_MAIN(IntervalBenchmark)

#include "IntervalBenchmark.moc"

