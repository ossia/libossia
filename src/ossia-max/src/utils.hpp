#pragma once
#include "ext.h"
#include "ext_obex.h"
#include "jpatcher_api.h"
#undef error
#undef post

#include <ossia/detail/fmt.hpp>
#include <ossia/network/dataspace/dataspace_visitors.hpp>
#include <ossia/network/domain/domain.hpp>

#include <ossia-max/src/client.hpp>
#include <ossia-max/src/model.hpp>
#include <ossia-max/src/object_base.hpp>
#include <ossia-max/src/ossia-max.hpp>
#include <ossia-max/src/parameter.hpp>
#include <ossia-max/src/remote.hpp>
#include <ossia-max/src/view.hpp>
namespace ossia
{
namespace max_binding
{

void object_namespace(object_base* x);

/**
 * @brief Find all objects [classname] in the current patcher.
 * @param patcher : patcher in which we are looking for objects
 * @param classname : name of the object to search (ossia.model or ossia.view)
 * @return std::vector<object_base*> containing pointer to object_base struct of the
 * corresponding classname
 */
std::vector<object_base*> find_children_to_register(
    t_object* object, t_object* patcher, t_symbol* classname, bool search_dev = false);

/**
 * @brief register_objects_in_patcher_recursively : iterate over all patcher's objects
 * and register them one by one recursively
 * @param root_patcher: starting patcher
 * @param caller: object that calls the function
 */
void register_children_in_patcher_recursively(
    t_object* root_patcher, object_base* caller);

/**
 * @brief get_poly_index : return the instance number if patcher is loaded by a poly~, 0
 * otherwise
 * @param caller: object that calls the function
 * @return poly~ instance number or 0
 */
long get_poly_index(t_object* patcher);

/**
 * @brief Convenient method to easily get the patcher where a box is
 */
t_object* get_patcher(t_object* object);

/**
 * @brief update_path_recursively: update children path recursively
 * @param patcher
 */
void update_path_recursively(t_object* patcher);

/**
 */
std::vector<std::string> parse_tags_symbol(t_symbol** tags_symbol, long size);

/**
 * @brief get_all_devices: iterate over all ossia.device and ossia.client to get their
 * generic_device
 * @return a list of all known generic_devices*
 */
std::vector<ossia::net::generic_device*> get_all_devices();

/**
 * @brief get_parameter_type: return address type (relative, absolute or global)
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
 * @brief symbol2bounding_mode convert t_symbol* to corresponding ossia::bounding_mode
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
std::vector<ossia::max_binding::matcher*>
make_matchers_vector(object_base* x, const ossia::net::node_base* node);

ossia::value atom2value(t_symbol* s, int argc, t_atom* argv);

// return a list of nodes priority from root to node
std::vector<ossia::net::priority> get_priority_list(ossia::net::node_base* node);

struct node_priority
{
  matcher* obj{};
  std::vector<ossia::net::priority> priorities;

  friend std::ostream& operator<<(std::ostream& output, const node_priority& n)
  {
    output << object_classname(n.obj->get_owner())->s_name << "\t"
           << n.obj->get_node()->get_name() << "\t";
    for(auto p : n.priorities)
      output << p << " ";
    return output;
  }
};

/**
 * @brief fire_values_by_priority: sort graph by priority ond output values
 * @param priority_graph: vector of node_priority to sort and fire
 * @param only_default: only output default values
 */
void fire_values_by_priority(
    std::vector<node_priority>& priority_graph, bool only_default);

/**
 * @brief fire_all_values_by_priority: output every parameter's values by priority order
 * @param patcher: the patcher in which to look for object
 * @param only_default: output only default value
 */
void output_all_values(t_object* patcher, bool only_default);

template <typename T>
void address_mess_cb(T* x, t_symbol* address)
{
  x->save_children_state();
  x->m_name = address;
  x->m_addr_scope = ossia::net::get_address_scope(x->m_name->s_name);
  x->update_path();
  x->m_node_selection.clear();
  x->m_matchers.clear();
  x->do_registration();

  switch(x->m_otype)
  {
    case object_class::view:
    case object_class::model:
      register_children_in_patcher_recursively(x->m_patcher, x);
      output_all_values(x->m_patcher, false);
      break;
    case object_class::param:
      for(const auto& m : x->m_matchers)
      {
        auto param = m->get_node()->get_parameter();
        param->push_value(param->value());
      }
      break;
    case object_class::remote:
      for(const auto& m : x->m_matchers)
      {
        auto param = m->get_node()->get_parameter();
        auto val = param->value();
        m->output_value(val);
      }
      break;
  }
}

struct domain_visitor
{
  parameter_base* x;

