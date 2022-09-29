#pragma once

#include <ossia/detail/fmt.hpp>
#include <ossia/network/dataspace/dataspace_visitors.hpp>
#include <ossia/network/domain/domain.hpp>

#include <ossia-pd/src/client.hpp>
#include <ossia-pd/src/model.hpp>
#include <ossia-pd/src/object_base.hpp>
#include <ossia-pd/src/ossia-pd.hpp>
#include <ossia-pd/src/parameter.hpp>
#include <ossia-pd/src/remote.hpp>
#include <ossia-pd/src/view.hpp>

#include <chrono>
#include <iostream>

namespace ossia::pd
{

template <typename TimeT = std::chrono::microseconds>
struct measure
{
  template <typename F, typename... Args>
  static typename TimeT::rep execution(F&& func, Args&&... args)
  {
    auto start = std::chrono::steady_clock::now();
    // std::invoke(std::forward<decltype(func)>(func), std::forward<Args>(args)...);
    std::forward<decltype(func)>(func)(std::forward<Args>(args)...);
    auto duration
        = std::chrono::duration_cast<TimeT>(std::chrono::steady_clock::now() - start);
    return duration.count();
  }
};

#pragma mark Value type conversion helper

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
    for(std::size_t i = 0; i < N; i++)
    {
      t_atom a;
      SETFLOAT(&a, vec[i]);
      data.push_back(a);
    }
  }

  void operator()(const std::vector<ossia::value>& t) const
  {
    data.reserve(data.size() + t.size());
    for(const auto& v : t)
      v.apply(*this);
  }

  void operator()() const { }
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
    if(x->m_dataout)
      outlet_bang(x->m_dataout);

    if(x->m_setout)
      outlet_bang(x->m_setout);
  }
  void operator()(int32_t i) const
  {
    t_atom a;
    SETFLOAT(&a, (t_float)i);
    if(x->m_dataout)
      outlet_float(x->m_dataout, (t_float)i);

    if(x->m_setout)
      outlet_anything(x->m_setout, ossia_pd::o_sym_set, 1, &a);
  }
  void operator()(float f) const
  {
    t_atom a;
    SETFLOAT(&a, f);
    if(x->m_dataout)
      outlet_float(x->m_dataout, (t_float)f);

    if(x->m_setout)
      outlet_anything(x->m_setout, ossia_pd::o_sym_set, 1, &a);
  }
  void operator()(bool b) const
  {
    t_atom a;
    t_float f = b ? 1. : 0.;
    SETFLOAT(&a, f);
    if(x->m_dataout)
      outlet_float(x->m_dataout, f);

    if(x->m_setout)
      outlet_anything(x->m_setout, ossia_pd::o_sym_set, 1, &a);
  }
  void operator()(const std::string& str) const
  {
    t_symbol* s = gensym(str.c_str());
    t_atom a;
    SETSYMBOL(&a, s);
    if(x->m_dataout)
      outlet_symbol(x->m_dataout, s);

    if(x->m_setout)
      outlet_anything(x->m_setout, ossia_pd::o_sym_set, 1, &a);
  }
  void operator()(char c) const
  {
    t_atom a;
    SETFLOAT(&a, (float)c);
    if(x->m_dataout)
      outlet_float(x->m_dataout, (float)c);

    if(x->m_setout)
      outlet_anything(x->m_setout, ossia_pd::o_sym_set, 1, &a);
  }

  template <std::size_t N>
  void operator()(std::array<float, N> vec) const
  {
    t_atom a[N];

    for(std::size_t i = 0; i < N; i++)
    {
      SETFLOAT(a + i, vec[i]);
    }

    if(x->m_dataout)
      outlet_list(x->m_dataout, gensym("list"), N, a);

    if(x->m_setout)
      outlet_anything(x->m_setout, ossia_pd::o_sym_set, N, a);
  }

  void operator()(const std::vector<ossia::value>& t) const
  {
    std::vector<t_atom> va;
    value2atom vm{va};

    if(t.empty())
      return;

    for(const auto& v : t)
      v.apply(vm);

    t_atom* list_ptr = !va.empty() ? va.data() : nullptr;
    if(x->m_dataout)
      outlet_list(x->m_dataout, gensym("list"), va.size(), list_ptr);

    if(x->m_setout)
      outlet_anything(x->m_setout, ossia_pd::o_sym_set, va.size(), list_ptr);
  }

  void operator()() const { pd_error(x, "%s received invalid data", x->m_name->s_name); }
};

struct domain_visitor
{
  parameter_base* x;

