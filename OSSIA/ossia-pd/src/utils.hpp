#pragma once

#include <ossia-pd/src/client.hpp>
#include <ossia-pd/src/model.hpp>
#include <ossia-pd/src/ossia-pd.hpp>
#include <ossia-pd/src/parameter.hpp>
#include <ossia-pd/src/remote.hpp>
#include <ossia-pd/src/view.hpp>
#include <ossia-pd/src/ossia_obj_base.hpp>

#include <ossia/editor/dataspace/dataspace_visitors.hpp>
#include <fmt/format.h>

namespace ossia
{
namespace pd
{

std::vector<std::string> parse_tags_symbol(t_symbol* tags_symbol);
std::string string_from_path(const std::vector<std::string>& vs, fmt::MemoryWriter& fullpath);

/**
 * @brief register_quarantinized Try to register all quarantinized objects
 */
void register_quarantinized();

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
    outlet_bang(x->x_dataout);
    if (x->x_setout)
      outlet_bang(x->x_setout);
  }
  void operator()(int32_t i) const
  {
    t_atom a;
    SETFLOAT(&a, (t_float)i);
    outlet_float(x->x_dataout, (t_float)i);
    if (x->x_setout)
      outlet_anything(x->x_setout, gensym("set"), 1, &a);
  }
  void operator()(float f) const
  {
    t_atom a;
    SETFLOAT(&a, f);
    outlet_float(x->x_dataout, (t_float)f);
    if (x->x_setout)
      outlet_anything(x->x_setout, gensym("set"), 1, &a);
  }
  void operator()(bool b) const
  {
    t_atom a;
    t_float f = b ? 1. : 0.;
    SETFLOAT(&a, f);
    outlet_float(x->x_dataout, f);
    if (x->x_setout)
      outlet_anything(x->x_setout, gensym("set"), 1, &a);
  }
  void operator()(const std::string& str) const
  {
    t_symbol* s = gensym(str.c_str());
    t_atom a;
    SETSYMBOL(&a, s);
    outlet_symbol(x->x_dataout, s);
    if (x->x_setout)
      outlet_anything(x->x_setout, gensym("set"), 1, &a);
  }
  void operator()(char c) const
  {
    t_atom a;
    SETFLOAT(&a, (float)c);
    outlet_float(x->x_dataout, (float)c);
    if (x->x_setout)
      outlet_anything(x->x_setout, gensym("set"), 1, &a);
  }

  template <std::size_t N>
  void operator()(std::array<float, N> vec) const
  {
    t_atom a[N];

    for (std::size_t i = 0; i < N; i++)
    {
      SETFLOAT(a + i, vec[i]);
    }

    outlet_list(x->x_dataout, gensym("list"), N, a);
    if (x->x_setout)
      outlet_anything(x->x_setout, gensym("set"), N, a);
  }

  void operator()(const std::vector<ossia::value>& t) const
  {
    std::vector<t_atom> va;
    value2atom vm{va};
    for (const auto& v : t)
      v.apply(vm);

    t_atom* list_ptr = !va.empty() ? va.data() : nullptr;
    outlet_list(x->x_dataout, gensym("list"), va.size(), list_ptr);
    if (x->x_setout)
      outlet_anything(x->x_setout, gensym("set"), va.size(), list_ptr);
  }

  void operator()() const
  {
    pd_error(x, "%s receive an invalid data", x->x_name->s_name);
  }
};

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
t_obj_base* find_parent(t_eobj* x, std::string classname, int start_level, int* level);

/**
 * @brief find_parent_alive
 * @details Find a parent that is not being removed soon
 * @param x
 * @param classname
 * @param start_level
 * @return
 */
static t_obj_base* find_parent_alive(
    t_eobj* x, std::string classname, int start_level, int* level)
{
  t_obj_base* obj = find_parent(x, classname, start_level, level);
  if (obj)
  {
    while (obj && obj->x_dead)
    {
      obj = find_parent_alive(&obj->x_obj, classname, 1, level);
    }
  }
  return obj;
}

/**
 * @brief get_absolute_path
 * @param
 *
 *
 * @return std::string with full path to object from root device in an OSC
 * style (with '/')
 */

