#pragma once

#include <ossia-pd/src/client.hpp>
#include <ossia-pd/src/model.hpp>
#include <ossia-pd/src/ossia-pd.hpp>
#include <ossia-pd/src/parameter.hpp>
#include <ossia-pd/src/remote.hpp>
#include <ossia-pd/src/view.hpp>
#include <ossia-pd/src/object_base.hpp>
#include <ossia/network/domain/domain.hpp>

#include <ossia/network/dataspace/dataspace_visitors.hpp>
#include <fmt/format.h>

namespace ossia
{
namespace pd
{

#pragma mark Value type convertion helper

struct value2atom
{
  std::vector<t_atom>& data;
  void operator()(impulse) const
  {
    t_atom a;
    SETSYMBOL(&a, gensym("bang"));
    data.push_back(a);
  }

  void operator()(int32_t i) const
  {
    t_atom a;
    SETFLOAT(&a, (t_float)i);
    data.push_back(a);
  }

  void operator()(float f) const
  {
    t_atom a;
    SETFLOAT(&a, f);
    data.push_back(a);
  }

  void operator()(bool b) const
  {
    t_atom a;
    t_float f = b ? 1. : 0.;
    SETFLOAT(&a, f);
    data.push_back(a);
  }

  void operator()(const std::string& str) const
  {
    t_symbol* s = gensym(str.c_str());
    t_atom a;
    SETSYMBOL(&a, s);
    data.push_back(a);
  }

  void operator()(char c) const
  {
    t_atom a;
    SETFLOAT(&a, (float)c);
    data.push_back(a);
  }

  template <std::size_t N>
  void operator()(std::array<float, N> vec) const
  {
    data.reserve(data.size() + N);
    for (std::size_t i = 0; i < N; i++)
    {
      t_atom a;
      SETFLOAT(&a, vec[i]);
      data.push_back(a);
    }
  }

  void operator()(const std::vector<ossia::value>& t) const
  {
    data.reserve(data.size() + t.size());
    for (const auto& v : t)
      v.apply(*this);
  }

  void operator()() const
  {
  }
};

template <typename T>
struct value_visitor
{
  T* x;

  void operator()(impulse) const
  {
    // TODO how to deal with impulse ? in Pd bang object doesn't have [set ...(
    // message
    // and sending a bang to the bang object connected to the inlet of the
    // sender will lead to stack overflow...
    if (x->m_dataout)
      outlet_bang(x->m_dataout);

    if (x->m_setout)
      outlet_bang(x->m_setout);
  }
  void operator()(int32_t i) const
  {
    t_atom a;
    SETFLOAT(&a, (t_float)i);
    if (x->m_dataout)
      outlet_float(x->m_dataout, (t_float)i);

    if (x->m_setout)
      outlet_anything(x->m_setout, ossia_pd::instance().sym_set, 1, &a);
  }
  void operator()(float f) const
  {
    t_atom a;
    SETFLOAT(&a, f);
    if (x->m_dataout)
      outlet_float(x->m_dataout, (t_float)f);

    if (x->m_setout)
      outlet_anything(x->m_setout, ossia_pd::instance().sym_set, 1, &a);
  }
  void operator()(bool b) const
  {
    t_atom a;
    t_float f = b ? 1. : 0.;
    SETFLOAT(&a, f);
    if (x->m_dataout)
      outlet_float(x->m_dataout, f);

    if (x->m_setout)
      outlet_anything(x->m_setout, ossia_pd::instance().sym_set, 1, &a);
  }
  void operator()(const std::string& str) const
  {
    t_symbol* s = gensym(str.c_str());
    t_atom a;
    SETSYMBOL(&a, s);
    if (x->m_dataout)
      outlet_symbol(x->m_dataout, s);

    if (x->m_setout)
      outlet_anything(x->m_setout, ossia_pd::instance().sym_set, 1, &a);
  }
  void operator()(char c) const
  {
    t_atom a;
    SETFLOAT(&a, (float)c);
    if (x->m_dataout)
      outlet_float(x->m_dataout, (float)c);

    if (x->m_setout)
      outlet_anything(x->m_setout, ossia_pd::instance().sym_set, 1, &a);
  }

