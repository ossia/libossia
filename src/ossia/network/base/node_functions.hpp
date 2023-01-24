#pragma once
#include <ossia/network/base/address_scope.hpp>
#include <ossia/network/base/node.hpp>
#include <ossia/network/value/destination.hpp>

#include <functional>

/**
 * \file node_functions.hpp
 *
 * Various utilities to work with a node hierarchy.
 */
namespace ossia
{
namespace net
{

/**
 * @brief Find a node in a device.
 *
 * The address is in OSC format, e.g. /foo/bar.
 *
 * @return null if the node was not found.
 */
OSSIA_EXPORT ossia::net::node_base*
find_node(node_base& dev, std::string_view parameter_base);

/**
 * @brief Find all nodes matching a pattern in a device
 *
 * The address is in OSC pattern match format, e.g. /foo.[0-5]/bar*
 *
 * @note If the pattern is known beforehand and may be reused,
 * prefer storing a traversal::path and using the functions in path.hpp.
 */
OSSIA_EXPORT std::vector<ossia::net::node_base*>
find_nodes(node_base& dev, std::string_view pattern);

/**
 * @brief Create a node in a device.
 *
 * If the node already exists, a new instance will be created.
 * Hence there is no guarantee that the created node name is the same
 * than the one requested; the output should be checked.
 */
OSSIA_EXPORT node_base& create_node(node_base& dev, std::string_view parameter_base);

/**
 * @brief Create nodes according to a brace-expansion-like mechanism
 *
 * Only [] and { } are possible. e.g.
 * /foo/{bar,baz}.[0-9][0-9].
 *
 * \see ossia::net::is_brace_expansion
 */
OSSIA_EXPORT std::vector<ossia::net::node_base*>
create_nodes(node_base& dev, std::string_view pattern);

/**
 * @brief Find a node and create it if it does not exist.
 *
 * If the node exists, it will be returned, else a new node will be created.
 */
OSSIA_EXPORT node_base&
find_or_create_node(node_base& dev, std::string_view parameter_base);

parameter_base* find_or_create_parameter(
    node_base& node, std::string_view address, std::string_view type);
/**
 * @brief Find a parameter and create it if it does not exist.
 * @details Find a node matching the address, if it already has a parameter
 * create a new one (with name incremented by 1), if not create parameter
 * It some matching nodes does not exist, they will be created with a parameter
 * @param node
 * @param address
 * @param type of the parameter
 * @return vector of created parameters
 */
OSSIA_EXPORT std::vector<parameter_base*> find_parameter_or_create_node(
    node_base& node, std::string_view address, std::string_view type);

/**
 * @brief Calls find_node or create_node according to the value `create`
 */
OSSIA_EXPORT node_base*
find_or_create_node(node_base& dev, std::string_view parameter_base, bool create);

//! Get a valid name for a given node
void sanitize_name(std::string& name, const node_base::children_t& brethren);

/**
 * @brief Get the value associated with a destination
 */
OSSIA_EXPORT ossia::value_with_unit get_value(const ossia::destination& addr);

/**
 * @brief Send a value to a given destination
 */
OSSIA_EXPORT void
push_value(const ossia::destination& addr, const ossia::value_with_unit&);

template <typename Address>
auto create_parameter(ossia::net::node_base& root, std::string name)
{
  auto& node = ossia::net::create_node(root, std::move(name));
  auto addr = new Address(node);
  node.set_parameter(std::unique_ptr<Address>(addr));
  return addr;
}

template <typename Address>
auto find_parameter_or_create_node(ossia::net::node_base& root, std::string_view name)
{
  auto& node = ossia::net::find_or_create_node(root, std::move(name));
  if(auto p = dynamic_cast<Address*>(node.get_parameter()))
  {
    return p;
  }
  else
  {
    auto addr = new Address{node};
    node.set_parameter(std::unique_ptr<Address>(addr));
    return addr;
  }
}
OSSIA_EXPORT std::ostream& operator<<(std::ostream&, const ossia::net::parameter_base&);

OSSIA_EXPORT
void expand_ranges(std::string& str);

OSSIA_EXPORT
std::pair<std::vector<std::string>, bool> expand_address(std::string address);

/**
 * @brief list_all_children : list all child nodes recursively
 * @param node : starting point
 * @return list : vector of children sorted by name then by priority
 */
OSSIA_EXPORT
std::vector<ossia::net::node_base*>
list_all_children(ossia::net::node_base* node, unsigned int depth = 0, bool sort = true);

OSSIA_EXPORT
void
list_all_children(
        ossia::net::node_base* node,
        std::vector<ossia::net::node_base*>&out,
        unsigned int depth,
        bool sort);

/**
 * @brief Iterates all the child parameters given a base node
 */
OSSIA_EXPORT
void iterate_all_children(
    ossia::net::node_base* node,
    const std::function<void(ossia::net::parameter_base&)>&);

struct OSSIA_EXPORT fuzzysearch_result
{
  double score{};
  std::string oscname;
  ossia::net::node_base* node{};

  friend bool
  operator==(const fuzzysearch_result& lhs, const fuzzysearch_result& rhs) noexcept
  {
    return lhs.score == rhs.score && lhs.oscname == rhs.oscname && lhs.node == rhs.node;
  }
};

struct fuzzysearch_options
{
  bool case_sensitive{true};
};

OSSIA_EXPORT
void fuzzysearch(
    const std::vector<ossia::net::node_base*>& node,
    const std::vector<std::string>& patterns, std::vector<fuzzysearch_result>& results,
    fuzzysearch_options = {});

/**
 * @brief Converts a node in a map of values
 *
 * e.g. given the tree
 * ```
 * /foo/bar: 10
 * /foo/baz
 * /foo/baz/bux: [1.3, 5.]
 * ```
 *
 * then `to_map(foo)` gives something more or less like:
 *
 * map{
 *  { "bar", 10 },
 *  { "baz", map{ { "bux", vector{1.3, 5} } } }
 * }
 *
 */
OSSIA_EXPORT
ossia::value_map_type to_map(const ossia::net::node_base& n) noexcept;

}
}