template<typename T>
std::string get_absolute_path(T* x, typename T::is_model* = nullptr)
{
  fmt::MemoryWriter fullpath;
  std::vector<std::string> vs;

  t_model* model = nullptr;
  int model_level = 0;

  int start_level = 0;
  if (std::is_same<T, t_model>::value)
    start_level = 1;

  model = (t_model*)find_parent_alive(
      &x->x_obj, "ossia.model", start_level, &model_level);
  t_model* tmp = nullptr;

  while (model)
  {
    vs.push_back(model->x_name->s_name);
    tmp = model;
    model = (t_model*)find_parent_alive(
        &tmp->x_obj, "ossia.model", 1, &model_level);
  }

  t_eobj* obj = tmp ? &tmp->x_obj : &x->x_obj;

  int device_level = 0;
  int client_level = 0;

  // FIXme TODO use get root device instead
  auto device = (t_device*)find_parent(obj, "ossia.device", 0, &device_level);
  auto client = (t_client*)find_parent(obj, "ossia.client", 0, &client_level);

  if (client)
    fullpath << client->x_name->s_name << ":";
  if (device)
    fullpath << device->x_name->s_name << ":";
  else
    fullpath << ossia_pd::instance().get_default_device()->get_name() << ":";

  return string_from_path(vs, fullpath);
}

template<typename T>
std::string get_absolute_path(T* x, typename T::is_view* = nullptr)
{
  fmt::MemoryWriter fullpath;
  std::vector<std::string> vs;

  t_view* view = nullptr;
  int view_level = 0;

  int start_level = 0;
  if (std::is_same<T, t_view>::value)
    start_level = 1;

  view =  (t_view*)find_parent_alive(
      &x->x_obj, "ossia.view", start_level, &view_level);
  t_view* tmp = nullptr;

  while (view)
  {
    vs.push_back(view->x_name->s_name);
    tmp = view;
    view
        = (t_view*) find_parent_alive(&tmp->x_obj, "ossia.view", 1, &view_level);
  }

  t_eobj* obj = tmp ? &tmp->x_obj : &x->x_obj;


  int device_level = 0;
  int client_level = 0;

  // FIXme TODO use get root device instead
  auto device = (t_device*)find_parent(obj, "ossia.device", 0, &device_level);
  auto client = (t_client*)find_parent(obj, "ossia.client", 0, &client_level);

  if (client)
    fullpath << client->x_name->s_name << ":";
  if (device)
    fullpath << device->x_name->s_name << ":";
  else
    fullpath << ossia_pd::instance().get_default_device()->get_name() << ":";

  return string_from_path(vs, fullpath);
}

/**
 * @brief find_parent_node : find first active node above
 * @param x : starting object object
 * @return active node pointer if found or nullptr
 */
ossia::net::node_base* find_parent_node(t_obj_base* x);

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
  if (x->x_node)
    return true; // already registered
  if (x->x_dead)
    return false; // object will be removed soon

  auto node = find_parent_node(x);

  return x->register_node(node);
}

/*
template <typename T>
extern void obj_bang(T* x);
*/

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