  template <typename T>
  void operator()(ossia::domain_base<T>& d)
  {
    if(d.min && d.max)
    {
      x->m_range_size = 2;
      A_SETFLOAT(x->m_range, *d.min);
      A_SETFLOAT(x->m_range + 1, *d.max);
    }

    if(d.min)
    {
      x->m_min_size = 1;
      A_SETFLOAT(x->m_min, *d.min);
    }

    if(d.max)
    {
      x->m_max_size = 1;
      A_SETFLOAT(x->m_max, *d.max);
    }
  }
  void operator()(ossia::domain_base<bool>& d)
  {
    // nothing to do
  }
  void operator()(ossia::domain_base<impulse>& d)
  {
    // nothing to do
  }
  void operator()(ossia::domain_base<std::string> d)
  {
    if(!d.values.empty())
    {
      x->m_range_size = d.values.size() > OSSIA_MAX_MAX_ATTR_SIZE
                            ? OSSIA_MAX_MAX_ATTR_SIZE
                            : d.values.size();
      int i = 0;
      for(const auto& s : d.values)
      {
        auto sym = gensym(s.c_str());
        A_SETSYM(x->m_range + i, sym);
        i++;
        if(i == x->m_range_size)
          break;
      }
    }
  }
  void operator()(ossia::domain_base<ossia::value> d)
  {
    // TODO
    if(d.min)
    {
    }
    if(d.max)
    {
    }
    if(!d.values.empty())
    {
    }
  }

  template <std::size_t N>
  void operator()(ossia::vecf_domain<N>& d)
  {
    x->m_min_size = d.min.size() > OSSIA_MAX_MAX_ATTR_SIZE ? OSSIA_MAX_MAX_ATTR_SIZE
                                                           : d.min.size();
    x->m_max_size = d.max.size() > OSSIA_MAX_MAX_ATTR_SIZE ? OSSIA_MAX_MAX_ATTR_SIZE
                                                           : d.max.size();

    for(int i = 0; i < x->m_max_size; i++)
      atom_setfloat(&x->m_max[i], *d.max[i]);

    for(int i = 0; i < x->m_min_size; i++)
      atom_setfloat(&x->m_min[i], *d.min[i]);

    x->m_range_size = 0;
    if(x->m_min_size == x->m_max_size && x->m_min_size > 1)
    {
      bool flag = true;
      for(int i = 1; i < x->m_min_size && flag; i++)
      {
        flag |= *d.min[0] == *d.min[i];
        flag |= *d.max[0] == *d.max[i];
        if(!flag)
          break;
      }
      if(flag)
      {
        x->m_range_size = 2;
        atom_setfloat(&x->m_range[0], *d.min[0]);
        atom_setfloat(&x->m_range[1], *d.max[0]);
      }
    }
  }

  void operator()(ossia::vector_domain& d)
  {
    x->m_min_size = d.min.size() > OSSIA_MAX_MAX_ATTR_SIZE ? OSSIA_MAX_MAX_ATTR_SIZE
                                                           : d.min.size();
    x->m_max_size = d.max.size() > OSSIA_MAX_MAX_ATTR_SIZE ? OSSIA_MAX_MAX_ATTR_SIZE
                                                           : d.max.size();

    std::vector<t_atom> vamin, vamax;
    value2atom minvisitor{vamin}, maxvisitor{vamax};
    for(const auto& v : d.min)
      v.apply(minvisitor);
    for(int i = 0; i < vamin.size(); i++)
      x->m_min[i] = vamin[i];

    for(const auto& v : d.max)
      v.apply(maxvisitor);
    for(int i = 0; i < vamax.size(); i++)
      x->m_max[i] = vamax[i];

    // TODO range
    x->m_range_size = 0;
  }
  void operator()() { }
};

} // namespace max
} // namespace ossia
