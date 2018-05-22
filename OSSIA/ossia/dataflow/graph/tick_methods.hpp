#pragma once
#include <ossia/dataflow/execution_state.hpp>
#include <ossia/dataflow/graph/graph_utils.hpp>
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
    std::vector<double>& m_tickDurations;
    uint64_t rdtsc()
    {
        unsigned int lo = 0;
        unsigned int hi = 0;
        __asm__ __volatile__ (
            "lfence\n"
            "rdtsc\n"
            "lfence" : "=a"(lo), "=d"(hi)
        );
        return ((uint64_t)hi << 32) | lo;
    }

    uint64_t t0;

    cycle_count_bench(std::vector<double>& v)
      : m_tickDurations{v}, t0{rdtsc()}
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
    std::vector<double>& m_tickDurations;
    std::chrono::time_point<std::chrono::steady_clock> t0;

    clock_count_bench(std::vector<double>& v)
      : m_tickDurations{v}, t0{std::chrono::steady_clock::now()}
    {
    }

    ~clock_count_bench()
    {
      auto t1 = std::chrono::steady_clock::now();
      m_tickDurations.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count());
    }
};
struct callgrind_bench
{
    callgrind_bench()
    {
      CALLGRIND_START_INSTRUMENTATION;
    }
    ~callgrind_bench()
    {
      CALLGRIND_STOP_INSTRUMENTATION;
    }
};
}
#endif
#endif

namespace ossia
{

struct tick_all_nodes
{
    ossia::execution_state& e;
    ossia::graph_base& g;

    void operator()(unsigned long samples, double) const
    {
      e.clear_local_state();
      e.get_new_values();
      const time_value old_date{e.samples_since_start};
      e.samples_since_start += samples;
      const time_value new_date{e.samples_since_start};

      for(auto& node : g.m_nodes)
        node.first->request(
                token_request{old_date, new_date});

      g.state(e);
      e.commit();
    }
};

// 1 tick per buffer
template<void(ossia::execution_state::*Commit)()>
struct buffer_tick
{
    ossia::execution_state& st;
    ossia::graph_interface& g;
    ossia::time_interval& itv;

    void operator()(unsigned long frameCount, double seconds)
    {
      st.clear_local_state();
      st.get_new_values();
      st.samples_since_start += frameCount;
      st.bufferSize = (int)frameCount;
      // we could run a syscall and call now() but that's a bit more costly.
      st.cur_date = seconds * 1e9;
      itv.tick_offset(ossia::time_value(frameCount), 0_tv);
      g.state(st);
      (st.*Commit)();
    }
};

// 1 tick per sample
template<void(ossia::execution_state::*Commit)()>
struct precise_score_tick
{
    ossia::execution_state& st;
    ossia::graph_interface& g;
    ossia::time_interval& itv;

    void operator()(unsigned long frameCount, double seconds)
    {
      st.bufferSize = 1;
      st.cur_date = seconds * 1e9;
      for(std::size_t i = 0; i < frameCount; i++)
      {
        st.clear_local_state();
        st.get_new_values();
        st.samples_since_start++;
        itv.tick_offset(ossia::time_value(1), 0_tv);
        g.state(st);
        (st.*Commit)();

        st.advance_tick(1);
      }
    }
};

template<void(ossia::execution_state::*Commit)()>
struct split_score_tick
{
  public:
    split_score_tick(ossia::execution_state& a, ossia::graph_base& b, ossia::time_interval& c):
      st{a}, g{b}, itv{c} { }

    ossia::execution_state& st;
    ossia::graph_base& g;
    ossia::time_interval& itv;

    static void do_cuts(
        boost::container::flat_set<int64_t>& cuts,
        token_request_vec& tokens,
        time_value cur_date)
    {
      double parent_length = 0;
      for(auto& token : tokens)
      {
        if(token.date != 0 && token.position != 0)
        {
          parent_length = token.date / token.position;
          break;
        }
      }

      for(auto it = tokens.begin(); it != tokens.end(); ++it)
      {
        if(it->date > cur_date)
        {
          auto token_end_offset = it->offset + std::abs(it->date - cur_date);
          auto start_it = cuts.upper_bound(it->offset);
          while(start_it != cuts.end() && (*start_it) < token_end_offset)
          {
            auto cut = *start_it;
            auto N = cut - it->offset;
            auto inserted_token = *it;

            // make first token shorter
            it->date = cur_date + N;
            it->position = it->date / parent_length;

            // make next token
            inserted_token.offset = cut;
            it = tokens.insert(it, inserted_token);


            ++start_it;
          }
        }

        cur_date = it->date;
      }
    }

    void cut(ossia::graph_base& g)
    {
      cuts.clear();
      requests.clear();
      for(const auto& node : g.m_nodes)
      {
        for(const auto& tk : node.first->requested_tokens)
        {
          cuts.insert(tk.offset);
          cuts.insert(tk.offset + std::abs(tk.date - tk.prev_date));
        }
      }

      for(auto& node : g.m_nodes)
      {
        if(!node.first->requested_tokens.empty())
        {
          do_cuts(cuts, node.first->requested_tokens, node.first->requested_tokens.front().prev_date);
          auto it = requests.insert({node.first.get(), { std::move(node.first->requested_tokens) , {} }});
          it.first->second.second = it.first->second.first.begin(); // set iterator to begin() of token requests
          node.first->requested_tokens.clear();
        }
      }
      for(auto& cut : cuts)
      {
        st.clear_local_state();
        st.get_new_values();

        for(auto& node : g.m_nodes)
        {
          auto& req = requests[node.first.get()];
          if(req.second != req.first.end() && req.second->offset == cut)
          {
            node.first->request(*req.second);
            ++req.second;
          }
        }

        g.state(st);
        (st.*Commit)();
      }
    }

    void operator()(unsigned long frameCount, double seconds)
    {
      st.samples_since_start += frameCount;
      st.bufferSize = (int)frameCount;
      // we could run a syscall and call now() but that's a bit more costly.
      st.cur_date = seconds * 1e9;
      itv.tick_offset(ossia::time_value(frameCount), 0_tv);

      cut(g);
    }

  private:
    boost::container::flat_set<int64_t> cuts;
    std::map<const ossia::graph_node*, std::pair<ossia::token_request_vec, ossia::token_request_vec::iterator>> requests;
};
#if defined(SCORE_BENCHMARK)
template<typename BaseTick>
struct benchmark_score_tick
{
    BaseTick base;
    std::vector<double> m_tickDurations;

    void operator()(unsigned long frameCount, double seconds)
    {
      cycle_count_bench bench{m_tickDurations};
      base(frameCount, seconds);
    }
    benchmark_score_tick()
    {
      m_tickDurations.reserve(100000);
    }
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
