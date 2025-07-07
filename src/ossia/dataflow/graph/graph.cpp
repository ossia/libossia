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
#include <ossia/dataflow/graph/tick_setup.hpp>
#include <ossia/dataflow/token_request_format.hpp>
#include <ossia/network/value/format_value.hpp>

#include <boost/pool/pool.hpp>
#if !defined(OSSIA_FREESTANDING)
#include <spdlog/spdlog.h>
#endif
namespace ossia
{
using boost_pool = boost::pool<boost::default_user_allocator_malloc_free>;

struct edge_pool
{
  boost_pool pool{graph_edge::size_of_allocated_memory_by_make_shared()};
  ossia::audio_spin_mutex execution_storage_mut;
};

template <typename T>
struct edge_pool_alloc
{
  using value_type = T;

  template <typename U>
  using rebind = edge_pool_alloc<U>;

  edge_pool_alloc(std::shared_ptr<edge_pool>& pool) noexcept
      : m_pool(pool)
  {
    assert(pool_size() >= sizeof(T));
  }

  template <typename U>
  edge_pool_alloc(edge_pool_alloc<U> const& other) noexcept
      : m_pool(other.m_pool)
  {
    assert(pool_size() >= sizeof(T));
  }

  T* allocate(const size_t n) noexcept
  {
    T* ret{};
    {
      std::lock_guard lck{m_pool->execution_storage_mut};
      ret = static_cast<T*>(m_pool->pool.ordered_malloc(n));
    }
    return ret;
  }

  void deallocate(T* ptr, const size_t n) noexcept
  {
    if(ptr && n)
    {
      std::lock_guard lck{m_pool->execution_storage_mut};
      m_pool->pool.ordered_free(ptr, n);
    }
  }

  // for comparing
  size_t pool_size() const noexcept
  {
    std::lock_guard lck{m_pool->execution_storage_mut};
    return m_pool->pool.get_requested_size();
  }

