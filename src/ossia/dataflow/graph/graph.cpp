// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/audio/audio_parameter.hpp>
#include <ossia/dataflow/graph/graph.hpp>
#include <ossia/dataflow/graph/graph_interface.hpp>
#include <ossia/dataflow/graph/graph_parallel.hpp>
#include <ossia/dataflow/graph/graph_static.hpp>
#include <ossia/dataflow/graph/graph_utils.hpp>
#include <ossia/dataflow/graph/node_executors.hpp>
#include <ossia/dataflow/graph/tick_methods.hpp>
#include <ossia/network/value/format_value.hpp>

#include <spdlog/spdlog.h>
namespace ossia
{
std::shared_ptr<bench_map> bench_ptr()
{
  static std::shared_ptr<bench_map> b = std::make_shared<bench_map>();
  return b;
}
template <typename T>
struct wrap_type
{
  using type = T;
};
std::shared_ptr<ossia::graph_interface>
make_graph_impl(const ossia::graph_setup_options& opt)
{
  using namespace ossia;
  auto sched = opt.scheduling;
  auto setup = [&](auto t) -> std::shared_ptr<ossia::graph_interface> {
    using exec_t = typename decltype(t)::type;
    if (sched == ossia::graph_setup_options::Dynamic)
    {
      return std::make_shared<ossia::graph>();
    }
    else if (sched == ossia::graph_setup_options::StaticBFS)
    {
      using graph_type = graph_static<bfs_update, exec_t>;

      auto g = std::make_shared<graph_type>();
      g->tick_fun.set_logger(opt.log);
      g->tick_fun.set_bench(opt.bench);
      return g;
    }
    else if (sched == ossia::graph_setup_options::StaticFixed)
    {
      using graph_type = graph_static<simple_update, exec_t>;

      auto g = std::make_shared<graph_type>();
      g->tick_fun.set_logger(opt.log);
      g->tick_fun.set_bench(opt.bench);
      return g;
    }
    else // if(sched == ossia::graph_setup_options::StaticTC)
    {
      using graph_type = graph_static<tc_update<fast_tc>, exec_t>;

      auto g = std::make_shared<graph_type>();
      g->tick_fun.set_logger(opt.log);
      g->tick_fun.set_bench(opt.bench);
      return g;
    }
  };
  if (opt.bench && opt.log)
  {
    return setup(wrap_type<ossia::static_exec_logger_bench>{});
  }
  else if (opt.bench)
  {
    return setup(wrap_type<ossia::static_exec_bench>{});
  }
  else if (opt.log)
  {
    return setup(wrap_type<ossia::static_exec_logger>{});
  }
  else
  {
    return setup(wrap_type<ossia::static_exec>{});
  }
}

std::shared_ptr<ossia::graph_interface>
make_graph_par_impl(const ossia::graph_setup_options& opt)
{
  using namespace ossia;
#if defined(OSSIA_PARALLEL)

  auto sched = opt.scheduling;

  if (sched == ossia::graph_setup_options::StaticBFS)
  {
    using graph_type
        = graph_static<cpptf_update<bfs_update>, cpptf_exec>;

    auto g = std::make_shared<graph_type>();

    g->update_fun.logger = opt.log;
    g->update_fun.perf_map = opt.bench;

    return g;
  }
  else if (sched == ossia::graph_setup_options::StaticTC)
  {
    using graph_type
        = graph_static<cpptf_update<tc_update<fast_tc>>, cpptf_exec>;

    auto g = std::make_shared<graph_type>();

    g->update_fun.logger = opt.log;
    g->update_fun.perf_map = opt.bench;

    return g;
  }
  else if (sched == ossia::graph_setup_options::StaticFixed)
  {
    using graph_type
        = graph_static<cpptf_update<simple_update>, cpptf_exec>;

    auto g = std::make_shared<graph_type>();

    g->update_fun.logger = opt.log;
    g->update_fun.perf_map = opt.bench;

    return g;
  }
#endif
  return {};
}
std::shared_ptr<ossia::graph_interface>
make_graph(const ossia::graph_setup_options& opt)
{
  std::shared_ptr<ossia::graph_interface> g;

#if defined(OSSIA_PARALLEL)
  if (opt.parallel)
  {
    g = make_graph_par_impl(opt);
  }
#endif

  if (!g)
  {
    g = make_graph_impl(opt);
  }
  return g;
}

smallfun::function<void(unsigned long, double), 128> make_tick(
    const tick_setup_options& settings, ossia::execution_state& st,
    ossia::graph_interface& gb, ossia::time_interval& itv)
{
  auto& g = static_cast<ossia::graph_base&>(gb);
  auto tick = settings.tick;
  auto commit = settings.commit;

  if (commit == tick_setup_options::Default)
  {
    static constexpr const auto commit_policy
        = &ossia::execution_state::commit;
    if (tick == tick_setup_options::Buffer)
      return ossia::buffer_tick<commit_policy>{st, g, itv};
    else if (tick == tick_setup_options::Precise)
      return ossia::precise_score_tick<commit_policy>{st, g, itv};
    //else if (tick == tick_setup_options::ScoreAccurate)
    //  return ossia::split_score_tick<commit_policy>{st, g, itv};
    else
      return ossia::buffer_tick<commit_policy>{st, g, itv};
  }
  else if (commit == tick_setup_options::Ordered)
  {
    static constexpr const auto commit_policy
        = &ossia::execution_state::commit_ordered;
    if (tick == tick_setup_options::Buffer)
      return ossia::buffer_tick<commit_policy>{st, g, itv};
    else if (tick == tick_setup_options::Precise)
      return ossia::precise_score_tick<commit_policy>{st, g, itv};
    //else if (tick == tick_setup_options::ScoreAccurate)
    //  return ossia::split_score_tick<commit_policy>{st, g, itv};
    else
      return ossia::buffer_tick<commit_policy>{st, g, itv};
  }
  else if (commit == tick_setup_options::Priorized)
  {
    static constexpr const auto commit_policy
        = &ossia::execution_state::commit_priorized;
    if (tick == tick_setup_options::Buffer)
      return ossia::buffer_tick<commit_policy>{st, g, itv};
    else if (tick == tick_setup_options::Precise)
      return ossia::precise_score_tick<commit_policy>{st, g, itv};
    //else if (tick == tick_setup_options::ScoreAccurate)
    //  return ossia::split_score_tick<commit_policy>{st, g, itv};
    else
      return ossia::buffer_tick<commit_policy>{st, g, itv};
  }
  else if (commit == tick_setup_options::Merged)
  {
    static constexpr const auto commit_policy
        = &ossia::execution_state::commit_merged;
    if (tick == tick_setup_options::Buffer)
      return ossia::buffer_tick<commit_policy>{st, g, itv};
    else if (tick == tick_setup_options::Precise)
      return ossia::precise_score_tick<commit_policy>{st, g, itv};
    //else if (tick == tick_setup_options::ScoreAccurate)
    //  return ossia::split_score_tick<commit_policy>{st, g, itv};
    else
      return ossia::buffer_tick<commit_policy>{st, g, itv};
  }
  return ossia::buffer_tick<&ossia::execution_state::commit>{st, g, itv};
}

void graph_util::log_inputs(const graph_node& n, spdlog::logger& logger)
{
  int i = 0;
  struct
  {
    spdlog::logger& logger;
    int& i;
    void operator()(const ossia::value_port& p) const noexcept
    {
      for (const ossia::timed_value& val : p.get_data())
        logger.log(spdlog::level::debug, "input {} (value): {}", i, val.value);
      i++;
    }
    void operator()(const ossia::audio_port& p) const noexcept
    {
      // logger.log(spdlog::level::debug, "input {} (audio)");
      i++;
    }
    void operator()(const ossia::midi_port& p) const noexcept
    {
      for (const rtmidi::message& val : p.messages)
      {
        switch (val.bytes.size())
        {
          case 1:
            logger.log(
                spdlog::level::debug, "input {} (midi): {}", i, val.bytes[0]);
            break;
          case 2:
            logger.log(
                spdlog::level::debug, "input {} (midi): {} {}", i,
                val.bytes[0], val.bytes[1]);
            break;
          case 3:
            logger.log(
                spdlog::level::debug, "input {} (midi): {} {} {}", i,
                val.bytes[0], val.bytes[1], val.bytes[2]);
            break;
          default:
            break;
        }
      }
      i++;
    }

    void operator()() const noexcept
    {
    }
  } vis{logger, i};

  n.for_each_inlet([&] (auto& in) { in.visit(vis); });
}

void graph_util::log_outputs(const graph_node& n, spdlog::logger& logger)
{
  int i = 0;
  struct
  {
    spdlog::logger& logger;
    int& i;
    void operator()(const ossia::value_port& p) const noexcept
    {
      for (const ossia::timed_value& val : p.get_data())
        logger.log(
            spdlog::level::debug, "output {} (value): {}", i, val.value);
      i++;
    }
    void operator()(const ossia::audio_port& p) const noexcept
    {
      // logger.log(spdlog::level::debug, "output {} (audio)");
      i++;
    }
    void operator()(const ossia::midi_port& p) const noexcept
    {
      for (const rtmidi::message& val : p.messages)
      {
        switch (val.bytes.size())
        {
          case 1:
            logger.log(
                spdlog::level::debug, "output {} (midi): {}", i, val.bytes[0]);
            break;
          case 2:
            logger.log(
                spdlog::level::debug, "output {} (midi): {} {}", i,
                val.bytes[0], val.bytes[1]);
            break;
          case 3:
            logger.log(
                spdlog::level::debug, "output {} (midi): {} {} {}", i,
                val.bytes[0], val.bytes[1], val.bytes[2]);
            break;
          default:
            break;
        }
      }
      i++;
    }
    void operator()() const noexcept
    {
    }
  } vis{logger, i};

  n.for_each_outlet([&] (auto& out) { out.visit(vis); });
}
graph_interface::~graph_interface()
{
}
audio_parameter::~audio_parameter()
{
}

graph::~graph()
{
  clear();
}
}
