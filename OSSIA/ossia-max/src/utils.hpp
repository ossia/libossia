#pragma once
#include "ext.h"
#include "ext_obex.h"
#include "jpatcher_api.h"
#undef error
#undef post

#include "ossia_object_base.hpp"

namespace ossia
{
namespace max
{

/**
 * @brief find_peers: find object of the same class in the patcher
 * @param x : object we are looking for friend
 * @return true if we find one, false otherwise
 */
bool find_peers(t_object_base *x);

/**
 * @brief find_global_node: find node matching address with a 'device:' prefix
 * @param addr : address string
 * @return pointer to the node
 */
ossia::net::node_base* find_global_node(const std::string& addr);

} // namespace max
} // namespace ossia
