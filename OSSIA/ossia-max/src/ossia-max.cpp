// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <boost/range/algorithm_ext/erase.hpp>
#include <ossia-max/src/client.hpp>
#include <ossia-max/src/device.hpp>
#include <ossia-max/src/logger.hpp>
#include <ossia-max/src/model.hpp>
#include <ossia-max/src/ossia-max.hpp>
#include <ossia-max/src/parameter.hpp>
#include <ossia-max/src/remote.hpp>
#include <ossia-max/src/view.hpp>
#include <ossia-max/src/utils.hpp>

#include <commonsyms.h>
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
  ossia_ossia_setup();

  post("OSSIA library for Max is loaded");
  post("build on %s at %s", __DATE__, __TIME__);
}

// ossia-max library constructor
ossia_max::ossia_max():
    m_localProtocol{new ossia::net::local_protocol},
    m_device{std::unique_ptr<ossia::net::protocol_base>(m_localProtocol), "ossia_max_device"}
{
}

// ossia-max library destructor
ossia_max::~ossia_max()
{
    for (auto x : views.copy()){
      x->m_node->about_to_be_deleted.disconnect<t_view, &t_view::is_deleted>(x);
    }
    for (auto x : remotes.copy()){
      x->m_node->about_to_be_deleted.disconnect<t_remote, &t_remote::is_deleted>(x);
    }
    for (auto x : models.copy()){
      x->m_node->about_to_be_deleted.disconnect<t_model, &t_model::is_deleted>(x);
    }
    for (auto x : parameters.copy()){
      x->m_node->about_to_be_deleted.disconnect<t_parameter, &t_parameter::is_deleted>(x);
    }
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
bool max_object_register(T* x)
{
  if (x->m_node)
    return true; // already registered
  if (x->m_dead)
    return false; // object will be removed soon

  ossia::net::node_base* node = nullptr;

  if (x->m_address_type == AddrType::global)
  {
    node = ossia::max::find_global_node(x->m_name->s_name);
  }
  else
  {
    int l;
    t_device* device = (t_device*) find_parent_box_alive(&x->m_object, gensym("ossia.device"), 0, &l);
    t_client* client = (t_client*) find_parent_box_alive(&x->m_object, gensym("ossia.client"), 0, &l);

    t_model* model = nullptr;
    t_view* view = nullptr;
    int view_level = 0, model_level = 0;
    int start_level = 0;

    if (std::is_same<T, t_view>::value || std::is_same<T, t_model>::value)
    {
      start_level = 1;
    }

    if (x->m_address_type == AddrType::relative)
    {
      // then try to locate a parent view or model
      if (std::is_same<T, t_view>::value || std::is_same<T, t_remote>::value)
      {
        view = (t_view*) find_parent_box_alive(
              &x->m_object, gensym("ossia.view"), start_level, &view_level);
      }

      if (!view)
      {
        model = (t_model*)find_parent_box_alive(
              &x->m_object, gensym("ossia.model"), start_level, &model_level);
      }
    }

    if (view)
      node = view->m_node;
    else if (model)
      node = model->m_node;
    else if (client)
      node = client->m_node;
    else if (device)
      node = device->m_node;
    else
      node = &ossia_max::get_default_device()->get_root_node();
  }

  return x->register_node(node);
}

template bool max_object_register<t_parameter>(t_parameter*);
template bool max_object_register<t_model>(t_model*);
template bool max_object_register<t_remote>(t_remote*);
template bool max_object_register<t_view>(t_view*);

template <typename T>
std::string object_path_absolute(T* x)
{
  fmt::MemoryWriter fullpath;
  std::vector<std::string> vs;

  if (std::is_same<T, t_view>::value || std::is_same<T, t_remote>::value)
  {
    int start_level = 0;
    int view_level = 0;
    t_view* view = nullptr;

    if (std::is_same<T, t_view>::value)
      start_level = 1;

    view = (t_view*)find_parent_box_alive(
        &x->m_object, gensym("ossia.view"), start_level, &view_level);
    t_view* tmp = nullptr;

    while (view)
    {
      tmp = view;
      vs.push_back(tmp->m_name->s_name);
      view = (t_view*)find_parent_box_alive(
          &tmp->m_object, gensym("ossia.view"), 1, &view_level);
    }

    t_object* object = nullptr;

    if (tmp)
      object = &tmp->m_object;
    else
      object = &x->m_object;

    int l = 0;
    t_device* device
        = (t_device*) find_parent_box(object, gensym("ossia.device"), 0, &l);
    t_client* client
        = (t_client*) find_parent_box(object, gensym("ossia.client"), 0, &l);

    if (client)
      fullpath << client->m_name->s_name
               << ":";
    else if (device)
      fullpath << device->m_name->s_name
               << ":";
    else
      fullpath << ossia_max::get_default_device()->get_name()
               << ":";
  }
  else
  {
    int start_level = 0;
    int model_level = 0;

    t_model* model = nullptr;

    if (std::is_same<T, t_model>::value)
      start_level = 1;

    model = (t_model*) find_parent_box_alive(
        &x->m_object, gensym("ossia.model"), start_level, &model_level);
    t_model* tmp = nullptr;

    while (model)
    {
      tmp = model;

      vs.push_back(tmp->m_name->s_name);
      model = (t_model*) find_parent_box_alive(
          &tmp->m_object, gensym("ossia.model"), 1, &model_level);
    }

    t_object* object = nullptr;
    if (tmp)
      object = &tmp->m_object;
    else
      object = &x->m_object;

    int l = 0;
    t_device* device = (t_device*)find_parent_box(object, gensym("ossia.device"), 0, &l);

    if (device)
      fullpath << device->m_name->s_name
               << ":";
    else
       fullpath << ossia_max::get_default_device()->get_name()
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
    max_object_register<t_model>(static_cast<t_model*>(model));
  }

  for (auto parameter : t_parameter::quarantine().copy())
  {
    max_object_register<t_parameter>(static_cast<t_parameter*>(parameter));
  }

  for (auto view : t_view::quarantine().copy())
  {
    max_object_register<t_view>(static_cast<t_view*>(view));
  }

  for (auto remote : t_remote::quarantine().copy())
  {
    max_object_register<t_remote>(static_cast<t_remote*>(remote));
  }
}

t_object_base* find_parent_box(
    t_object* object, t_symbol* classname, int start_level, int* level)
{

  t_object* patcher = get_patcher(object);
  t_object_base* parent = nullptr;

  // look upper if there is an upper level and if it is not the level where to
  // start the research
  while (patcher && start_level--)
    patcher = jpatcher_get_parentpatcher(patcher);

  // if no parent object have been found in upper patcher, look around
  while (patcher && !parent)
  {
    t_object* box =  jpatcher_get_firstobject(patcher);

    while (box)
    {
      if (object_attr_getsym(box, _sym_maxclass) == classname)
      {
        t_object* object_box = NULL;
        object_obex_lookup(object, gensym("#B"), &object_box);

        // the object itself cannot be its own parent
        if (box != object_box)
        {
          parent = (t_object_base*)jbox_get_object(box);
          *level = start_level;
          break;
        }
      }

      box = jbox_get_nextobject(box);
    }
    patcher = jpatcher_get_parentpatcher(patcher);
  }

  return parent;
}

t_object_base* find_parent_box_alive(
    t_object* object, t_symbol* classname, int start_level, int* level)
{
  t_object_base* parent
      = find_parent_box(object, classname, start_level, level);

  while (parent && parent->m_dead)
  {
    parent
        = find_parent_box_alive(&parent->m_object, classname, 1, level);
  }

  return parent;
}

std::vector<t_object_base*> find_children_to_register(
    t_object* object, t_object* patcher, t_symbol* classname)
{
  t_symbol* subclassname = classname == gensym("ossia.model")
                               ? gensym("ossia.parameter")
                               : gensym("ossia.remote");

  std::vector<t_object_base*> found;
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
      if (next_box != object_box && next_box != nullptr)
      {
        t_object_base* o = (t_object_base*) jbox_get_object(next_box);

        // ignore dying object
        if (!o->m_dead)
          found.push_back(o);
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

        std::vector<t_object_base*> found_tmp
            = find_children_to_register(object, patcher, classname);

        for (auto obj : found_tmp)
        {
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

        // the object itself shouln't be stored
        if (object_box)
        {
          t_object_base* o = (t_object_base*) jbox_get_object(next_box);
          found.push_back(o);
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
    t_object* parentclass = object_attr_getobj(object, _sym_parentclass);
    t_symbol *cls = object_classname(parentclass);
    post("parentclasse name %s", cls->s_name);
    // this crashes if box is in the root patcher
    // patcher = object_attr_getobj(object, _sym_parentpatcher);
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
