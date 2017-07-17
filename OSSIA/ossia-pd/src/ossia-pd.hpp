#pragma once
#include <ossia/ossia.hpp>
#include <ossia/network/common/websocket_log_sink.hpp>
#include <sstream>

extern "C" {
#include <cicm_wrapper.h>
}

namespace ossia
{
namespace pd
{

extern "C" void setup_ossia0x2eclient(void);
extern "C" void setup_ossia0x2edevice(void);
extern "C" void setup_ossia0x2emodel(void);
extern "C" void setup_ossia0x2eparam(void);
extern "C" void setup_ossia0x2eremote(void);
extern "C" void setup_ossia0x2eview(void);

class ossia_pd
{
public:
    static ossia_pd& instance();
    static ossia::net::generic_device* get_default_device(){ return &instance().m_device; }

    t_eclass* client{};
    t_eclass* device{};
    t_eclass* logger{};
    t_eclass* model{};
    t_eclass* param{};
    t_eclass* remote{};
    t_eclass* view{};
    t_eclass* ossia{};

private:
    ossia_pd(); // constructor

    ossia::net::local_protocol* m_localProtocol{};
    ossia::net::generic_device m_device;
    string_map<std::shared_ptr<ossia::websocket_threaded_connection>> m_connections;
};

struct t_obj_base;

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
static t_pd*
find_parent(t_eobj* x, std::string classname, int start_level, int* level)
{
  t_canvas* canvas = x->o_canvas;

  *level = start_level;

  while (canvas && start_level)
  {
    canvas = canvas->gl_owner; // gl_owner seems to be corrupted on the root
                               // canvas : canvas has no value
    start_level--;
  }

  if (start_level > 0)
    return nullptr; // if we can't reach start level (because we reach the root
                    // canvas before the start_level) then abort

  while (canvas != 0)
  {
    t_gobj* list = canvas->gl_list;
    while (list)
    {
      std::string current = list->g_pd->c_name->s_name;
      if ((current == classname) && (&(list->g_pd) != &(x->o_obj.te_g.g_pd)))
      { // check if type match and not the same intance...
        return &(list->g_pd);
      }
      list = list->g_next;
    }
    canvas = canvas->gl_owner;
    (*level)++;
  }
  return nullptr;
}

/**
 * @brief find_parent_alive
 * @details Find a parent that is not being removed soon
 * @param x
 * @param classname
 * @param start_level
 * @return
 */
template <typename T>
static T* find_parent_alive(
    t_eobj* x, std::string classname, int start_level, int* level)
{
  T* obj = (T*)find_parent(x, classname, start_level, level);
  if (obj)
  {
    while (obj && obj->x_dead)
    {
      obj = find_parent_alive<T>(&obj->x_obj, classname, 1, level);
    }
  }
  return obj;
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
}
} // namespace
