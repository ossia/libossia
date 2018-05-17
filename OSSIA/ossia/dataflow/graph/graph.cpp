// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/audio/audio_parameter.hpp>
#include <ossia/dataflow/graph/graph_interface.hpp>
#include <ossia/dataflow/graph/graph_utils.hpp>
#include <ossia/dataflow/graph/graph.hpp>
#include <ossia/dataflow/graph/graph_parallel.hpp>
#include <ossia/dataflow/graph/graph_static.hpp>
#include <ossia/dataflow/graph/node_executors.hpp>
#include <ossia/dataflow/graph/tick_methods.hpp>
#include <spdlog/spdlog.h>
namespace ossia
{
std::shared_ptr<bench_map> bench_ptr()
{
  static std::shared_ptr<bench_map> b = std::make_shared<bench_map>();
  return b;
}
template<typename T>
struct wrap_type { using type = T; };
std::shared_ptr<ossia::graph_interface> make_graph_impl(const ossia::graph_setup_options& opt)
{
  using namespace ossia;
  auto sched = opt.scheduling;
  auto setup = [&] (auto t) -> std::shared_ptr<ossia::graph_interface>
  {
    using exec_t = typename decltype(t)::type;
    if(sched == ossia::graph_setup_options::Dynamic)
    {
      return std::make_shared<ossia::graph>();
    }
    else if(sched == ossia::graph_setup_options::StaticBFS)
    {
      using graph_type = graph_static<
          bfs_update
        , exec_t>;

      auto g = std::make_shared<graph_type>();
      g->tick_fun.set_logger(opt.log);
      g->tick_fun.set_bench(opt.bench);
      return g;
    }
    else if(sched == ossia::graph_setup_options::StaticFixed)
    {
      using graph_type = graph_static<
          simple_update
        , exec_t>;

      auto g = std::make_shared<graph_type>();
      g->tick_fun.set_logger(opt.log);
      g->tick_fun.set_bench(opt.bench);
      return g;
    }
    else // if(sched == ossia::graph_setup_options::StaticTC)
    {
      using graph_type = graph_static<
          tc_update<fast_tc>
        , exec_t>;

      auto g = std::make_shared<graph_type>();
      g->tick_fun.set_logger(opt.log);
      g->tick_fun.set_bench(opt.bench);
      return g;
    }

  };
  if(opt.bench && opt.log)
  {
    return setup(wrap_type<ossia::static_exec_logger_bench>{});
  }
  else if(opt.bench)
  {
    return setup(wrap_type<ossia::static_exec_bench>{});
  }
  else if(opt.log)
  {
    return setup(wrap_type<ossia::static_exec_logger>{});
  }
  else
  {
    return setup(wrap_type<ossia::static_exec>{});
  }
}

std::shared_ptr<ossia::graph_interface> make_graph_par_impl(const ossia::graph_setup_options& opt)
{
  using namespace ossia;
#if defined(OSSIA_PARALLEL)

  auto sched = opt.scheduling;

  if(sched == ossia::graph_setup_options::StaticBFS)
  {
    using graph_type = graph_static<
        parallel_update<bfs_update>
      , parallel_exec>;

    auto g = std::make_shared<graph_type>();

    g->update_fun.logger = opt.log;
    g->update_fun.perf_map = opt.bench;

    return g;
  }
  else if(sched == ossia::graph_setup_options::StaticTC)
  {
    using graph_type = graph_static<
        parallel_update<tc_update<fast_tc>>
      , parallel_exec>;

    auto g = std::make_shared<graph_type>();

    g->update_fun.logger = opt.log;
    g->update_fun.perf_map = opt.bench;

    return g;
  }
  else if(sched == ossia::graph_setup_options::StaticFixed)
  {
    using graph_type = graph_static<
        parallel_update<simple_update>
      , parallel_exec>;

    auto g = std::make_shared<graph_type>();

    g->update_fun.logger = opt.log;
    g->update_fun.perf_map = opt.bench;

    return g;
  }
#endif
  return {};
}
std::shared_ptr<ossia::graph_interface> make_graph(const ossia::graph_setup_options& opt)
{
  std::shared_ptr<ossia::graph_interface> g;

#if defined(OSSIA_PARALLEL)
  if(opt.parallel)
  {
    g = make_graph_par_impl(opt);
  }
#endif

  if(!g)
  {
    g = make_graph_impl(opt);
  }
  return g;
}

smallfun::function<void(unsigned long, double), 128> make_tick(
    const tick_setup_options& settings,
    ossia::execution_state& st,
    ossia::graph_interface& gb,
    ossia::time_interval& itv)
{
  auto& g = static_cast<ossia::graph_base&>(gb);
  auto tick = settings.tick;
  auto commit = settings.commit;

  if(commit == tick_setup_options::Default)
  {
    static constexpr const auto commit_policy = &ossia::execution_state::commit;
    if(tick == tick_setup_options::Buffer)
      return ossia::buffer_tick<commit_policy>{st, g, itv};
    else if(tick == tick_setup_options::Precise)
      return ossia::precise_score_tick<commit_policy>{st, g, itv};
    else if(tick == tick_setup_options::ScoreAccurate)
      return ossia::split_score_tick<commit_policy>{st, g, itv};
    else
      return ossia::buffer_tick<commit_policy>{st, g, itv};
  }
  else if(commit == tick_setup_options::Ordered)
  {
    static constexpr const auto commit_policy = &ossia::execution_state::commit_ordered;
    if(tick == tick_setup_options::Buffer)
      return ossia::buffer_tick<commit_policy>{st, g, itv};
    else if(tick == tick_setup_options::Precise)
      return ossia::precise_score_tick<commit_policy>{st, g, itv};
    else if(tick == tick_setup_options::ScoreAccurate)
      return ossia::split_score_tick<commit_policy>{st, g, itv};
    else
      return ossia::buffer_tick<commit_policy>{st, g, itv};
  }
  else if(commit == tick_setup_options::Priorized)
  {
    static constexpr const auto commit_policy = &ossia::execution_state::commit_priorized;
    if(tick == tick_setup_options::Buffer)
      return ossia::buffer_tick<commit_policy>{st, g, itv};
    else if(tick == tick_setup_options::Precise)
      return ossia::precise_score_tick<commit_policy>{st, g, itv};
    else if(tick == tick_setup_options::ScoreAccurate)
      return ossia::split_score_tick<commit_policy>{st, g, itv};
    else
      return ossia::buffer_tick<commit_policy>{st, g, itv};
  }
  else if(commit == tick_setup_options::Merged)
  {
    static constexpr const auto commit_policy = &ossia::execution_state::commit_merged;
    if(tick == tick_setup_options::Buffer)
      return ossia::buffer_tick<commit_policy>{st, g, itv};
    else if(tick == tick_setup_options::Precise)
      return ossia::precise_score_tick<commit_policy>{st, g, itv};
    else if(tick == tick_setup_options::ScoreAccurate)
      return ossia::split_score_tick<commit_policy>{st, g, itv};
    else
      return ossia::buffer_tick<commit_policy>{st, g, itv};
  }
  return ossia::buffer_tick<&ossia::execution_state::commit>{st, g, itv};
}

struct rescale_in
{
    const ossia::time_value& prev_date;
    const ossia::token_request& request;
    void operator()(ossia::audio_port& port)
    {
      if(request.speed > 1.0)
      {
        for(auto& c : port.samples)
        {
          auto old = c;
          c.resize(c.size() * request.speed);
          for(double i = 0; i < old.size(); i += request.speed)
          {

          }
        }
      }
      else if(request.speed < 1.0)
      {

      }

    }
    void operator()(ossia::midi_port& port)
    {
      value_vector<rtmidi::message> values;
      for(auto& val : port.messages)
      {
        if(val.timestamp >= request.offset && val.timestamp < request.offset + request.date - prev_date)
        {
          values.push_back(val);
          values.back().timestamp *= request.speed;
        }
      }
      port.messages = std::move(values);
    }
    void operator()(ossia::value_port& port)
    {
      value_vector<ossia::tvalue> values;
      for(auto& val : port.get_data())
      {
        if(val.timestamp >= request.offset && val.timestamp < request.offset + request.date - prev_date)
        {
          values.push_back(val);
          values.back().timestamp *= request.speed;
        }
      }
      port.set_data(std::move(values));
    }