  template <typename T>
  void operator()(ossia::domain_base<T>& d)
  {
    if(d.min && d.max)
    {
      x->m_range_size = 2;
      SETFLOAT(x->m_range, *d.min);
      SETFLOAT(x->m_range + 1, *d.max);
    }

    if(d.min)
    {
      x->m_min_size = 1;
      SETFLOAT(x->m_min, *d.min);
    }

    if(d.max)
    {
      x->m_max_size = 1;
      SETFLOAT(x->m_max, *d.max);
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
      x->m_range_size = d.values.size() > OSSIA_PD_MAX_ATTR_SIZE ? OSSIA_PD_MAX_ATTR_SIZE
                                                                 : d.values.size();
      for(int i = 0; i < x->m_range_size; i++)
      {
        // SETSYMBOL(x->m_range+i,gensym(d.values[i].c_str()));
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
    x->m_min_size
        = d.min.size() > OSSIA_PD_MAX_ATTR_SIZE ? OSSIA_PD_MAX_ATTR_SIZE : d.min.size();
    x->m_max_size
        = d.max.size() > OSSIA_PD_MAX_ATTR_SIZE ? OSSIA_PD_MAX_ATTR_SIZE : d.max.size();

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
    std::vector<t_atom> vamin, vamax;
    value2atom minvisitor{vamin};
    value2atom maxvisitor{vamax};

    x->m_min_size
        = vamin.size() > OSSIA_PD_MAX_ATTR_SIZE ? OSSIA_PD_MAX_ATTR_SIZE : vamin.size();
    x->m_max_size
        = vamax.size() > OSSIA_PD_MAX_ATTR_SIZE ? OSSIA_PD_MAX_ATTR_SIZE : vamax.size();

    for(const auto& v : d.min)
      v.apply(minvisitor);
    for(unsigned int i = 0; i < vamin.size(); i++)
      x->m_min[i] = vamin[i];

    for(const auto& v : d.max)
      v.apply(maxvisitor);
    for(unsigned int i = 0; i < vamax.size(); i++)
      x->m_max[i] = vamax[i];

    // TODO range
    x->m_range_size = 0;
  }
  void operator()() { }
};

ossia::value atom2value(t_symbol* s, int argc, t_atom* argv);

#pragma mark Prototype

std::vector<std::string> parse_tags_symbol(t_symbol* tags_symbol);
std::string string_from_path(const std::vector<std::string>& vs);

/**
 * @brief register_quarantinized Try to register all quarantinized objects
 */
void register_quarantinized();

template <typename T>
std::vector<T*> get_objects(typename T::is_model* = nullptr)
{
  return ossia_pd::instance().models.copy();
}

template <typename T>
std::vector<T*> get_objects(typename T::is_device* = nullptr)
{
  return ossia_pd::instance().devices.copy();
}

template <typename T>
std::vector<T*> get_objects(typename T::is_client* = nullptr)
{
  return ossia_pd::instance().clients.copy();
}

template <typename T>
std::vector<T*> get_objects(typename T::is_attribute* = nullptr)
{
  return ossia_pd::instance().attributes.copy();
}

template <typename T>
std::vector<T*> get_objects(typename T::is_parameter* = nullptr)
{
  return ossia_pd::instance().parameters.copy();
}

template <typename T>
std::vector<T*> get_objects(typename T::is_remote* = nullptr)
{
  return ossia_pd::instance().remotes.copy();
}

template <typename T>
std::vector<T*> get_objects(typename T::is_view* = nullptr)
{
  return ossia_pd::instance().views.copy();
}

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
template <typename T>
T* find_parent(object_base* x, unsigned int start_level, int* level)
{
  if(start_level > x->m_patcher_hierarchy.size())
    return nullptr; // if we can't reach start level (because we reach the root
                    // canvas before the start_level) then abort

  std::vector<T*> objects = get_objects<T>();

  // first remove objects that are deeper in the patcher
  objects.erase(
      ossia::remove_if(
          objects,
          [&](T* obj) {
    return obj->m_patcher_hierarchy.size() > x->m_patcher_hierarchy.size();
          }),
      objects.end());

  // then remove the object itself
  ossia::remove_one(objects, x);

  // and sort objects by hierarchy size
  // because the first parent have potentially the same hierarchy depth
  ossia::sort(objects, [](auto o1, auto o2) {
    return o1->m_patcher_hierarchy.size() > o2->m_patcher_hierarchy.size();
  });

  for(unsigned int i = start_level; i < x->m_patcher_hierarchy.size(); i++)
  {
    // remove objects that are deeper than the expected level
    auto size = x->m_patcher_hierarchy.size() - i;
    objects.erase(
        ossia::remove_if(
            objects, [&](T* obj) { return obj->m_patcher_hierarchy.size() > size; }),
        objects.end());

    for(auto o : objects)
    {
      if(x->m_patcher_hierarchy[i] == o->m_patcher_hierarchy[0])
      {
        (*level) = i;
        return o;
      }
    }
  }
  return nullptr;
}

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
template <typename T>
static inline T* find_parent_alive(object_base* x, int start_level, int* level)
{
  T* obj = find_parent<T>(x, start_level, level);
  if(obj)
  {
    while(obj && obj->m_dead)
    {
      obj = find_parent_alive<T>(obj, 1, level);
    }
  }
  assert(!obj || !obj->m_dead);
  return obj;
}

#pragma mark template

std::string get_absolute_path(object_base* x);

/**
 * @brief find_parent_node : find first active node above
 * @param x : starting object object
 * @return active node pointer if found or nullptr
 */
const std::vector<t_matcher>& find_parent_node(object_base* x);

/**
 * @brief Find all objects [classname] in the current patcher starting at
 * specified level.
 * @param list : list in which we are looking for objecfts
 * @param classname : name of the object to search (ossia.model or ossia.view)
 * @return std::vector<t_pd*> containing pointer to t_pd struct of the
 * corresponding classname
 */
std::vector<object_base*>
find_child_to_register(object_base* x, t_gobj* start_list, t_symbol* classname);

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
std::vector<ossia::pd::t_matcher*>
make_matchers_vector(object_base* x, const ossia::net::node_base* node);

void trig_output_value(ossia::net::node_base* node);

#pragma mark Templates

template <typename T>
/**
 * @brief copy : utility function to return a copy of an object
 * @param v : object to copy
 */
auto copy(const T& v)
{
  return v;
}

template <typename T>
// self registering (when creating the object)
bool ossia_register(T* x)
{
  if(x->m_dead)
    return false; // object will be removed soon

  std::vector<t_matcher> tmp;
  std::vector<t_matcher>* matchers = &tmp;
  std::vector<ossia::net::node_base*> nodes;

  if(x->m_addr_scope == ossia::net::address_scope::global)
  {
    std::string addr = x->m_name->s_name;
    if(x->m_otype == object_class::param || x->m_otype == object_class::model)
    {
      size_t pos = 0;
      while(pos != std::string::npos && nodes.empty())
      {
        // remove the last part which should be created
        pos = addr.find_last_of('/');
        if(pos < addr.size())
        {
          addr = addr.substr(0, pos);
        }
        nodes = ossia::pd::find_global_nodes(addr + "/");
      }
      addr += '/';
    }
    else
    {
      nodes = ossia::pd::find_global_nodes(addr);
    }

    tmp.reserve(nodes.size());
    for(auto n : nodes)
    {
      tmp.emplace_back(n, (object_base*)nullptr);
    }
  }
  else
  {
    std::pair<int, ossia::pd::device*> device{};
    device.second = find_parent_alive<ossia::pd::device>(x, 0, &device.first);

    std::pair<int, ossia::pd::client*> client{};
    client.second = find_parent_alive<ossia::pd::client>(x, 0, &client.first);

    std::pair<int, ossia::pd::model*> model{};
    std::pair<int, ossia::pd::view*> view{};
    int start_level{};

    if(x->m_otype == object_class::view || x->m_otype == object_class::model)
    {
      start_level = 1;
    }

    if(x->m_addr_scope == net::address_scope::relative)
    {
      // then try to locate a parent view or model
      if(x->m_otype == object_class::view || x->m_otype == object_class::remote)
      {
        view.second = find_parent_alive<ossia::pd::view>(x, start_level, &view.first);
      }

      if(!view.second)
      {
        model.second = find_parent_alive<ossia::pd::model>(x, 0, &model.first);
      }
    }

    std::vector<std::pair<int, object_base*>> vec{device, client, model, view};
    // sort pair by ascending order : closest one first
    std::sort(vec.begin(), vec.end());

    for(auto& p : vec)
    {
      if(p.second)
      {
        matchers = &p.second->m_matchers;
        break;
      }
    }

    if(matchers == &tmp && x->m_addr_scope != ossia::net::address_scope::global)
    {
      tmp.emplace_back(
          &ossia_pd::get_default_device()->get_root_node(), (object_base*)nullptr);
    }
  }

  return x->register_node(*matchers);
}

template <typename T>
void ossia_check_and_register(T* x)
{
  auto& map = ossia_pd::instance().m_root_patcher;
  auto it = map.find(x->m_patcher_hierarchy.back());

  // register object only if root patcher have been loadbanged
  // else the patcher itself will trig a registration on loadbang
  if(it != map.end() && it->second.is_loadbanged)
    ossia_register(x);
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
  ossia_register(x);
}

template <typename T>
bool obj_is_quarantined(T* x)
{
  return x->quarantine().contains(x);
}

template <typename T>
void obj_quarantining(T* x)
{
  x->quarantine().push_back(x);
}

template <typename T>
void obj_dequarantining(T* x)
{
  x->quarantine().remove_all(x);
}

} // namespace ossia