  template <std::size_t N>
  void operator()(std::array<float, N> vec) const
  {
    t_atom a[N];

    for (std::size_t i = 0; i < N; i++)
    {
      SETFLOAT(a + i, vec[i]);
    }

    if (x->m_dataout)
      outlet_list(x->m_dataout, gensym("list"), N, a);

    if (x->m_setout)
      outlet_anything(x->m_setout, ossia_pd::instance().sym_set, N, a);
  }

  void operator()(const std::vector<ossia::value>& t) const
  {
    std::vector<t_atom> va;
    value2atom vm{va};
    for (const auto& v : t)
      v.apply(vm);

    t_atom* list_ptr = !va.empty() ? va.data() : nullptr;
    if (x->m_dataout)
      outlet_list(x->m_dataout, gensym("list"), va.size(), list_ptr);

    if (x->m_setout)
      outlet_anything(x->m_setout, ossia_pd::instance().sym_set, va.size(), list_ptr);
  }

  void operator()() const
  {
    pd_error(x, "%s received invalid data", x->m_name->s_name);
  }
};

struct domain_visitor {
  parameter_base* x;

  template<typename T>
  void operator()(ossia::domain_base<T>& d)
  {
    if(d.min && d.max) {
      x->m_range_size = 2;
      SETFLOAT(x->m_range, *d.min);
      SETFLOAT(x->m_range+1, *d.max);
    }

    if (d.min) {
      x->m_min_size = 1;
      SETFLOAT(x->m_min, *d.min);
    }

    if (d.max) {
      x->m_max_size = 1;
      SETFLOAT(x->m_max, *d.max);
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
      x->m_range_size = d.values.size() > OSSIA_PD_MAX_ATTR_SIZE ? OSSIA_PD_MAX_ATTR_SIZE : d.values.size();
      for (int i = 0; i < x->m_range_size; i++)
      {
        // SETSYMBOL(x->m_range+i,gensym(d.values[i].c_str()));
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
    x->m_min_size = d.min.size() > OSSIA_PD_MAX_ATTR_SIZE ? OSSIA_PD_MAX_ATTR_SIZE : d.min.size();
    x->m_max_size = d.max.size() > OSSIA_PD_MAX_ATTR_SIZE ? OSSIA_PD_MAX_ATTR_SIZE : d.max.size();

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
    x->m_min_size = d.min.size() > OSSIA_PD_MAX_ATTR_SIZE ? OSSIA_PD_MAX_ATTR_SIZE : d.min.size();
    x->m_max_size = d.max.size() > OSSIA_PD_MAX_ATTR_SIZE ? OSSIA_PD_MAX_ATTR_SIZE : d.max.size();

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

ossia::value atom2value(t_symbol* s, int argc, t_atom* argv);

#pragma mark Prototype

std::vector<std::string> parse_tags_symbol(t_symbol* tags_symbol);
std::string string_from_path(const std::vector<std::string>& vs, fmt::MemoryWriter& fullpath);

/**
 * @brief register_quarantinized Try to register all quarantinized objects
 */
void register_quarantinized();

/**
 * @fn                static t_class* find_parent(t_eobj* x, t_symbol*
 * classname)
 * @brief             Find the first instance of classname beside or above (in
 * a parent patcher) context.
 * @details           The function iterate all objects at the same level or
 * above x and return the first instance of classname found.
 * @param x           The object around which to search.
 * @param classname   The name of the object object we are looking for.
 * @param start_level Level above current object where to start. 0 for current
 * patcher, 1 start searching in parent canvas.
 * @param level       Return level of the found object
 * @return The instance of the found object.
 */
object_base* find_parent(t_eobj* x, ossia::string_view classname, int start_level, int* level);

/**
 * @brief replace_brackets Replace '<' ans '>' with '{' and '}'
 * @return the processed string
 */
std::string replace_brackets(const string_view);

/**
 * @brief find_parent_alive
 * @details Find a parent that is not being removed soon
 * @param x
 * @param classname
 * @param start_level
 * @return
 */
static inline object_base* find_parent_alive(
    t_eobj* x, ossia::string_view classname, int start_level, int* level)
{
  object_base* obj = find_parent(x, classname, start_level, level);
  if (obj)
  {
    while (obj && obj->m_dead)
    {
      obj = find_parent_alive(&obj->m_obj, classname, 1, level);
    }
  }
  assert(!obj || !obj->m_dead);
  return obj;
}

#pragma mark template

/**
 * @brief get_absolute_path
 * @param
 *
 *
 * @return std::string with full path to object from root device in an OSC
 * style (with '/')
 */
/*
template<typename T>
std::string get_absolute_path(T* x, typename T::is_model* = nullptr)
{
  fmt::MemoryWriter fullpath;
  std::vector<std::string> vs;

  vs.push_back(x->m_name->s_name);

  model* m = nullptr;
  int model_level = 0;

  int start_level = 0;
  if (std::is_same<T, model>::value)
    start_level = 1;

  m = (model*)find_parent_alive(
      &x->m_obj, "ossia.model", start_level, &model_level);
  model* tmp = nullptr;

  while (m)
  {
    vs.push_back(m->m_name->s_name);
    tmp = m;
    m = (model*)find_parent_alive(
        &tmp->m_obj, "ossia.model", 1, &model_level);
  }

  t_eobj* obj = tmp ? &tmp->m_obj : &x->m_obj;

  int device_level = 0;
  int client_level = 0;

  // FIXme TODO use get root device instead
  auto device = (ossia::pd::device*)find_parent(obj, "ossia.device", 0, &device_level);
  auto client = (ossia::pd::client*)find_parent(obj, "ossia.client", 0, &client_level);

  if (client)
    fullpath << client->m_name->s_name << ":";
  if (device)
    fullpath << device->m_name->s_name << ":";
  else
    fullpath << ossia_pd::instance().get_default_device()->get_name() << ":";

  return string_from_path(vs, fullpath);
}
*/

std::string get_absolute_path(object_base* x);

/**
 * @brief find_parent_node : find first active node above
 * @param x : starting object object
 * @return active node pointer if found or nullptr
 */
std::vector<ossia::net::node_base*> find_parent_node(object_base* x);

/**
 * @brief Find all objects [classname] in the current patcher starting at
 * specified level.
 * @param list : list in which we are looking for objecfts
 * @param classname : name of the object to search (ossia.model or ossia.view)
 * @return std::vector<t_pd*> containing pointer to t_pd struct of the
 * corresponding classname
 */
std::vector<object_base*> find_child_to_register(
    object_base* x, t_gobj* start_list, ossia::string_view classname);

/**
 * @brief find_peer: iterate through patcher's object list to find a peer
 * @param x
 * @return true if a peer have been found, false otherwise
 */
bool find_peer(object_base* x);

/**
 * @brief find_global_nodes: find nodes matching address with a 'device:' prefix
 * @param addr : address string
 * @return vector of pointers to matching nodes
 */
std::vector<ossia::net::node_base*> find_global_nodes(ossia::string_view addr);

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
std::vector<ossia::pd::t_matcher*> make_matchers_vector(object_base* x, const ossia::net::node_base* node);

#pragma mark Templates

template<typename T>
/**
 * @brief copy : utility function to return a copy of an object
 * @param v : object to copy
 */
auto copy(const T& v) { return v; }

template<typename T>
// self registering (when creating the object)
bool obj_register(T* x)
{
  if (x->m_dead)
    return false; // object will be removed soon

  auto node = find_parent_node(x);

  return x->register_node(node);
}

template <typename T>
void address_mess_cb(T* x, t_symbol* address)
{
  // TODO maybe instead use a temporary local char array.
  std::string name = replace_brackets(address->s_name);
  x->m_name = gensym(name.c_str());
  x->m_addr_scope = ossia::net::get_address_scope(x->m_name->s_name);
  x->update_path();
  x->unregister();
  obj_register(x);
}

template <typename T>
bool obj_is_quarantined(T* x)
{
  return x->quarantine().contains(x);
}

template <typename T>
void obj_quarantining(T* x)
{
  if (!obj_is_quarantined(x))
    x->quarantine().push_back(x);
}

template <typename T>
void obj_dequarantining(T* x)
{
  x->quarantine().remove_all(x);
}

} // namespace pd
} // namespace ossia
