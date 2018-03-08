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

void trig_output_value(ossia::net::node_base* node);

// put templates after prototype so we can use them

template<typename T>
std::vector<T*> get_objects(typename T::is_model* = nullptr)
{
  return ossia_max::instance().models.copy();
}

template<typename T>
std::vector<T*> get_objects(typename T::is_device* = nullptr)
{
  return ossia_max::instance().devices.copy();
}

template<typename T>
std::vector<T*> get_objects(typename T::is_client* = nullptr)
{
  return ossia_max::instance().clients.copy();
}

template<typename T>
std::vector<T*> get_objects(typename T::is_attribute* = nullptr)
{
  return ossia_max::instance().attributes.copy();
}

template<typename T>
std::vector<T*> get_objects(typename T::is_parameter* = nullptr)
{
  return ossia_max::instance().parameters.copy();
}

template<typename T>
std::vector<T*> get_objects(typename T::is_remote* = nullptr)
{
  return ossia_max::instance().remotes.copy();
}

template<typename T>
std::vector<T*> get_objects(typename T::is_view* = nullptr)
{
  return ossia_max::instance().views.copy();
}

/**
 * @brief             Find the first box of classname beside or above (in a
 * parent patcher) context.
 * @details           The function iterate all objects at the same level or
 * above x and return the first instance of classname found.
 * @param object      The Max object instance around which to search.
 * @param classname   The class name of the box object we are looking for.
 * @param start_level Level above current object where to start. 0 for current
 * patcher, 1 start searching in parent canvas.
 * @param level       Return level of the found object
 * @return The instance of the parent box if exists. Otherwise returns nullptr.
 */
template<typename T>
T* find_parent_box(
    object_base* x, int start_level, int* level)
{
  if (start_level > x->m_patcher_hierarchy.size())
    return nullptr; // if we can't reach start level (because we reach the root
                    // canvas before the start_level) then abort

  std::vector<T*> objects = get_objects<T>();

  // first remove objects that are deeper in the patcher
  objects.erase(
        ossia::remove_if(objects, [&](T* obj){
          return obj->m_patcher_hierarchy.size() > x->m_patcher_hierarchy.size(); }),
            objects.end());

  // then remove the object itself
  ossia::remove_one(objects, x);

  // and sort objects by hierarchy size
  // because the first parent have potentially the same hierarchy depth
  ossia::sort(objects, [](auto o1, auto o2){
    return o1->m_patcher_hierarchy.size() > o2->m_patcher_hierarchy.size();});

  for (int i = start_level; i < x->m_patcher_hierarchy.size(); i++)
  {
    // remove objects that are deeper than the expected level
    auto size = x->m_patcher_hierarchy.size() - i;
    objects.erase(
          ossia::remove_if(objects, [&](T* obj){
            return obj->m_patcher_hierarchy.size() > size; }),
              objects.end());

    for (auto o : objects)
    {
      if (x->m_patcher_hierarchy[i] == o->m_patcher_hierarchy[0])
      {
        return o;
      }
    }
  }
  return nullptr;
}


/**
 * @brief find_parent_box_alive
 * @details Find a parent that is not being removed soon
 * @param object      The Max object instance around which to search.
 * @param classname
 * @param start_level
 * @return
 */
template<typename T>
static inline T* find_parent_box_alive(
    object_base* object, int start_level, int* level)
{
  T* parent
      = find_parent_box<T>(object, start_level, level);

  while (parent && parent->m_dead)
  {
    parent
        = find_parent_box_alive<T>(parent, 1, level);
  }

  return parent;
}

template<typename T> bool
ossia_register(T*x)
{
  if (!x->m_name)
    return false;

  std::cout << "registering " << x << " " << x->m_name->s_name << std::endl;

  std::vector<std::shared_ptr<t_matcher>> tmp;
  std::vector<std::shared_ptr<t_matcher>>* matchers = &tmp;

  if (x->m_addr_scope == ossia::net::address_scope::global)
  {
    auto nodes = ossia::max::find_global_nodes(x->m_name->s_name);

    tmp.reserve(nodes.size());
    for (auto n : nodes)
    {
      tmp.emplace_back(std::make_shared<t_matcher>(n, (object_base*)nullptr));
    }
  }
  else
  {
    int l;
    ossia::max::device* device =
        find_parent_box_alive<ossia::max::device>(x, 0, &l);
    ossia::max::client* client =
        find_parent_box_alive<ossia::max::client>(x, 0, &l);

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
        view = find_parent_box_alive<ossia::max::view>(
              x, start_level, &view_level);
      }

      if (!view)
      {
        model = find_parent_box_alive<ossia::max::model>(
              x, start_level, &model_level);
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
      tmp.emplace_back(std::make_shared<t_matcher>(&ossia_max::get_default_device()->get_root_node(), (object_base*)nullptr));
    }
  }

  return x->register_node(*matchers);
}


template<typename T>
void ossia_check_and_register(T* x)
{
  x->get_hierarchy();
  auto& map = ossia_max::instance().root_patcher;
  auto it = map.find(x->m_patcher_hierarchy.back());

  // register object only if root patcher have been loadbanged
  // else the patcher itself will trig a registration on loadbang
  if(it != map.end() && it->second)
    ossia_register(x);
}

template <typename T>
void address_mess_cb(T* x, t_symbol* address)
{
  x->m_name = address;
  x->m_addr_scope = ossia::net::get_address_scope(x->m_name->s_name);
  x->update_path();
  x->unregister();
  ossia_register(x);
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
      x->m_range_size = d.values.size() > OSSIA_MAX_MAX_ATTR_SIZE ? OSSIA_MAX_MAX_ATTR_SIZE : d.values.size();
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
    x->m_min_size = d.min.size() > OSSIA_MAX_MAX_ATTR_SIZE ? OSSIA_MAX_MAX_ATTR_SIZE : d.min.size();
    x->m_max_size = d.max.size() > OSSIA_MAX_MAX_ATTR_SIZE ? OSSIA_MAX_MAX_ATTR_SIZE : d.max.size();

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
        if (!flag)
          break;
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
    x->m_min_size = d.min.size() > OSSIA_MAX_MAX_ATTR_SIZE ? OSSIA_MAX_MAX_ATTR_SIZE : d.min.size();
    x->m_max_size = d.max.size() > OSSIA_MAX_MAX_ATTR_SIZE ? OSSIA_MAX_MAX_ATTR_SIZE : d.max.size();

    std::vector<t_atom> vamin, vamax;
    value2atom minvisitor{vamin}, maxvisitor{vamax};
    for (const auto& v : d.min)
      v.apply(minvisitor);
    for (int i=0; i < vamin.size(); i++)
      x->m_min[i] = vamin[i];

    for (const auto& v : d.max)
      v.apply(maxvisitor);
    for (int i=0; i < vamax.size(); i++)
      x->m_max[i] = vamax[i];

    // TODO range
    x->m_range_size = 0;

  }
  void operator()()
  {

  }
};

} // namespace max
} // namespace ossia
