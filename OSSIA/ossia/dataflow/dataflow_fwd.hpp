#pragma once
#include <ossia/detail/config.hpp>
#include <ossia/detail/hash.hpp>
#include <eggs/variant.hpp>
#include <utility>
#include <memory>
namespace std
{
template <typename T>
class hash<std::pair<T*, T*>>
{
public:
  std::size_t operator()(const std::pair<T*, T*>& p) const
  {
    std::size_t seed = 0;
    ossia::hash_combine(seed, p.first);
    ossia::hash_combine(seed, p.second);
    return seed;
  }
};
}

namespace ossia
{
namespace net
{
class parameter_base;
class node_base;
}
namespace traversal
{
struct path;
}
using destination_t = eggs::variant<
  ossia::net::parameter_base*,
  ossia::traversal::path,
  ossia::net::node_base*
>;
struct execution_state;
class graph_node;
struct graph_edge;
using node_ptr = std::shared_ptr<graph_node>;
using edge_ptr = std::shared_ptr<graph_edge>;
struct port;
struct outlet;
struct inlet;
using inlet_ptr = inlet*;
using outlet_ptr = outlet*;
class graph_interface;

struct audio_port;
struct midi_port;
struct value_port;

struct audio_delay_line;
struct midi_delay_line;
struct value_delay_line;

using data_type = eggs::variant<audio_port, midi_port, value_port>;
using delay_line_type = eggs::variant<audio_delay_line, midi_delay_line, value_delay_line>;
}
