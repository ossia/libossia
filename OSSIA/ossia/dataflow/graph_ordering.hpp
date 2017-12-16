#pragma once
#include <ossia/dataflow/audio_parameter.hpp>
#include <boost/range/algorithm/lexicographical_compare.hpp>
#include <ossia/network/common/path.hpp>
#include <ossia/detail/logger.hpp>

namespace ossia
{

struct topological_ordering
{
    const std::vector<graph_node*>& topo_order;
    bool operator()(const graph_node* lhs, const graph_node* rhs) const
    {
      // just return the nodes in their topological order
      for(std::size_t i = 0, N = topo_order.size(); i < N; i++)
      {
        if(topo_order[i] == lhs)
          return true;
        if(topo_order[i] == rhs)
          return false;
      }
      throw std::runtime_error("lhs and rhs have to be found");
    }
};

struct temporal_ordering
{
    template<typename... Args>
    temporal_ordering(Args&&...) { }
    bool operator()(const graph_node* lhs, const graph_node* rhs) const
    {
      return boost::range::lexicographical_compare(lhs->temporal_priority, rhs->temporal_priority);
    }
};

struct custom_ordering
{
    template<typename... Args>
    custom_ordering(Args&&...) { }
    bool operator()(const graph_node* lhs, const graph_node* rhs) const
    {
      return boost::range::lexicographical_compare(lhs->custom_priority, rhs->custom_priority);
    }
};

template<typename OrderingPolicy = topological_ordering>
struct node_sorter
{
    OrderingPolicy ord;
    const execution_state& st;

    bool operator()(const graph_node* lhs, const graph_node* rhs) const
    {
      // This sorting method ensures that if for instance
      // node A produces "/a" and node B consumes "/a",
      // A executes before B.
      bool c1 = lhs->has_port_inputs();
      bool c2 = rhs->has_port_inputs();
      if (c1 && !c2)
        return true;
      else if (!c1 && c2)
        return false;
      else if (c1 && c2)
        // the nodes are already sorted through the toposort
        // so we can just keep their original order
        return ord(lhs, rhs);

      bool l1 = lhs->has_local_inputs(st);
      bool l2 = rhs->has_local_inputs(st);

      if (l1 && !l2)
        return true;
      else if (!l1 && l2)
        return false;
      else if (l1 && l2)
        return ord(lhs, rhs);

      bool g1 = lhs->has_global_inputs();
      bool g2 = rhs->has_global_inputs();
      if (g1 && !g2)
        return true;
      else if (!g1 && g2)
        return false;
      else if (g1 && g2)
        return ord(lhs, rhs);

      return ord(lhs, rhs);
    }
};

template<typename Graph_T>
struct init_node_visitor
{
    inlet& in;
    graph_edge& edge;
    execution_state& e;

    void operator()(immediate_glutton_connection) const
    {
      operator()();
    }

    void operator()(immediate_strict_connection) const
    {
      Graph_T::copy(*edge.out, in);
    }

    void operator()(delayed_glutton_connection& con) const
    {
      Graph_T::copy(con.buffer, con.pos, in);
      con.pos++;
    }

    void operator()(delayed_strict_connection& con) const
    {
      Graph_T::copy(con.buffer, con.pos, in);
      con.pos++;
    }

    void operator()(reduction_connection) const
    {
      operator()();
    }
    void operator()(replacing_connection) const
    {
      operator()();
    }
    void operator()(dependency_connection) const
    {
      operator()();
    }
    void operator()() const
    {
      if (edge.out_node->enabled())
      {
        Graph_T::copy(*edge.out, in);
      }
      else
      {
        // todo delay, etc
        Graph_T::pull_from_parameter(in, e);
      }
    }
};

struct env_writer
{
    outlet& out;
    graph_edge& edge;
    execution_state& e;
    void operator()(immediate_glutton_connection) const
    {
      out.write(e);
    }
    void operator()(immediate_strict_connection) const
    {
      // Nothing to do : copied on "input" phase
    }
    void operator()(delayed_glutton_connection& con) const
    {
      // Copy to the buffer
      if (con.buffer && out.data && con.buffer.which() == out.data.which())
        eggs::variants::apply(copy_data{}, out.data, con.buffer);
    }
    void operator()(delayed_strict_connection& con) const
    {
      // Copy to the buffer
      if (con.buffer && out.data && con.buffer.which() == out.data.which())
        eggs::variants::apply(copy_data{}, out.data, con.buffer);
    }
    void operator()(reduction_connection) const
    {
    }
    void operator()(replacing_connection) const
    {
    }
    void operator()(dependency_connection) const
    {
    }
    void operator()() const
    {
    }
};

}
