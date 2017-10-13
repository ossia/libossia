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
#include <ossia-max/src/object_base.hpp>
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
bool find_peer(object_base *x);

/**
 * @brief find_global_node: find node matching address with a 'device:' prefix
 * @param addr : address string
 * @return pointer to the node
 */
std::vector<ossia::net::node_base*> find_global_nodes(ossia::string_view addr);

/**
 * @brief get_parameter_type: return address type (relative, absolute or globale)
 * @param addr: the address to process
 * @return
 */
ossia::net::address_scope get_address_scope(ossia::string_view addr);

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


/**
 * @brief symbol2bounding_mode convert t_symbol* to corresponging ossia::bounding_mode
 * @param t_symbol* bounding_mode
 * @return ossia::bounding_mode
 */
ossia::bounding_mode symbol2bounding_mode(t_symbol* bounding_mode);
t_symbol* bounding_mode2symbol(ossia::bounding_mode bm);

ossia::access_mode symbol2access_mode(t_symbol* access_mode);
t_symbol* access_mode2symbol(ossia::access_mode mode);

/**
 * @brief make_matchers_vector return one matcher that matches node
 * @param x
 * @param node
 * @return
 */
std::vector<ossia::max::t_matcher*> make_matchers_vector(object_base* x, const ossia::net::node_base* node);

ossia::value atom2value(t_symbol* s, int argc, t_atom* argv);

// put templates after prototype so we can use them

template <typename T>
bool max_object_register(T* x)
{
  if (x->m_dead)
    return false; // object will be removed soon

  std::vector<ossia::net::node_base*> nodes{};

  if (x->m_addr_scope == ossia::net::address_scope::global)
  {
    nodes = {ossia::max::find_global_nodes(x->m_name->s_name)};
  }
  else
  {
    int l;
    ossia::max::device* device = (ossia::max::device*)
        find_parent_box_alive(&x->m_object, gensym("ossia.device"), 0, &l);
    ossia::max::client* client = (ossia::max::client*)
        find_parent_box_alive(&x->m_object, gensym("ossia.client"), 0, &l);

    model* model = nullptr;
    view* view = nullptr;
    int view_level = 0, model_level = 0;
    int start_level = 0;

    if (std::is_same<T, ossia::max::view>::value || std::is_same<T, ossia::max::model>::value)
    {
      start_level = 1;
    }

    if (x->m_addr_scope == ossia::net::address_scope::relative)
    {
      // then try to locate a parent view or model
      if (x->m_otype == object_class::view || x->m_otype == object_class::remote)
      {
        view = (ossia::max::view*) find_parent_box_alive(
              &x->m_object, gensym("ossia.view"), start_level, &view_level);
      }

      if (!view)
      {
        model = (ossia::max::model*)find_parent_box_alive(
              &x->m_object, gensym("ossia.model"), start_level, &model_level);
      }
    }

    if (view)
      nodes = view->m_nodes;
    else if (model)
      nodes = model->m_nodes;
    else if (client)
      nodes = client->m_nodes;
    else if (device)
      nodes = device->m_nodes;
    else
      nodes = {&ossia_max::get_default_device()->get_root_node()};
  }

  return x->register_node(nodes);
}

} // namespace max
} // namespace ossia
