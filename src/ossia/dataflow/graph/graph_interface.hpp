#pragma once
#include <ossia/dataflow/dataflow_fwd.hpp>
#include <ossia/dataflow/transport.hpp>
#include <ossia/detail/logger_fwd.hpp>

#include <boost/pool/pool.hpp>

#include <tcb/span.hpp>

#include <smallfun.hpp>

namespace spdlog
{
class logger;
}
namespace ossia
{

using boost_pool = boost::pool<boost::default_user_allocator_malloc_free>;

struct edge_pool
{
  boost_pool pool{1024 * 1024};
  ossia::audio_spin_mutex execution_storage_mut;
};

struct bench_map;
struct connection;
class time_interval;
class OSSIA_EXPORT graph_interface
{
public:
  graph_interface();
  virtual ~graph_interface();
  [[deprecated]] virtual void add_node(ossia::node_ptr) = 0;

  [[deprecated]] virtual void remove_node(const ossia::node_ptr&) = 0;

  [[deprecated]] virtual void connect(ossia::edge_ptr) = 0;

  [[deprecated]] virtual void disconnect(const ossia::edge_ptr&) = 0;

  [[deprecated]] virtual void disconnect(ossia::graph_edge*) = 0;

  [[deprecated]] virtual void mark_dirty() = 0;

  virtual void state(execution_state& e) = 0;

  virtual void clear() = 0;

  virtual void print(std::ostream&) = 0;

  [[nodiscard]] virtual tcb::span<ossia::graph_node* const>
  get_nodes() const noexcept = 0;

  std::shared_ptr<edge_pool> pool;

  ossia::edge_ptr allocate_edge(
      connection c, outlet_ptr pout, inlet_ptr pin, node_ptr pout_node,
      node_ptr pin_node);
};

struct graph_setup_options
{
  enum
  {
    StaticFixed,
    StaticBFS,
    StaticTC,
    Dynamic
  } scheduling{};
  enum
  {
    Creation,
    XY,
    YX,
    Temporal
  } order{};
  enum
  {
    Merge,
    Append,
    Replace
  } merge{};

  bool parallel{};
  std::shared_ptr<ossia::logger_type> log{};
  std::shared_ptr<bench_map> bench{};
};

struct tick_setup_options
{
  enum
  {
    Default,
    Ordered,
    Priorized,
    Merged
  } commit{};
  enum
  {
    Buffer,
    ScoreAccurate,
    Precise
  } tick{};
};

OSSIA_EXPORT
std::shared_ptr<ossia::graph_interface> make_graph(const graph_setup_options&);

}