  std::shared_ptr<edge_pool> m_pool;
};

graph_interface::graph_interface()
    : pool{std::make_shared<edge_pool>()}
{
  // Reserve some space
#if !defined(OSSIA_FREESTANDING)
  auto arr = pool->pool.ordered_malloc(1000);
  pool->pool.ordered_free(arr);
#endif
}

edge_ptr graph_interface::allocate_edge(
    connection c, outlet_ptr pout, inlet_ptr pin, node_ptr pout_node, node_ptr pin_node)
{
  return std::allocate_shared<ossia::graph_edge, edge_pool_alloc<graph_edge>>(
      pool, c, pout, pin, pout_node, pin_node);
}

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
    if(sched == ossia::graph_setup_options::Dynamic)
    {
      return std::make_shared<ossia::graph>();
    }
    else if(sched == ossia::graph_setup_options::StaticBFS)
    {
      using graph_type = graph_static<bfs_update, exec_t>;

      auto g = std::make_shared<graph_type>(opt);
      g->tick_fun.set_logger(opt.log);
      g->tick_fun.set_bench(opt.bench);
      return g;
    }
    else if(sched == ossia::graph_setup_options::StaticFixed)
    {
      using graph_type = graph_static<simple_update, exec_t>;

      auto g = std::make_shared<graph_type>(opt);
      g->tick_fun.set_logger(opt.log);
      g->tick_fun.set_bench(opt.bench);
      return g;
    }
    else // if(sched == ossia::graph_setup_options::StaticTC)
    {
      using graph_type = graph_static<tc_update<fast_tc>, exec_t>;

      auto g = std::make_shared<graph_type>(opt);
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

std::shared_ptr<ossia::graph_interface>
make_graph_par_impl(const ossia::graph_setup_options& opt)
{
  using namespace ossia;
#if defined(OSSIA_PARALLEL)

  auto sched = opt.scheduling;

  if(sched == ossia::graph_setup_options::StaticBFS)
  {
    using graph_type
        = graph_static<custom_parallel_update<bfs_update>, custom_parallel_exec>;

    auto g = std::make_shared<graph_type>(opt);

    g->update_fun.logger = opt.log;
    g->update_fun.perf_map = opt.bench;

    return g;
  }
  else if(sched == ossia::graph_setup_options::StaticTC)
  {
    using graph_type
        = graph_static<custom_parallel_update<tc_update<fast_tc>>, custom_parallel_exec>;

    auto g = std::make_shared<graph_type>(opt);

    g->update_fun.logger = opt.log;
    g->update_fun.perf_map = opt.bench;

    return g;
  }
  else if(sched == ossia::graph_setup_options::StaticFixed)
  {
    using graph_type
        = graph_static<custom_parallel_update<simple_update>, custom_parallel_exec>;

    auto g = std::make_shared<graph_type>(opt);

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
    const tick_setup_options& settings, ossia::execution_state& st,
    ossia::graph_interface& gb, ossia::time_interval& itv, ossia::scenario& root,
    ossia::transport_info_fun transport)
{
  auto& g = static_cast<ossia::graph_base&>(gb);
  auto tick = settings.tick;

  st.set_policy(settings);

  if(tick == tick_setup_options::Buffer)
    return ossia::buffer_tick{st, g, root, transport};
  else if(tick == tick_setup_options::Precise)
    return ossia::precise_score_tick{st, g, itv, transport};
  // else if (tick == tick_setup_options::ScoreAccurate)
  //   return ossia::split_score_tick{st, g, itv, transport};
  else
    return ossia::buffer_tick{st, g, root, transport};
}

void graph_util::check_inputs(const graph_node& n, ossia::execution_state& e)
{
  int i = 0;
  struct
  {
    const graph_node& n;
    int64_t bs;
    int& i;
    void operator()(const ossia::value_port& p) const noexcept
    {
      for(const ossia::timed_value& val : p.get_data())
      {
        if(val.timestamp < 0 || val.timestamp >= bs)
        {
          ossia::logger().error(
              "{}|{}: input {} (value)[{}]: {}", typeid(n).name(), n.label(), i,
              val.timestamp, val.value);
        }
      }
      i++;
    }
    void operator()(const ossia::audio_port& p) const noexcept
    {
      for(const auto& channel : p)
      {
        if(channel.size() != bs && !channel.empty())
          ossia::logger().error(
              "{}|{}: input {} (audio): {} != {}", typeid(n).name(), n.label(), i,
              channel.size(), bs);
        int k = 0;
        for(auto v : channel)
        {
          if(std::isnan(v))
          {
            ossia::logger().error(
                "{}|{}: input {} (audio)[{}]: NaN", typeid(n).name(), n.label(), i, k);
            return;
          }
          else if(std::isinf(v))
          {
            ossia::logger().error(
                "{}|{}: input {} (audio)[{}]: Inf", typeid(n).name(), n.label(), i, k);
            return;
          }
        }
      }
      i++;
    }
    void operator()(const ossia::midi_port& p) const noexcept
    {
      for(const libremidi::ump& val : p.messages)
      {
        if(val.timestamp < 0 || val.timestamp >= bs)
        {
          switch(val.size())
          {
            case 1:
              ossia::logger().error(
                  "{}|{}: input {} (midi)[{}]: {}", typeid(n).name(), n.label(), i,
                  val.timestamp, val.data[0]);
              break;
            case 2:
              ossia::logger().error(
                  "{}|{}: input {} (midi)[{}]: {} {}", typeid(n).name(), n.label(), i,
                  val.timestamp, val.data[0], val.data[1]);
              break;
            case 3:
              ossia::logger().error(
                  "{}|{}: input {} (midi)[{}]: {} {} {}", typeid(n).name(), n.label(), i,
                  val.timestamp, val.data[0], val.data[1], val.data[2]);
              break;
            case 4:
              ossia::logger().error(
                  "{}|{}: input {} (midi)[{}]: {} {} {} {}", typeid(n).name(), n.label(),
                  i, val.timestamp, val.data[0], val.data[1], val.data[2], val.data[3]);
              break;
            default:
              break;
          }
        }
      }
      i++;
    }
    void operator()(const ossia::geometry_port& p) const noexcept
    {
      // logger.log(spdlog::level::debug, "input {} (geometry)");
      i++;
    }

    void operator()() const noexcept { }
  } vis{n, e.bufferSize, i};

  for_each_inlet(n, [&](auto& in) { in.visit(vis); });
}

void graph_util::check_outputs(
    const graph_node& n, ossia::execution_state& e, const ossia::token_request& req)
{
  int i = 0;
  struct
  {
    const graph_node& n;
    const ossia::token_request& req;
    int64_t bs;
    int& i;
    void operator()(const ossia::value_port& p) const noexcept
    {
      for(const ossia::timed_value& val : p.get_data())
      {
        if(val.timestamp < 0 || val.timestamp >= bs)
        {
          ossia::logger().error(
              "{}|{}: output {} (value)[{}]: {} ; {}", typeid(n).name(), n.label(), i,
              val.timestamp, val.value, req);
        }
      }
      i++;
    }
    void operator()(const ossia::audio_port& p) const noexcept
    {
      for(const auto& channel : p)
      {
        if(channel.size() != bs && !channel.empty())
          ossia::logger().error(
              "{}|{}: output {} (audio): {} != {} ; {}", typeid(n).name(), n.label(), i,
              channel.size(), bs, req);
        int k = 0;
        for(auto v : channel)
        {
          if(std::isnan(v))
          {
            ossia::logger().error(
                "{}|{}: output {} (audio)[{}]: NaN", typeid(n).name(), n.label(), i, k);
            return;
          }
          else if(std::isinf(v))
          {
            ossia::logger().error(
                "{}|{}: output {} (audio)[{}]: Inf", typeid(n).name(), n.label(), i, k);
            return;
          }
        }
      }
      i++;
    }
    void operator()(const ossia::midi_port& p) const noexcept
    {
      for(const libremidi::ump& val : p.messages)
      {
        if(val.timestamp < 0 || val.timestamp >= bs)
        {
          switch(val.size())
          {
            case 1:
              ossia::logger().error(
                  "{}|{}: output {} (midi)[{}]: {}", typeid(n).name(), n.label(), i,
                  val.timestamp, val.data[0]);
              break;
            case 2:
              ossia::logger().error(
                  "{}|{}: output {} (midi)[{}]: {} {}", typeid(n).name(), n.label(), i,
                  val.timestamp, val.data[0], val.data[1]);
              break;
            case 3:
              ossia::logger().error(
                  "{}|{}: output {} (midi)[{}]: {} {} {}", typeid(n).name(), n.label(),
                  i, val.timestamp, val.data[0], val.data[1], val.data[2]);
              break;
            case 4:
              ossia::logger().error(
                  "{}|{}: output {} (midi)[{}]: {} {} {} {}", typeid(n).name(),
                  n.label(), i, val.timestamp, val.data[0], val.data[1], val.data[2],
                  val.data[3]);
              break;
            default:
              break;
          }
        }
      }
      i++;
    }
    void operator()(const ossia::geometry_port& p) const noexcept
    {
      // logger.log(spdlog::level::debug, "output {} (geometry)");
      i++;
    }
    void operator()() const noexcept { }
  } vis{n, req, e.bufferSize, i};

  for_each_outlet(n, [&](auto& out) { out.visit(vis); });
}
void graph_util::log_inputs(const graph_node& n, ossia::logger_type& logger)
{
#if !defined(OSSIA_FREESTANDING)
  int i = 0;
  struct
  {
    ossia::logger_type& logger;
    int& i;
    int64_t frames;
    void operator()(const ossia::value_port& p) const noexcept
    {
      for(const ossia::timed_value& val : p.get_data())
        logger.log(
            spdlog::level::debug, "{}: input {} (value)[{}]: {}", frames, i,
            val.timestamp, val.value);
      i++;
    }
    void operator()(const ossia::audio_port& p) const noexcept
    {
      // logger.log(spdlog::level::debug, "input {} (audio)");
      i++;
    }
    void operator()(const ossia::midi_port& p) const noexcept
    {
      for(const libremidi::ump& val : p.messages)
      {
        switch(val.size())
        {
          case 1:
            logger.log(
                spdlog::level::debug, "{}: input {} (midi)[{}]: {}", frames, i,
                val.timestamp, val.data[0]);
            break;
          case 2:
            logger.log(
                spdlog::level::debug, "{}: input {} (midi)[{}]: {} {}", frames, i,
                val.timestamp, val.data[0], val.data[1]);
            break;
          case 3:
            logger.log(
                spdlog::level::debug, "{}: input {} (midi)[{}]: {} {} {}", frames, i,
                val.timestamp, val.data[0], val.data[1], val.data[2]);
            break;
          case 4:
            logger.log(
                spdlog::level::debug, "{}: input {} (midi)[{}]: {} {} {} {}", frames, i,
                val.timestamp, val.data[0], val.data[1], val.data[2], val.data[3]);
            break;
          default:
            break;
        }
      }
      i++;
    }
    void operator()(const ossia::geometry_port& p) const noexcept
    {
      // logger.log(spdlog::level::debug, "input {} (geometry)");
      i++;
    }

    void operator()() const noexcept { }
  } vis{logger, i, n.processed_frames()};

  for_each_inlet(n, [&](auto& in) { in.visit(vis); });
#endif
}

void graph_util::log_outputs(const graph_node& n, ossia::logger_type& logger)
{
#if !defined(OSSIA_FREESTANDING)
  int i = 0;
  struct
  {
    ossia::logger_type& logger;
    int& i;
    int64_t frames;
    void operator()(const ossia::value_port& p) const noexcept
    {
      for(const ossia::timed_value& val : p.get_data())
        logger.log(
            spdlog::level::debug, "{}: output {} (value)[{}]: {}", frames, i,
            val.timestamp, val.value);
      i++;
    }
    void operator()(const ossia::audio_port& p) const noexcept
    {
      // logger.log(spdlog::level::debug, "output {} (audio)");
      i++;
    }
    void operator()(const ossia::midi_port& p) const noexcept
    {
      for(const libremidi::ump& val : p.messages)
      {
        switch(val.size())
        {
          case 1:
            logger.log(
                spdlog::level::debug, "{}: output {} (midi)[{}]: {}", frames, i,
                val.timestamp, val.data[0]);
            break;
          case 2:
            logger.log(
                spdlog::level::debug, "{}: output {} (midi)[{}]: {} {}", frames, i,
                val.timestamp, val.data[0], val.data[1]);
            break;
          case 3:
            logger.log(
                spdlog::level::debug, "{}: output {} (midi)[{}]: {} {} {}", frames, i,
                val.timestamp, val.data[0], val.data[1], val.data[2]);
            break;
          case 4:
            logger.log(
                spdlog::level::debug, "{}: output {} (midi)[{}]: {} {} {} {}", frames, i,
                val.timestamp, val.data[0], val.data[1], val.data[2], val.data[3]);
            break;
          default:
            break;
        }
      }
      i++;
    }
    void operator()(const ossia::geometry_port& p) const noexcept
    {
      // logger.log(spdlog::level::debug, "output {} (geometry)");
      i++;
    }
    void operator()() const noexcept { }
  } vis{logger, i, n.processed_frames()};

  for_each_outlet(n, [&](auto& out) { out.visit(vis); });
#endif
}
graph_interface::~graph_interface() = default;
graph::~graph()
{
  clear();
}
}
