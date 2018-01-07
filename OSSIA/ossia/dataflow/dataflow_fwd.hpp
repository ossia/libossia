#pragma once
#include <ossia/network/value/value.hpp>
#include <ossia/network/base/parameter.hpp>
#include <ModernMIDI/midi_message.h>
#include <boost/container/flat_set.hpp>
#include <boost/functional/hash.hpp>
#include <eggs/variant.hpp>
#include <ossia/network/common/path.hpp>

#include <array>
#include <deque>
#include <vector>
namespace std
{
template <typename T>
class hash<std::pair<T*, T*>>
{
public:
  std::size_t operator()(const std::pair<T*, T*>& p) const
  {
    std::size_t seed = 0;
    boost::hash_combine(seed, p.first);
    boost::hash_combine(seed, p.second);
    return seed;
  }
};
}

namespace ossia
{

template <typename T>
using set = boost::container::flat_set<T>;

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
using inlet_ptr = std::shared_ptr<inlet>;
using outlet_ptr = std::shared_ptr<outlet>;
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
