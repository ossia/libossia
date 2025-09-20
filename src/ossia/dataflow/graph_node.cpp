// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/dataflow/dataflow.hpp>
#include <ossia/dataflow/execution_state.hpp>
#include <ossia/dataflow/for_each_port.hpp>
#include <ossia/dataflow/fx_node.hpp>
#include <ossia/dataflow/graph_edge.hpp>
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/dataflow/node_process.hpp>
#include <ossia/detail/algorithms.hpp>
#include <ossia/detail/logger.hpp>

namespace ossia
{
audio_buffer_pool::audio_buffer_pool() = default;
audio_buffer_pool::~audio_buffer_pool() = default;

audio_buffer_pool& audio_buffer_pool::instance() noexcept
{
  static audio_buffer_pool p;
  return p;
}

#if defined(OSSIA_SCENARIO_DATAFLOW)
node_process::node_process(node_ptr n)
{
  assert(n);
  node = std::move(n);
}

void node_process::offset_impl(time_value date) { }

void node_process::transport_impl(time_value date) { }

void node_process::start()
{
  // TODO reset all delay buffer positions
}

void node_process::stop()
{
  if(node)
    node->all_notes_off();
}

void node_process::pause() { }

void node_process::resume() { }

void node_process::mute_impl(bool) { }

node_process::~node_process() = default;
#endif
/*
graph_edge::graph_edge(
    connection c, std::size_t pout, std::size_t pin, node_ptr pout_node,
    node_ptr pin_node) noexcept
    : graph_edge{c, pout_node->outputs()[pout], pin_node->inputs()[pin],
                 std::move(pout_node), std::move(pin_node)}
{
}
*/
graph_edge::graph_edge(
    connection c, outlet_ptr pout, inlet_ptr pin, node_ptr pout_node,
    node_ptr pin_node) noexcept
    : con{c}
    , out{std::move(pout)}
    , in{std::move(pin)}
    , out_node{std::move(pout_node)}
    , in_node{std::move(pin_node)}
{
  assert(out_node);
  assert(in_node);
}

template <typename T>
struct alloc_observer
{
  std::allocator<T> alloc;
  std::size_t& request;
  template <typename U>
  using rebind = alloc_observer<U>;
  using value_type = T;

  alloc_observer(std::size_t& request) noexcept
      : request(request)
  {
    // Tricky note: std::make/allocate_shared store the allocator. Thus the allocated size depend on alloc_observer / edge_pool_alloc size.
    // To get the correct size, we have to make sure that sizeof(alloc_observer) == sizeof(edge_pool_alloc) (which is just a shared_ptr)

    static_assert(sizeof(alloc_observer<T>) == sizeof(std::shared_ptr<int>));
  }

  template <typename U>
  alloc_observer(alloc_observer<U> const& other) noexcept
      : request(other.request)
  {
    static_assert(sizeof(alloc_observer<T>) == sizeof(std::shared_ptr<int>));
    static_assert(sizeof(alloc_observer<U>) == sizeof(std::shared_ptr<int>));
  }

