#pragma once
#include <ossia/network/base/node.hpp>
#include <ossia/network/base/parameter.hpp>
#include <ossia/network/base/parameter_data.hpp>
namespace ossia::net
{
inline const std::string& osc_address(const ossia::net::full_parameter_data& d)
{
  return d.address;
}

inline const std::string& osc_address(const ossia::net::parameter_base& addr)
{
  return addr.get_node().osc_address();
}

/*!
 * \brief Constructs the textual address of a node : aDevice:/an/address
 */
OSSIA_EXPORT std::string address_string_from_node(const ossia::net::node_base&);
OSSIA_EXPORT std::string address_string_from_node(const ossia::net::parameter_base&);

/**
 * @brief Constructs the address of a node relative to parent
 */
OSSIA_EXPORT std::string relative_address_string_from_nodes(
    const ossia::net::node_base& node, const ossia::net::node_base& parent);

/**
 * @brief Constructs the OSC address of a node: /an/address
 */
OSSIA_EXPORT std::string osc_parameter_string(const ossia::net::parameter_base&);
OSSIA_EXPORT std::string osc_parameter_string(const ossia::net::node_base&);

/**
 * @brief Constructs the OSC address of a node with the leading device
 *
 * e.g. /aDevice/an/address
 */
OSSIA_EXPORT std::string
osc_parameter_string_with_device(const ossia::net::parameter_base&);
OSSIA_EXPORT std::string osc_parameter_string_with_device(const ossia::net::node_base&);

}