template <typename T>
void obj_dump(T* x)
{
  t_atom a;
  std::string fullpath;
  if (x->x_otype == Type::remote)
  {
    t_remote* remote = (t_remote*) x;
    if (remote->x_matchers.size() == 1)
      x->x_node = remote->x_matchers[0].get_node();
    else x->x_node = nullptr;
  }
  if (x->x_node)
  {
    fullpath = ossia::net::address_string_from_node(*x->x_node);
    SETSYMBOL(&a, gensym(fullpath.c_str()));
    outlet_anything(x->x_dumpout, gensym("fullpath"), 1, &a);
  }
  fullpath = get_absolute_path<T>(x);
  if (fullpath.back() != '/')
    fullpath += "/";
  fullpath += x->x_name->s_name;
  SETSYMBOL(&a, gensym(fullpath.c_str()));
  outlet_anything(x->x_dumpout, gensym("pdpath"), 1, &a);

  if (x->x_node)
  {
    SETFLOAT(&a, 1.);
  }
  else
  {
    SETFLOAT(&a, 0.);
  }
  outlet_anything(x->x_dumpout, gensym("registered"), 1, &a);

  SETFLOAT(&a, obj_is_quarantined<T>(x));
  outlet_anything(x->x_dumpout, gensym("quarantined"), 1, &a);

  if (x->x_node)
  {
    ossia::net::address_base* address = x->x_node->get_address();
    if (address)
    {
      // type
      std::string type = "unknown";
      switch (address->get_value_type())
      {
        case ossia::val_type::FLOAT:
          type = "float";
          break;
        case ossia::val_type::INT:
          type = "int";
          break;
        case ossia::val_type::VEC2F:
          type = "vec2f";
          break;
        case ossia::val_type::VEC3F:
          type = "vec3f";
          break;
        case ossia::val_type::VEC4F:
          type = "vec4f";
          break;
        case ossia::val_type::IMPULSE:
          type = "impulse";
          break;
        case ossia::val_type::BOOL:
          type = "bool";
          break;
        case ossia::val_type::STRING:
          type = "string";
          break;
        case ossia::val_type::TUPLE:
          type = "tuple";
          break;
        case ossia::val_type::CHAR:
          type = "char";
          break;
        default:
          type = "unknown";
      }

      SETSYMBOL(&a, gensym(type.c_str()));
      outlet_anything(x->x_dumpout, gensym("type"), 1, &a);

      // domain
      ossia::domain domain = address->get_domain();
      SETSYMBOL(&a, gensym(domain.to_pretty_string().c_str()));
      outlet_anything(x->x_dumpout, gensym("domain"), 1, &a);

      // bounding mode
      std::string bounding_mode;
      switch (address->get_bounding())
      {
        case ossia::bounding_mode::FREE:
          bounding_mode = "free";
          break;
        case ossia::bounding_mode::CLIP:
          bounding_mode = "clip";
          break;
        case ossia::bounding_mode::WRAP:
          bounding_mode = "wrap";
          break;
        case ossia::bounding_mode::FOLD:
          bounding_mode = "fold";
          break;
        case ossia::bounding_mode::LOW:
          bounding_mode = "low";
          break;
        case ossia::bounding_mode::HIGH:
          bounding_mode = "high";
          break;
        default:
          bounding_mode = "unknown";
      }
      SETSYMBOL(&a, gensym(bounding_mode.c_str()));
      outlet_anything(x->x_dumpout, gensym("bounding_mode"), 1, &a);

      // access mode
      std::string access_mode;
      switch (address->get_access())
      {
        case ossia::access_mode::BI:
          access_mode = "bi";
          break;
        case ossia::access_mode::GET:
          access_mode = "get";
          break;
        case ossia::access_mode::SET:
          access_mode = "set";
          break;
        default:
          access_mode = "unknown";
      }
      SETSYMBOL(&a, gensym(access_mode.c_str()));
      outlet_anything(x->x_dumpout, gensym("access_mode"), 1, &a);

      // repetition filter
      bool rep = address->get_repetition_filter();
      SETFLOAT(&a, rep);
      outlet_anything(x->x_dumpout, gensym("repetition_filter"), 1, &a);

      // unit
      std::string pretty_unit
          = ossia::get_pretty_unit_text(address->get_unit());
      SETSYMBOL(&a, gensym(pretty_unit.c_str()));
      outlet_anything(x->x_dumpout, gensym("unit"), 1, &a);
    }

    // description
    auto description = ossia::net::get_description(*(x->x_node));
    if (description)
    {
      SETSYMBOL(&a, gensym((*description).c_str()));
      outlet_anything(x->x_dumpout, gensym("description"), 1, &a);
    }
    else
      outlet_anything(x->x_dumpout, gensym("tags"), 0, nullptr);

    // tags
    auto tags = ossia::net::get_tags(*x->x_node);
    if (tags)
    {
      std::size_t N = (*tags).size();
      std::vector<t_atom> l(N);
      for (std::size_t i = 0; i < N; i++)
      {
        SETSYMBOL(&l[i], gensym((*tags)[i].c_str()));
      }
      outlet_anything(x->x_dumpout, gensym("tags"), N, l.data());
    }
    else
    {
      outlet_anything(x->x_dumpout, gensym("tags"), 0, nullptr);
    }
  }
}

/**
 * @brief Find all objects [classname] in the current patcher starting at
 * specified level.
 * @param list : list in which we are looking for objecfts
 * @param classname : name of the object to search (ossia.model or ossia.view)
 * @return std::vector<t_pd*> containing pointer to t_pd struct of the
 * corresponding classname
 */
std::vector<t_obj_base*> find_child_to_register(
    t_obj_base* x, t_gobj* start_list, const std::string& classname);

/**
 * @brief find_peer: iterate through patcher's object list to find a peer
 * @param x
 * @return true if a peer have been found, false otherwise
 */
bool find_peer(t_obj_base* x);

/**
 * @brief find_global_node: find node matching address with a 'device:' prefix
 * @param addr : address string
 * @return pointer to the node
 */
ossia::net::node_base* find_global_node(const std::string& addr);


/**
 * @brief get_address_type: return address type (relative, absolute or globale)
 * @param addr: the address to process
 * @return
 */
ossia::pd::AddrType get_address_type(const std::string& addr);

}
}
