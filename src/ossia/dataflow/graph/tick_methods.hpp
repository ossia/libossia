#pragma once
#include <ossia/audio/audio_tick.hpp>
#include <ossia/dataflow/execution_state.hpp>
#include <ossia/dataflow/graph/graph_interface.hpp>
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/detail/disable_fpe.hpp>
#include <ossia/detail/hash_map.hpp>
#include <ossia/detail/pod_vector.hpp>
#include <ossia/editor/scenario/execution_log.hpp>
#include <ossia/editor/scenario/scenario.hpp>
#include <ossia/editor/scenario/time_interval.hpp>

#if defined(SCORE_BENCHMARK)
#if __has_include(<valgrind/callgrind.h>)
#include <QFile>
#include <QTextStream>

#include <valgrind/callgrind.h>
namespace ossia
{

struct cycle_count_bench
{
  ossia::double_vector& m_tickDurations;
  uint64_t rdtsc()
  {
    unsigned int lo = 0;
    unsigned int hi = 0;
    __asm__ __volatile__(
        "lfence\n"
        "rdtsc\n"
        "lfence"
        : "=a"(lo), "=d"(hi));
    return ((uint64_t)hi << 32) | lo;
  }

  uint64_t t0;

  cycle_count_bench(ossia::double_vector& v)
      : m_tickDurations{v}
      , t0{rdtsc()}
  {
  }

  ~cycle_count_bench()
  {
    auto t1 = rdtsc();
    m_tickDurations.push_back(t1 - t0);
  }
};

struct clock_count_bench
{
  ossia::double_vector& m_tickDurations;
  std::chrono::time_point<std::chrono::steady_clock> t0;

  clock_count_bench(ossia::double_vector& v)
      : m_tickDurations{v}
      , t0{std::chrono::steady_clock::now()}
  {
  }

  ~clock_count_bench()
  {
    auto t1 = std::chrono::steady_clock::now();
    m_tickDurations.push_back(
        std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count());
  }
};
struct callgrind_bench
{
  callgrind_bench() { CALLGRIND_START_INSTRUMENTATION; }
  ~callgrind_bench() { CALLGRIND_STOP_INSTRUMENTATION; }
};
}
#endif
#endif

namespace ossia
{

struct tick_all_nodes
{
  ossia::execution_state& e;
  ossia::graph_interface& g;

  void operator()(const ossia::audio_tick_state& st) { (*this)(st.frames, st.seconds); }

  void operator()(unsigned long samples, double) const
  {
    ossia::disable_fpe();
    std::atomic_thread_fence(std::memory_order_seq_cst);
    e.begin_tick();
    const time_value old_date{e.samples_since_start};
    e.samples_since_start += samples;
    const time_value new_date{e.samples_since_start};

    // TODO tempo / sig ?
    for(auto& node : g.get_nodes())
      node->request(
          token_request{old_date, new_date, 0_tv, 0_tv, 1.0, {}, ossia::root_tempo});

    g.state(e);
    std::atomic_thread_fence(std::memory_order_seq_cst);
    e.commit();
  }
};

// 1 tick per buffer
struct buffer_tick
{
  ossia::execution_state& st;
  ossia::graph_interface& g;
  ossia::scenario& scenar;
  ossia::transport_info_fun transport;

  void operator()(const ossia::audio_tick_state& st) { (*this)(st.frames, st.seconds); }

  void operator()(unsigned long frameCount, double seconds)
  {
    auto& itv = **scenar.get_time_intervals().begin();
#if defined(OSSIA_EXECUTION_LOG)
    auto log = g_exec_log.start_tick();
#endif

    ossia::disable_fpe();
    std::atomic_thread_fence(std::memory_order_seq_cst);
    st.begin_tick();
    st.samples_since_start += frameCount;
    st.bufferSize = (int)frameCount;
    // we could run a syscall and call now() but that's a bit more costly.
    st.cur_date = seconds * 1e9;

    const auto flicks = frameCount * st.samplesToModelRatio;

    ossia::token_request tok{};
    tok.prev_date = scenar.last_date();

    // FIXME this is a bit ugly..
    if(tok.prev_date == ossia::Infinite)
      tok.prev_date = 0_tv;

    tok.date = tok.prev_date + flicks;

    // Notify the current transport state
    if(transport.allocated())
    {
      transport(itv.current_transport_info());
    }

    // Temporal tick
    {
#if defined(OSSIA_EXECUTION_LOG)
      auto log = g_exec_log.start_temporal();
#endif

      scenar.state_impl(tok);
    }

    // Dataflow execution
    {
#if defined(OSSIA_EXECUTION_LOG)
      auto log = g_exec_log.start_dataflow();
#endif

      g.state(st);
    }

    std::atomic_thread_fence(std::memory_order_seq_cst);

    // Apply messages
    {
#if defined(OSSIA_EXECUTION_LOG)
      auto log = g_exec_log.start_commit();
#endif

      st.commit();
    }

#if defined(OSSIA_SCENARIO_DATAFLOW)
    // Clear the scenario token
    {
      scenar.node->requested_tokens.clear();
    }
#endif
  }
};

// 1 tick per sample
struct precise_score_tick
{
  ossia::execution_state& st;
  ossia::graph_interface& g;
  ossia::time_interval& itv;
  ossia::transport_info_fun transport;

