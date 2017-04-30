#pragma once
#include <boost/container/flat_set.hpp>
#include <boost/functional/hash.hpp>
#include <ossia/editor/value/value.hpp>
#include <ossia/network/base/address.hpp>
#include <ModernMIDI/midi_message.h>
#include <eggs/variant.hpp>

#include <vector>
#include <array>
#include <deque>
namespace std
{
template<typename T>
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

template<typename T>
using set = boost::container::flat_set<T>;

using destination_t = eggs::variant<
ossia::net::address_base*,
std::string // ossia::traversal::path
>;
class graph_node;
struct graph_edge;
using node_ptr = std::shared_ptr<graph_node>;
using edge_ptr = std::shared_ptr<graph_edge>;
struct port;
struct outlet;
struct inlet;
using inlet_ptr = std::shared_ptr<inlet>;
using outlet_ptr = std::shared_ptr<outlet>;
struct base_pair { graph_edge* edge{}; };
using inlet_pair = base_pair;
using outlet_pair = base_pair;
inline bool operator==(base_pair lhs, base_pair rhs) { return lhs.edge == rhs.edge; }

struct audio_port;
struct midi_port;
struct value_port;

using data_type = eggs::variant<audio_port, midi_port, value_port>;
}
