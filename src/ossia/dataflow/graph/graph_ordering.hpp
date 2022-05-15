#pragma once
#include <ossia/audio/audio_parameter.hpp>
#include <ossia/dataflow/data_copy.hpp>
#include <ossia/dataflow/graph_edge.hpp>
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/dataflow/port.hpp>
#include <ossia/detail/logger.hpp>
#include <ossia/network/common/path.hpp>

#include <boost/range/algorithm/lexicographical_compare.hpp>

namespace ossia
{

struct node_sorter
{
  const std::vector<graph_node*>& node_order;
  const execution_state& st;

  bool compare(const graph_node* lhs, const graph_node* rhs) const
  {
    for (std::size_t i = 0, N = node_order.size(); i < N; i++)
    {
      if (node_order[i] == lhs)
        return true;
      else if (node_order[i] == rhs)
        return false;
      else
        continue;
    }
    throw std::runtime_error("lhs and rhs have to be found");
  }

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
      return compare(lhs, rhs);

    bool l1 = lhs->has_local_inputs(st);
    bool l2 = rhs->has_local_inputs(st);

    if (l1 && !l2)
      return true;
    else if (!l1 && l2)
      return false;
    else if (l1 && l2)
      return compare(lhs, rhs);

    bool g1 = lhs->has_global_inputs();
    bool g2 = rhs->has_global_inputs();
    if (g1 && !g2)
      return true;
    else if (!g1 && g2)
      return false;
    else if (g1 && g2)
      return compare(lhs, rhs);

    return compare(lhs, rhs);
  }
};

struct init_node_visitor
{
  inlet& in;
  const graph_edge& edge;
  execution_state& e;

  static void copy(const delay_line_type& out, std::size_t pos, inlet& in)
  {
    const auto w = out.which();
    if (w.to_std_index() == in.which() && w.valid())
    {
      switch (w.index())
      {
        case delay_line_type::index_of<ossia::audio_delay_line>():
          copy_data_pos{pos}(
              *reinterpret_cast<const ossia::audio_delay_line*>(out.target()),
              in.cast<ossia::audio_port>());
          break;
        case delay_line_type::index_of<ossia::midi_delay_line>():
          copy_data_pos{pos}(
              *reinterpret_cast<const ossia::midi_delay_line*>(out.target()),
              in.cast<ossia::midi_port>());
          break;
        case delay_line_type::index_of<ossia::value_delay_line>():
          copy_data_pos{pos}(
              *reinterpret_cast<const ossia::value_delay_line*>(out.target()),
              in.cast<ossia::value_port>());
          break;
      }
    }
  }

  static void move(outlet& out, inlet& in)
  {
    const auto w = out.which();
    if (w == in.which() && w != data_type::npos)
    {
      switch (w)
      {
        case 0:
          move_data{}(
              out.cast<ossia::audio_port>(),
              in.cast<ossia::audio_port>());
          break;
        case 1:
          move_data{}(
              out.cast<ossia::midi_port>(),
              in.cast<ossia::midi_port>());
          break;
        case 2:
          move_data{}(
              out.cast<ossia::value_port>(),
              in.cast<ossia::value_port>());
          break;
      }
    }
  }
  static void copy(outlet& out, inlet& in)
  {
    if(out.cables().size() == 1 && in.cables().size() == 1 && !out.address && !in.address)
    {
      move(out, in);
    }
    else
    {
      const auto w = out.which();
      if (w == in.which() && w != data_type::npos)
      {
        switch (w)
        {
          case 0:
            copy_data{}(
                out.cast<ossia::audio_port>(),
                in.cast<ossia::audio_port>());
            break;
          case 1:
            copy_data{}(
                out.cast<ossia::midi_port>(),
                in.cast<ossia::midi_port>());
            break;
          case 2:
            copy_data{}(
                out.cast<ossia::value_port>(),
                in.cast<ossia::value_port>());
            break;
        }
      }
    }
  }