  void operator()(const ossia::audio_tick_state& st) { (*this)(st.frames, st.seconds); }

  void operator()(unsigned long frameCount, double seconds)
  {
    ossia::disable_fpe();
    std::atomic_thread_fence(std::memory_order_seq_cst);
    st.bufferSize = 1;
    st.cur_date = seconds * 1e9;
    for(std::size_t i = 0; i < frameCount; i++)
    {
      st.begin_tick();
      st.samples_since_start++;
      const ossia::token_request tok{};
      itv.tick_offset(ossia::time_value{1}, 0_tv, tok);
      g.state(st);
      std::atomic_thread_fence(std::memory_order_seq_cst);
      st.commit();

      st.advance_tick(1);
      std::atomic_thread_fence(std::memory_order_seq_cst);
    }
  }
};

/*
struct split_score_tick
{
public:
  split_score_tick(
      ossia::execution_state& a, ossia::graph_interface& b,
      ossia::time_interval& c)
      : st{a}, g{b}, itv{c}
  {
  }

  ossia::execution_state& st;
  ossia::graph_interface& g;
  ossia::time_interval& itv;
  ossia::transport_info_fun transport;

  static void do_cuts(
      ossia::flat_set<int64_t>& cuts, token_request_vec& tokens,
      time_value cur_date)
  {
    for (auto it = tokens.begin(); it != tokens.end(); ++it)
    {
      if (it->date > cur_date)
      {
        auto token_end_offset = it->offset + abs(it->date - cur_date);
        auto start_it = cuts.upper_bound(it->offset);
        while (start_it != cuts.end() && (*start_it) < token_end_offset)
        {
          auto cut = *start_it;
          auto N = cut - it->offset;
          auto inserted_token = *it;

          // make first token shorter
          it->date = cur_date + N;

          // make next token
          inserted_token.offset = cut;
          it = tokens.insert(it, inserted_token);

          ++start_it;
        }
      }

      cur_date = it->date;
    }
  }

  void cut(ossia::graph_interface& g)
  {
    cuts.clear();
    requests.clear();
    for (const auto& node : g.get_nodes())
    {
      for (const auto& tk : node->requested_tokens)
      {
        cuts.insert(tk.offset.impl);
        cuts.insert((tk.offset + abs(tk.date - tk.prev_date)).impl);
      }
    }

    for (auto& node : g.get_nodes())
    {
      if (!node->requested_tokens.empty())
      {
        do_cuts(
            cuts, node->requested_tokens,
            node->requested_tokens.front().prev_date);
        auto it
            = requests.insert({node, {std::move(node->requested_tokens), {}}});
        it.first->second.second
            = it.first->second.first
                  .begin(); // set iterator to begin() of token requests
        node->requested_tokens.clear();
      }
    }
    for (auto& cut : cuts)
    {
      st.begin_tick();

      for (auto& node : g.get_nodes())
      {
        auto& req = requests[node];
        if (req.second != req.first.end() && req.second->offset == cut)
        {
          node->request(*req.second);
          ++req.second;
        }
      }

      g.state(st);
      (st.*Commit)();
    }
  }

  void operator()(const ossia::audio_tick_state& st)
  {
    (*this)(st.frames, st.seconds);
  }

  void operator()(unsigned long frameCount, double seconds)
  {
    ossia::disable_fpe();
    st.samples_since_start += frameCount;
    st.bufferSize = (int)frameCount;
    // we could run a syscall and call now() but that's a bit more costly.
    st.cur_date = seconds * 1e9;
    const ossia::token_request tok{};
    itv.tick_offset(ossia::time_value{int64_t(frameCount)}, 0_tv, tok);

    cut(g);
  }

private:
  ossia::flat_set<int64_t> cuts;
  ossia::hash_map<
      const ossia::graph_node*,
      std::pair<ossia::token_request_vec, ossia::token_request_vec::iterator>>
      requests;
};
*/
#if defined(SCORE_BENCHMARK)
template <typename BaseTick>
struct benchmark_score_tick
{
  BaseTick base;
  ossia::double_vector m_tickDurations;

  void operator()(const ossia::audio_tick_state& st) { (*this)(st.frames, st.seconds); }

  void operator()(unsigned long frameCount, double seconds)
  {
    cycle_count_bench bench{m_tickDurations};
    base(frameCount, seconds);
  }
  benchmark_score_tick() { m_tickDurations.reserve(100000); }
  ~benchmark_score_tick()
  {
    QFile f("/tmp/out.data");
    QTextStream s(&f);
    f.open(QIODevice::WriteOnly);
    for(auto t : m_tickDurations)
      s << t << "\n";
  }
};
#endif
}