    void operator()()
    {

    }
};
struct rescale_out
{
    const ossia::time_value& prev_date;
    const ossia::token_request& request;
    void operator()(ossia::audio_port&)
    {

    }
    void operator()(ossia::midi_port&)
    {

    }
    void operator()(ossia::value_port&)
    {

    }

    void operator()()
    {

    }
};

void graph_util::run_scaled(graph_node& first_node, execution_state& e)
{
  std::vector<ossia::data_type> orig_ins;
  orig_ins.reserve(first_node.inputs().size());
  for(auto& n : first_node.inputs())
  {
    orig_ins.push_back(n->data);
  }


  for(const auto& request : first_node.requested_tokens)
  {
    for(auto& n : first_node.inputs())
      ossia::apply(rescale_in{first_node.prev_date(), request}, n->data);

    first_node.run(request, e);
    for(auto& n : first_node.outputs())
      ossia::apply(rescale_out{first_node.prev_date(), request}, n->data);

    first_node.set_prev_date(request.date);

    // Restore original input for the next token
    for(std::size_t i = 0; i < first_node.inputs().size(); i++)
      first_node.inputs()[i]->data = orig_ins[i];
  }
}


void graph_util::log_inputs(const graph_node& n, spdlog::logger& logger)
{
  struct
  {
      spdlog::logger& logger;
      int i = 0;
      void operator()(const ossia::value_port& p)
      {
        for(const ossia::tvalue& val : p.get_data())
          logger.log(spdlog::level::debug, "input {} (value): {}", i, val.value);
        i++;
      }
      void operator()(const ossia::audio_port& p)
      {
        //logger.log(spdlog::level::debug, "input {} (audio)");
        i++;
      }
      void operator()(const ossia::midi_port& p)
      {
        for(const rtmidi::message& val : p.messages)
        {
          switch(val.bytes.size())
          {
            case 1:
              logger.log(spdlog::level::debug, "input {} (midi): {}", i, val.bytes[0]);
              break;
            case 2:
              logger.log(spdlog::level::debug, "input {} (midi): {} {}", i, val.bytes[0], val.bytes[1]);
              break;
            case 3:
              logger.log(spdlog::level::debug, "input {} (midi): {} {} {}", i, val.bytes[0], val.bytes[1], val.bytes[2]);
              break;
            default:
              break;
          }
        }
        i++;
      }

      void operator()() { }
  } vis{logger};

  for(const ossia::inlet_ptr& in : n.inputs())
  {
    ossia::apply(vis, in->data);
  }
}

void graph_util::log_outputs(const graph_node& n, spdlog::logger& logger)
{
  struct
  {
      spdlog::logger& logger;
      int i = 0;
      void operator()(const ossia::value_port& p)
      {
        for(const ossia::tvalue& val : p.get_data())
          logger.log(spdlog::level::debug, "output {} (value): {}", i, val.value);
        i++;
      }
      void operator()(const ossia::audio_port& p)
      {
        //logger.log(spdlog::level::debug, "output {} (audio)");
        i++;
      }
      void operator()(const ossia::midi_port& p)
      {
        for(const rtmidi::message& val : p.messages)
        {
          switch(val.bytes.size())
          {
            case 1:
              logger.log(spdlog::level::debug, "output {} (midi): {}", i, val.bytes[0]);
              break;
            case 2:
              logger.log(spdlog::level::debug, "output {} (midi): {} {}", i, val.bytes[0], val.bytes[1]);
              break;
            case 3:
              logger.log(spdlog::level::debug, "output {} (midi): {} {} {}", i, val.bytes[0], val.bytes[1], val.bytes[2]);
              break;
            default:
              break;
          }
        }
        i++;
      }
      void operator()() { }
  } vis{logger};

  for(const ossia::outlet_ptr& out : n.outputs())
  {
    ossia::apply(vis, out->data);
  }

}
graph_interface::~graph_interface()
{

}
audio_parameter::~audio_parameter()
{
}

texture_generic_parameter::~texture_generic_parameter()
{
}

graph::~graph()
{
  clear();
}

}