  bool operator()(immediate_glutton_connection) const
  {
    if (edge.out_node->enabled())
    {
      copy(*edge.out, in);
      return false;
    }
    else
    {
      // todo delay, etc
      return true;
    }
  }

  bool operator()(immediate_strict_connection) const
  {
    // if it's a strict connection then the other node
    // is necessarily enabled
    copy(*edge.out, in);
    return false;
  }

  bool operator()(delayed_glutton_connection& con) const
  {
    // TODO If there is data...
    // Else...
    copy(con.buffer, con.pos, in);
    con.pos++;
    return false;
  }

  bool operator()(delayed_strict_connection& con) const
  {
    copy(con.buffer, con.pos, in);
    con.pos++;
    return false;
  }

  bool operator()(dependency_connection) const
  {
    return true;
  }

  bool operator()() const
  {
    return true;
  }
};
struct init_must_copy_visitor
{
  const graph_edge& edge;

  bool operator()(const immediate_glutton_connection&) const
  {
    return !edge.out_node->enabled();
  }

  bool operator()(const immediate_strict_connection&) const
  {
    return false;
  }

  bool operator()(const delayed_glutton_connection&) const
  {
    return false;
  }

  bool operator()(const delayed_strict_connection&) const
  {
    return false;
  }

  bool operator()(const dependency_connection&) const
  {
    return true;
  }

  bool operator()() const
  {
    return true;
  }
};

struct env_writer
{
  outlet& out;
  graph_edge& edge;
  bool operator()(const immediate_glutton_connection&) const
  {
    return !edge.in_node->enabled();
  }
  bool operator()(const immediate_strict_connection& con) const
  {
    using rs = immediate_strict_connection::required_sides_t;
    switch (con.required_sides)
    {
      case rs::inbound:
      case rs::outbound:
        // TODO
        break;
      default:
      case rs::both:
        // Nothing to do : copied on "input" phase if the node is active,
        // or no need to copy if the node isn't
        break;
    }
    return false;
  }
  bool operator()(delayed_glutton_connection& con) const
  {
    // Copy to the buffer
    const auto con_w = con.buffer.which();
    const auto out_w = out.which();
    if (con_w.to_std_index() == out_w)
    {
      switch (con_w.index())
      {
        case delay_line_type::index_of<ossia::audio_delay_line>():
          copy_data{}(out.cast<ossia::audio_port>(),
              *reinterpret_cast<ossia::audio_delay_line*>(
                con.buffer.target()));
          break;
        case delay_line_type::index_of<ossia::midi_delay_line>():
          copy_data{}(out.cast<ossia::midi_port>(),
              *reinterpret_cast<ossia::midi_delay_line*>(
                con.buffer.target()));
          break;
        case delay_line_type::index_of<ossia::value_delay_line>():
          copy_data{}(out.cast<ossia::value_port>(),
              *reinterpret_cast<ossia::value_delay_line*>(
                con.buffer.target()));
          break;
      }
    }
    return false;
  }
  bool operator()(delayed_strict_connection& con) const
  {
    // Copy to the buffer
    const auto con_w = con.buffer.which();
    const auto out_w = out.which();
    if (con_w.to_std_index() == out_w)
    {
      switch (con_w.index())
      {
        case delay_line_type::index_of<ossia::audio_delay_line>():
          copy_data{}(out.cast<ossia::audio_port>(),
              *reinterpret_cast<ossia::audio_delay_line*>(
                con.buffer.target()));
          break;
        case delay_line_type::index_of<ossia::midi_delay_line>():
          copy_data{}(out.cast<ossia::midi_port>(),
              *reinterpret_cast<ossia::midi_delay_line*>(
                con.buffer.target()));
          break;
        case delay_line_type::index_of<ossia::value_delay_line>():
          copy_data{}(out.cast<ossia::value_port>(),
              *reinterpret_cast<ossia::value_delay_line*>(
                con.buffer.target()));
          break;
      }
    }
    return false;
  }
  bool operator()(const dependency_connection&) const
  {
    return false;
  }
  bool operator()() const
  {
    return false;
  }
};
}
