#pragma once
#include "ext.h"
#include "ext_obex.h"
#include "jpatcher_api.h"
#undef error
#undef post

#include <ossia-max/src/client.hpp>
#include <ossia-max/src/model.hpp>
#include <ossia-max/src/ossia-max.hpp>
#include <ossia-max/src/parameter.hpp>
#include <ossia-max/src/remote.hpp>
#include <ossia-max/src/view.hpp>
#include <ossia-max/src/ossia_object_base.hpp>
#include <ossia/network/domain/domain.hpp>

#include <ossia/editor/dataspace/dataspace_visitors.hpp>
#include <fmt/format.h>
namespace ossia
{
namespace max
{

/**
 * @brief find_peers: find object of the same class in the patcher
 * @param x : object we are looking for friend
 * @return true if we find one, false otherwise
 */
bool find_peer(t_object_base *x);

/**
 * @brief find_global_node: find node matching address with a 'device:' prefix
 * @param addr : address string
 * @return pointer to the node
 */
std::vector<ossia::net::node_base*> find_global_nodes(const std::string& addr);

/**
 * @brief get_parameter_type: return address type (relative, absolute or globale)
 * @param addr: the address to process
 * @return
 */
ossia::max::address_scope get_parameter_type(const std::string& addr);

/**
 * @brief attribute2value : convert t_atom array from attribute to vector of ossia::value
 * @param atom : array of atom
 * @param size : number of value to take
 * @return array of ossia::value
 */
std::vector<ossia::value> attribute2value(t_atom* atom, long size);

/**
 * @brief symbol2val_type Convert a t_symbol into ossia::val_type
 * @param s
 * @return ossia::val_type
 */
ossia::val_type symbol2val_type(t_symbol* s);
t_symbol* val_type2symbol(ossia::val_type t);

} // namespace max
} // namespace ossia
