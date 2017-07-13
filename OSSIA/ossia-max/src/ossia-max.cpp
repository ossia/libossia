#include <boost/range/algorithm_ext/erase.hpp>
#include <commonsyms.h>
#include <ossia-max/src/client.hpp>
#include <ossia-max/src/device.hpp>
#include <ossia-max/src/logger.hpp>
#include <ossia-max/src/model.hpp>
#include <ossia-max/src/ossia-max.hpp>
#include <ossia-max/src/parameter.hpp>
#include <ossia-max/src/remote.hpp>
#include <ossia-max/src/view.hpp>

#pragma mark -
#pragma mark library

using namespace ossia::max;

// ossia-max library loading
extern "C" void ext_main(void* r)
{
  common_symbols_init();
  ossia_client_setup();
  ossia_device_setup();
  ossia_logger_setup();
  ossia_model_setup();
  ossia_parameter_setup();
  ossia_remote_setup();
  ossia_view_setup();

  post("OSSIA library for Max is loaded");
}

// ossia-max library constructor
ossia_max::ossia_max()
{
}

// ossia-max library instance
ossia_max& ossia_max::instance()
{
  static ossia_max library_instance;
  return library_instance;
}

namespace ossia
{
namespace max
{

#pragma mark -
#pragma mark Templates

template <typename T>
bool object_register(T* x)
{
  if (x->m_node)
    return true; // already registered
  if (x->m_dead)
    return false; // object will be removed soon

  int l;
  t_object* device_box
      = find_parent_box(&x->m_object, gensym("ossia.device"), 0, &l);
  t_object* client_box
      = find_parent_box(&x->m_object, gensym("ossia.client"), 0, &l);

  // first try to locate a ossia.device in the parent hierarchy...
  if (!device_box && !client_box)
    return false; // not ready to register : if there is no device, nothing
                  // could be registered

  t_object* model_box = nullptr;
  t_object* view_box = nullptr;
  int view_level = 0, model_level = 0;

  if (!x->m_absolute)
  {
    // then try to locate a parent view or model
    if (std::is_same<T, t_view>::value || std::is_same<T, t_remote>::value)
      view_box = find_parent_box_alive(
          &x->m_object, gensym("ossia.view"), 0, &view_level);
    else
      model_box = find_parent_box_alive(
          &x->m_object, gensym("ossia.model"), 0, &model_level);
  }

  ossia::net::node_base* node = nullptr;

  if (view_box)
    node = ((t_view*)jbox_get_object(view_box))->m_node;
  else if (model_box)
    node = ((t_model*)jbox_get_object(model_box))->m_node;
  else if (client_box)
    node = ((t_client*)jbox_get_object(client_box))->m_node;
  else
    node = ((t_device*)jbox_get_object(device_box))->m_node;

  return x->register_node(node);
}

template bool object_register<t_parameter>(t_parameter*);
template bool object_register<t_model>(t_model*);
template bool object_register<t_remote>(t_remote*);
template bool object_register<t_view>(t_view*);

template <typename T>
std::string object_path_absolute(T* x)
{
  std::vector<std::string> vs;

  t_object* model_box = nullptr;
  t_object* view_box = nullptr;
  int view_level = 0, model_level = 0;
  std::stringstream fullpath;

  if (std::is_same<T, t_view>::value || std::is_same<T, t_remote>::value)
  {
    int start_level = 0;

    if (std::is_same<T, t_view>::value)
      start_level = 1;

    view_box = find_parent_box_alive(
        &x->m_object, gensym("ossia.view"), start_level, &view_level);
    t_view* tmp = nullptr;

    while (view_box)
    {
      tmp = (t_view*)jbox_get_object(view_box);

      vs.push_back(tmp->m_name->s_name);
      view_box = find_parent_box_alive(
          &tmp->m_object, gensym("ossia.view"), 1, &view_level);
    }

    t_object* object = nullptr;

    if (tmp)
      object = &tmp->m_object;
    else
      object = &x->m_object;

    int l = 0;
    t_object* device_box
        = find_parent_box(object, gensym("ossia.device"), 0, &l);
    t_object* client_box
        = find_parent_box(object, gensym("ossia.client"), 0, &l);

    if (client_box)
      fullpath << ((t_client*)jbox_get_object(client_box))->m_name->s_name
               << ":";
    else if (device_box)
      fullpath << ((t_device*)jbox_get_object(device_box))->m_name->s_name
               << ":";
  }
  else
  {
    int start_level = 0;
    if (std::is_same<T, t_model>::value)
      start_level = 1;

    model_box = find_parent_box_alive(
        &x->m_object, gensym("ossia.model"), start_level, &model_level);
    t_model* tmp = nullptr;

    while (model_box)
    {
      tmp = (t_model*)jbox_get_object(model_box);

      vs.push_back(tmp->m_name->s_name);
      model_box = find_parent_box_alive(
          &tmp->m_object, gensym("ossia.model"), 1, &model_level);
    }

    t_object* object = nullptr;
    if (tmp)
      object = &tmp->m_object;
    else
      object = &x->m_object;

    int l = 0;
    t_object* device_box = nullptr;

    if (object)
      device_box = find_parent_box(object, gensym("ossia.device"), 0, &l);

    if (device_box)
      fullpath << ((t_device*)jbox_get_object(device_box))->m_name->s_name
               << ":";
  }

  auto rit = vs.rbegin();
  for (; rit != vs.rend(); ++rit)
  {
    fullpath << "/" << *rit;
  }

  if (vs.empty())
    fullpath << "/";

  return fullpath.str();
}

template std::string object_path_absolute<t_parameter>(t_parameter*);
template std::string object_path_absolute<t_model>(t_model*);
template std::string object_path_absolute<t_remote>(t_remote*);
template std::string object_path_absolute<t_view>(t_view*);

template <typename T>
void object_quarantining(T* x)
{
  if (!object_is_quarantined<T>(x))
    x->quarantine().push_back(x);
}

template void object_quarantining<t_parameter>(t_parameter*);
template void object_quarantining<t_model>(t_model*);
template void object_quarantining<t_remote>(t_remote*);
template void object_quarantining<t_view>(t_view*);

template <typename T>
void object_dequarantining(T* x)
{
  x->quarantine().remove_all(x);
}

template void object_dequarantining<t_parameter>(t_parameter*);
template void object_dequarantining<t_model>(t_model*);
template void object_dequarantining<t_remote>(t_remote*);
template void object_dequarantining<t_view>(t_view*);

template <typename T>
bool object_is_quarantined(T* x)
{
  return x->quarantine().contains(x);
}

template bool object_is_quarantined<t_parameter>(t_parameter*);
template bool object_is_quarantined<t_model>(t_model*);
template bool object_is_quarantined<t_remote>(t_remote*);
template bool object_is_quarantined<t_view>(t_view*);

template <typename T>
void object_dump(T* x)
{
  t_atom a;
  std::string fullpath;

  if (x->m_node)
  {
    fullpath = ossia::net::address_string_from_node(*x->m_node);
    atom_setsym(&a, gensym(fullpath.c_str()));
    outlet_anything(x->m_dump_out, gensym("fullpath"), 1, &a);
  }

  fullpath = object_path_absolute(x);
  if (fullpath.back() != '/')
    fullpath += "/";

  fullpath += x->m_name->s_name;
  atom_setsym(&a, gensym(fullpath.c_str()));
  outlet_anything(x->m_dump_out, gensym("maxpath"), 1, &a);

  if (x->m_node)
    atom_setfloat(&a, 1.);
  else
    atom_setfloat(&a, 0.);

  outlet_anything(x->m_dump_out, gensym("registered"), 1, &a);

  atom_setfloat(&a, object_is_quarantined(x));
  outlet_anything(x->m_dump_out, gensym("quarantined"), 1, &a);

  if (x->m_node)
  {
    ossia::net::address_base* address = x->m_node->get_address();
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
          type = " int";
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

      atom_setsym(&a, gensym(type.c_str()));
      outlet_anything(x->m_dump_out, gensym("type"), 1, &a);

      // domain
      ossia::domain domain = address->get_domain();
      atom_setsym(&a, gensym(domain.to_pretty_string().c_str()));
      outlet_anything(x->m_dump_out, gensym("domain"), 1, &a);

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

      atom_setsym(&a, gensym(bounding_mode.c_str()));
      outlet_anything(x->m_dump_out, gensym("bounding_mode"), 1, &a);

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

      atom_setsym(&a, gensym(access_mode.c_str()));
      outlet_anything(x->m_dump_out, gensym("access_mode"), 1, &a);

      // repetition filter
      bool rep = address->get_repetition_filter();
      atom_setfloat(&a, rep);
      outlet_anything(x->m_dump_out, gensym("repetition_filter"), 1, &a);

      // unit
      // TODO
      address->get_unit();
    }

    // description
    auto description = ossia::net::get_description(*(x->m_node));
    if (description)
    {
      atom_setsym(&a, gensym((*description).c_str()));
      outlet_anything(x->m_dump_out, gensym("description"), 1, &a);
    }
    else
      outlet_anything(x->m_dump_out, gensym("tags"), 0, nullptr);

    // tags
    auto tags = ossia::net::get_tags(*x->m_node);
    if (tags)
    {
      std::size_t N = (*tags).size();
      std::vector<t_atom> l(N);
      for (std::size_t i = 0; i < N; i++)
      {
        atom_setsym(&l[i], gensym((*tags)[i].c_str()));
      }
      outlet_anything(x->m_dump_out, gensym("tags"), N, l.data());
    }
    else
      outlet_anything(x->m_dump_out, gensym("tags"), 0, nullptr);
  }
}

template void object_dump<t_parameter>(t_parameter*);
template void object_dump<t_model>(t_model*);
template void object_dump<t_remote>(t_remote*);
template void object_dump<t_view>(t_view*);

#pragma mark -
#pragma mark Utilities

void register_quarantinized()
{
  for (auto model : t_model::quarantine().copy())
  {
    object_register<t_model>(static_cast<t_model*>(model));
  }

  for (auto parameter : t_parameter::quarantine().copy())
  {
    object_register<t_parameter>(static_cast<t_parameter*>(parameter));
  }

  for (auto view : t_view::quarantine().copy())
  {
    object_register<t_view>(static_cast<t_view*>(view));
  }

  for (auto remote : t_remote::quarantine().copy())
  {
    object_register<t_remote>(static_cast<t_remote*>(remote));
  }
}

t_object* find_parent_box(
    t_object* object, t_symbol* classname, int start_level, int* level)
{
  t_object* parent_box = nullptr;

  // look upper if there is an upper level and if it is not the level where to
  // start the research
  t_object* patcher = get_patcher(object);

  if (patcher && start_level)
    parent_box = find_parent_box(patcher, classname, start_level--, level);

  // if no parent object have been found in upper patcher, look around
  if (!parent_box)
  {
    t_object* next_box = object_attr_getobj(patcher, _sym_firstobject);

    while (next_box)
    {
      if (object_attr_getsym(next_box, _sym_maxclass) == classname)
      {
        t_object* object_box = NULL;
        object_obex_lookup(object, gensym("#B"), &object_box);

        // the object itself cannot be its own parent
        if (next_box != object_box)
        {
          parent_box = next_box;
          *level = start_level;
          break;
        }
      }

      next_box = object_attr_getobj(next_box, _sym_nextobject);
    }
  }

  return parent_box;
}

t_object* find_parent_box_alive(
    t_object* object, t_symbol* classname, int start_level, int* level)
{
  t_object* parent_box
      = find_parent_box(object, classname, start_level, level);

  if (parent_box)
  {
    t_object_base* object = (t_object_base*)jbox_get_object(parent_box);

    if (object)
    {
      while (object && object->m_dead)
      {
        parent_box
            = find_parent_box_alive(&object->m_object, classname, 1, level);
        object = (t_object_base*)jbox_get_object(parent_box);
      }
    }
  }

  return parent_box;
}

std::vector<box_hierachy> find_children_to_register(
    t_object* object, t_object* patcher, t_symbol* classname)
{
  t_symbol* subclassname = classname == gensym("ossia.model")
                               ? gensym("ossia.parameter")
                               : gensym("ossia.remote");

  std::vector<box_hierachy> found;
  t_object* next_box;

  // 1: look for [classname] objects into the patcher
  next_box = object_attr_getobj(patcher, _sym_firstobject);

  while (next_box)
  {
    if (object_attr_getsym(next_box, _sym_maxclass) == classname)
    {
      t_object* object_box = NULL;
      object_obex_lookup(object, gensym("#B"), &object_box);

      // the object itself cannot be stored into the hierachy
      if (next_box != object_box)
      {
        box_hierachy oh;

        oh.box = next_box;
        oh.hierarchy = 0;
        oh.classname = classname;

        // ignore dying object
        if (!((t_object_base*)jbox_get_object(oh.box))->m_dead)
          found.push_back(oh);
      }
    }

    next_box = object_attr_getobj(next_box, _sym_nextobject);
  }

  // 2: if there is no ossia.model / ossia.view in the current patch, look into
  // the subpatches
  if (found.empty())
  {
    next_box = object_attr_getobj(patcher, _sym_firstobject);

    while (next_box)
    {
      t_symbol* next_box_classname
          = object_attr_getsym(next_box, _sym_maxclass);

      // jpatcher or bpatcher case
      if (next_box_classname == _sym_jpatcher
          || next_box_classname == _sym_bpatcher)
      {
        t_object* patcher = jbox_get_object(next_box);

        std::vector<box_hierachy> found_tmp
            = find_children_to_register(object, patcher, classname);

        for (auto obj : found_tmp)
        {
          obj.hierarchy++; // increase hierarchy of objects found in a
                           // subpatcher
          found.push_back(obj);
        }
      }

      next_box = object_attr_getobj(next_box, _sym_nextobject);
    }

    // 3: finally look for ossia.param / ossia.remote in the same pather
    next_box = object_attr_getobj(patcher, _sym_firstobject);

    while (next_box)
    {
      if (object_attr_getsym(next_box, _sym_maxclass) == subclassname)
      {
        t_object* object_box = NULL;
        object_obex_lookup(object, gensym("#B"), &object_box);

        // the object itself cannot be stored into the hierachy
        if (next_box != object_box)
        {
          box_hierachy oh;

          oh.box = next_box;
          oh.hierarchy = 0;
          oh.classname = subclassname;

          found.push_back(oh);
        }
      }

      next_box = object_attr_getobj(next_box, _sym_nextobject);
    }
  }

  return found;
}

t_object* get_patcher(t_object* object)
{
  t_object* patcher = NULL;
  object_obex_lookup(object, gensym("#P"), &patcher);

  // if the box is in a bpatcher, the patcher is NULL
  if (!patcher)
  {
    patcher = object_attr_getobj(object, _sym_parentpatcher);
  }

  return patcher;
}

std::vector<std::string> parse_tags_symbol(t_symbol* tags_symbol)
{
  std::vector<std::string> tags;

  if (tags_symbol)
  {
    char* c = tags_symbol->s_name;
    std::string tag = "";

    while (*c != '\0')
    {
      if (*c == ' ')
      {
        tags.push_back(tag);
        tag = std::string("");
      }
      else
        tag += *c;

      c++;
    }
    tags.push_back(tag);
  }

  return tags;
}

} // max namespace
} // ossia namespace
