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

#include <ossia/network/dataspace/dataspace_visitors.hpp>
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

std::string object_path_absolute(object_base* x);

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

  std::vector<t_matcher> tmp;
  std::vector<t_matcher>* matchers = &tmp;

  if (x->m_addr_scope == ossia::net::address_scope::global)
  {
    auto nodes = ossia::max::find_global_nodes(x->m_name->s_name);

    tmp.reserve(nodes.size());
    for (auto n : nodes)
    {
      tmp.emplace_back(n, (object_base*)nullptr);
    }
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
    {
      matchers = &view->m_matchers;
    }
    else if (model)
    {
      matchers = &model->m_matchers;
    } else if (client)
    {
      matchers = &client->m_matchers;
    } else if (device)
    {
      matchers = &device->m_matchers;
    }
    else
    {
      tmp.emplace_back(&ossia_max::get_default_device()->get_root_node(), (object_base*)nullptr);
    }
  }

  return x->register_node(*matchers);
}

template <typename T>
void address_mess_cb(T* x, t_symbol* address)
{
  x->m_name = address;
  x->m_addr_scope = ossia::net::get_address_scope(x->m_name->s_name);
  x->update_path();
  object_method(x,gensym("unselect"),0);
  x->unregister();
  max_object_register(x);
}

struct domain_visitor {
  parameter_base* x;

  template<typename T>
  void operator()(ossia::domain_base<T>& d)
  {
    if(d.min && d.max) {
      x->m_range_size = 2;
      A_SETFLOAT(x->m_range, *d.min);
      A_SETFLOAT(x->m_range+1, *d.max);
    }

    if (d.min) {
      x->m_min_size = 1;
      A_SETFLOAT(x->m_min, *d.min);
    }

    if (d.max) {
      x->m_max_size = 1;
      A_SETFLOAT(x->m_max, *d.max);
    }
  }
  void operator()(ossia::domain_base<impulse>& d)
  {
    // nothing to do
  }
  void operator()(ossia::domain_base<std::string> d)
  {
    if(!d.values.empty())
    {
      x->m_range_size = d.values.size() > 512 ? 512 : d.values.size();
      for (int i = 0; i < x->m_range_size; i++)
      {
        // SETSYM(x->m_range+i,gensym(d.values[i].c_str()));
      }
    }
  }
  void operator()(ossia::domain_base<ossia::value> d)
  {
    // TODO
    if(d.min) { }
    if(d.max) { }
    if(!d.values.empty()) { }
  }

  template<std::size_t N>
  void operator()(ossia::vecf_domain<N>& d)
  {
    x->m_min_size = d.min.size() > 512 ? 512 : d.min.size();
    x->m_max_size = d.max.size() > 512 ? 512 : d.max.size();

    for (int i=0; i<x->m_max_size; i++)
      atom_setfloat(&x->m_max[i], *d.max[i]);

    for (int i=0; i<x->m_min_size; i++)
      atom_setfloat(&x->m_min[i], *d.min[i]);

    x->m_range_size = 0;
    if ( x->m_min_size == x->m_max_size && x->m_min_size > 1 )
    {
      bool flag = true;
      for (int i=1; i < x->m_min_size && flag; i++)
      {
        flag |= *d.min[0] == *d.min[i];
        flag |= *d.max[0] == *d.max[i];
      }
      if (flag)
      {
        x->m_range_size = 2;
        atom_setfloat(&x->m_range[0], *d.min[0]);
        atom_setfloat(&x->m_range[1], *d.max[0]);
      }
    }
  }

  void operator()(ossia::vector_domain& d)
  {
    x->m_min_size = d.min.size() > 512 ? 512 : d.min.size();
    x->m_max_size = d.max.size() > 512 ? 512 : d.max.size();

    std::vector<t_atom> vamin, vamax;
    value2atom minvisitor{vamin}, maxvisitor{vamax};
    for (const auto& v : d.min)
      v.apply(minvisitor);
    for (int i=0; i<x->m_min_size; i++)
      x->m_min[i] = vamin[i];

    for (const auto& v : d.max)
      v.apply(maxvisitor);
    for (int i=0; i<x->m_max_size; i++)
      x->m_max[i] = vamax[i];

    // TODO range

  }
  void operator()()
  {

  }
};

} // namespace max
} // namespace ossia