  T* allocate(const std::size_t n) noexcept
  {
    request = sizeof(T);
    return alloc.allocate(n);
  }
  void deallocate(T* ptr, const size_t n) noexcept { return alloc.deallocate(ptr, n); }
};

std::size_t graph_edge::size_of_allocated_memory_by_make_shared() noexcept
{
  struct private_bypass : graph_edge
  {
  public:
    private_bypass() = default;
  };
  std::size_t request{};
  alloc_observer<private_bypass> alloc{request};

  std::allocate_shared<private_bypass, alloc_observer<private_bypass>>(alloc);

  return request;
}

void graph_edge::init() noexcept
{
  if(in && out && in_node && out_node)
  {
    bool found_in = false;
    bool found_out = false;
    ossia::for_each_inlet(*in_node, [&](ossia::inlet& e) { found_in |= &e == in; });
    ossia::for_each_outlet(*out_node, [&](ossia::outlet& e) { found_out |= &e == out; });
    if(!found_in || !found_out)
    {
      ossia::logger().error("Trying to connect missing inlets / outlets");
      return;
    }
    out->connect(this);
    in->connect(this);

    if(auto delay = con.target<delayed_glutton_connection>())
    {
      out->visit(init_delay_line{delay->buffer});
    }
    else if(auto sdelay = con.target<delayed_strict_connection>())
    {
      out->visit(init_delay_line{sdelay->buffer});
    }
  }
  else
  {
    ossia::logger().error("Missing in_node / out_node");
  }
}

graph_edge::~graph_edge()
{
  clear();
}

void graph_edge::clear() noexcept
{
  if(in && out && in_node && out_node)
  {
    bool found_in = false;
    bool found_out = false;
    ossia::for_each_inlet(*in_node, [&](ossia::inlet& e) { found_in |= &e == in; });
    ossia::for_each_outlet(*out_node, [&](ossia::outlet& e) { found_out |= &e == out; });
    if(found_in && found_out)
    {
      out->disconnect(this);
      in->disconnect(this);
    }
    else
    {
      ossia::logger().error("Trying to disconnect missing inlets / outlets");
    }
  }

  con = connection{};
  out = {};
  in = {};
  out_node.reset();
  in_node.reset();
}

graph_node::~graph_node()
{
  for(auto inl : m_inlets)
    delete inl;
  for(auto outl : m_outlets)
    delete outl;
}

void graph_node::prepare(const execution_state& st) noexcept
{
  struct
  {
    std::size_t buffer_size{};
    void operator()(ossia::audio_port& p) const noexcept
    {
      if(p.empty())
        p.set_channels(2);

      for(auto& c : p.get())
      {
        c.shrink_to_fit();
        c.reserve(buffer_size);
      }
    }
    void operator()(ossia::midi_port& p) const noexcept { }
    void operator()(ossia::value_port& p) const noexcept { }
    void operator()(ossia::geometry_port& p) const noexcept { }
  } vis;
  for(auto& in : this->m_inlets)
  {
    in->visit(vis);
  }
  for(auto& out : this->m_outlets)
  {
    out->visit(vis);
  }
}

graph_node::graph_node() noexcept = default;

bool graph_node::consumes(const execution_state&) const noexcept
{
  return false;
}

void graph_node::run(const token_request& t, exec_state_facade) noexcept { }

std::string graph_node::label() const noexcept
{
  return {};
}

bool graph_node::has_port_inputs() const noexcept
{
  return any_of_inlet(*this, [](const inlet& inlet) { return !inlet.sources.empty(); });
}

bool graph_node::has_global_inputs() const noexcept
{
  return any_of_inlet(*this, [&](const inlet& inlet) {
    return (inlet.scope & port::scope_t::global) && bool(inlet.address);
  });
}

bool graph_node::has_local_inputs(const execution_state& st) const noexcept
{
  return any_of_inlet(*this, [&](const inlet& inlet) {
    if(inlet.scope & port::scope_t::local)
    {
      bool b = false;

      // TODO optimize by stopping when found
      apply_to_destination(
          inlet.address, st.exec_devices(),
          [&](ossia::net::parameter_base* addr, bool) {
        if(!b || st.in_local_scope(*addr))
          b = true;
          },
          do_nothing_for_nodes{});

      if(b)
        return true;

      if(consumes(st))
        return true;
    }
    return false;
  });
}

void graph_node::clear() noexcept
{
  for_each_inlet(*this, [](auto& port) {
    for(ossia::graph_edge* e : port.cables())
    {
      e->clear();
    }
  });
  for_each_outlet(*this, [](auto& port) {
    for(ossia::graph_edge* e : port.cables())
    {
      e->clear();
    }
  });

  for(auto outl : m_outlets)
  {
    delete outl;
  }
  for(auto inl : m_inlets)
  {
    delete inl;
  }
  m_inlets.clear();
  m_outlets.clear();
}

void graph_node::request(const token_request& req) noexcept
{
  /*
  if(std::abs(req.date - req.prev_date) > 1000)
  {
    std::cerr << typeid(this).name() << " " << req.prev_date << " => " <<
  req.date << std::endl;
  }
  */

  requested_tokens.push_back(std::move(req));
}

void graph_node::process_time(
    const ossia::token_request& req, execution_state& st) noexcept
{
  auto [s, d] = exec_state_facade{&st}.timings(req);
  this->m_processed_frames += d;
}

void graph_node::all_notes_off() noexcept { }

nonowning_graph_node::~nonowning_graph_node()
{
  m_inlets.clear();
  m_outlets.clear();
}

void nonowning_graph_node::clear() noexcept
{
  for(auto inl : m_inlets)
  {
    for(ossia::graph_edge* e : inl->sources)
    {
      e->clear();
    }
  }
  for(auto outl : m_outlets)
  {
    for(ossia::graph_edge* e : outl->targets)
    {
      e->clear();
    }
  }
}
}
