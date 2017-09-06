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
#include <ossia-max/src/object_base.hpp>

#include <commonsyms.h>
#pragma mark -
#pragma mark library

#include <git_info.h>

using namespace ossia::max;

// ossia-max library constructor
ossia_max::ossia_max():
    m_localProtocol{new ossia::net::local_protocol},
    m_device{std::unique_ptr<ossia::net::protocol_base>(m_localProtocol), "ossia_max_device"}
{
  common_symbols_init();
  post("OSSIA library for Max is loaded");
  post("build from SHA : %s %s", ossia::get_commit_sha().c_str(), ossia::get_commit_date().c_str());
}

// ossia-max library destructor
ossia_max::~ossia_max()
{
    for (auto x : views.copy()){
      x->m_matchers.clear();
    }
    for (auto x : remotes.copy()){
      x->m_matchers.clear();
    }
    for (auto x : models.copy()){
      x->m_matchers.clear();
    }
    for (auto x : parameters.copy()){
      x->m_matchers.clear();
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

template <typename T>
std::string object_path_absolute(T* x)
{
  fmt::MemoryWriter fullpath;
  std::vector<std::string> vs;

  if (std::is_same<T, view>::value || std::is_same<T, remote>::value)
  {
    int start_level = 0;
    int view_level = 0;
    view* view = nullptr;

    if (std::is_same<T, ossia::max::view>::value)
      start_level = 1;

    view = (ossia::max::view*)find_parent_box_alive(
        &x->m_object, gensym("ossia.view"), start_level, &view_level);
    ossia::max::view* tmp = nullptr;

    while (view)
    {
      tmp = view;
      vs.push_back(tmp->m_name->s_name);
      view = (ossia::max::view*)find_parent_box_alive(
          &tmp->m_object, gensym("ossia.view"), 1, &view_level);
    }

    t_object* object = nullptr;

    if (tmp)
      object = &tmp->m_object;
    else
      object = &x->m_object;

    int l = 0;
    ossia::max::device* device = (ossia::max::device*)
        find_parent_box(object, gensym("ossia.device"), 0, &l);
    ossia::max::client* client = (ossia::max::client*)
        find_parent_box(object, gensym("ossia.client"), 0, &l);

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

    ossia::max::model* model = nullptr;

    if (std::is_same<T, ossia::max::model>::value)
      start_level = 1;

    model = (ossia::max::model*) find_parent_box_alive(
        &x->m_object, gensym("ossia.model"), start_level, &model_level);
    ossia::max::model* tmp = nullptr;

    while (model)
    {
      tmp = model;

      vs.push_back(tmp->m_name->s_name);
      model = (ossia::max::model*) find_parent_box_alive(
          &tmp->m_object, gensym("ossia.model"), 1, &model_level);
    }

    t_object* object = nullptr;
    if (tmp)
      object = &tmp->m_object;
    else
      object = &x->m_object;

    int l = 0;
    ossia::max::device* device = (ossia::max::device*)
        find_parent_box(object, gensym("ossia.device"), 0, &l);

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

template std::string object_path_absolute<parameter>(parameter*);
template std::string object_path_absolute<model>(model*);
template std::string object_path_absolute<remote>(remote*);
template std::string object_path_absolute<view>(view*);

template <typename T>
void object_quarantining(T* x)
{
  if (!object_is_quarantined<T>(x))
    x->quarantine().push_back(x);
}

template void object_quarantining<parameter>(parameter*);
template void object_quarantining<model>(model*);
template void object_quarantining<remote>(remote*);
template void object_quarantining<view>(view*);

template <typename T>
void object_dequarantining(T* x)
{
  x->quarantine().remove_all(x);
}

template void object_dequarantining<parameter>(parameter*);
template void object_dequarantining<model>(model*);
template void object_dequarantining<remote>(remote*);
template void object_dequarantining<view>(view*);

template <typename T>
bool object_is_quarantined(T* x)
{
  return x->quarantine().contains(x);
}

template bool object_is_quarantined<parameter>(parameter*);
template bool object_is_quarantined<model>(model*);
template bool object_is_quarantined<remote>(remote*);
template bool object_is_quarantined<view>(view*);

template <typename T>
void object_dump(T* x)
{

  /*
  t_atom a;
  std::string fullpath;

  for (auto node : x->m_nodes)
  {
    fullpath = ossia::net::address_string_from_node(*node);
    atom_setsym(&a, gensym(fullpath.c_str()));
    outlet_anything(x->m_dumpout, gensym("fullpath"), 1, &a);
  }

  fullpath = object_path_absolute(x);
  if (fullpath.back() != '/')
    fullpath += "/";

  fullpath += x->m_name->s_name;
  atom_setsym(&a, gensym(fullpath.c_str()));
  outlet_anything(x->m_dumpout, gensym("maxpath"), 1, &a);

  if (!x->m_nodes.empty())
    atom_setfloat(&a, 1.);
  else
    atom_setfloat(&a, 0.);

  outlet_anything(x->m_dumpout, gensym("registered"), 1, &a);

  atom_setfloat(&a, object_is_quarantined(x));
  outlet_anything(x->m_dumpout, gensym("quarantined"), 1, &a);

  if (!x->m_nodes.empty())
  {
    ossia::net::parameter_base* address = x->m_nodes->get_parameter();
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
        case ossia::val_type::LIST:
          type = "list";
          break;
        case ossia::val_type::CHAR:
          type = "char";
          break;
        default:
          type = "unknown";
      }

      atom_setsym(&a, gensym(type.c_str()));
      outlet_anything(x->m_dumpout, gensym("type"), 1, &a);

      // domain
      ossia::domain domain = address->get_domain();
      atom_setsym(&a, gensym(domain.to_pretty_string().c_str()));
      outlet_anything(x->m_dumpout, gensym("domain"), 1, &a);

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
      outlet_anything(x->m_dumpout, gensym("bounding_mode"), 1, &a);

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
      outlet_anything(x->m_dumpout, gensym("access_mode"), 1, &a);

      // repetition filter
      bool rep = address->get_repetition_filter();
      atom_setfloat(&a, rep);
      outlet_anything(x->m_dumpout, gensym("repetition_filter"), 1, &a);

      // unit
      // TODO
      address->get_unit();
    }

    // description
    auto description = ossia::net::get_description(*(x->m_nodes));
    if (description)
    {
      atom_setsym(&a, gensym((*description).c_str()));
      outlet_anything(x->m_dumpout, gensym("description"), 1, &a);
    }
    else
      outlet_anything(x->m_dumpout, gensym("tags"), 0, nullptr);

    // tags
    auto tags = ossia::net::get_tags(*x->m_nodes);
    if (tags)
    {
      std::size_t N = (*tags).size();
      std::vector<t_atom> l(N);
      for (std::size_t i = 0; i < N; i++)
      {
        atom_setsym(&l[i], gensym((*tags)[i].c_str()));
      }
      outlet_anything(x->m_dumpout, gensym("tags"), N, l.data());
    }
    else
      outlet_anything(x->m_dumpout, gensym("tags"), 0, nullptr);
  }
  */
}

template void object_dump<parameter>(parameter*);
template void object_dump<model>(model*);
template void object_dump<remote>(remote*);
template void object_dump<view>(view*);

#pragma mark -
#pragma mark Utilities

void register_quarantinized()
{
  for (auto model : model::quarantine().copy())
  {
    max_object_register<ossia::max::model>(
          static_cast<ossia::max::model*>(model));
  }

  for (auto parameter : parameter::quarantine().copy())
  {
    max_object_register<ossia::max::parameter>(
          static_cast<ossia::max::parameter*>(parameter));
  }

  for (auto view : view::quarantine().copy())
  {
    max_object_register<ossia::max::view>(
          static_cast<ossia::max::view*>(view));
  }

  for (auto remote : remote::quarantine().copy())
  {
    max_object_register<ossia::max::remote>(
          static_cast<ossia::max::remote*>(remote));
  }
}

object_base* find_parent_box(
    t_object* object, t_symbol* classname, int start_level, int* level)
{

  t_object* patcher = get_patcher(object);
  object_base* parent = nullptr;

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
          parent = (object_base*)jbox_get_object(box);
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

object_base* find_parent_box_alive(
    t_object* object, t_symbol* classname, int start_level, int* level)
{
  object_base* parent
      = find_parent_box(object, classname, start_level, level);

  while (parent && parent->m_dead)
  {
    parent
        = find_parent_box_alive(&parent->m_object, classname, 1, level);
  }

  return parent;
}

std::vector<object_base*> find_children_to_register(
    t_object* object, t_object* patcher, t_symbol* classname, bool* found_dev)
{
  t_symbol* subclassname = classname == gensym("ossia.model")
                               ? gensym("ossia.parameter")
                               : gensym("ossia.remote");

  std::vector<object_base*> found;
  bool found_model = false;
  bool found_view = false;

  // 1: look for [classname] objects into the patcher
  t_object* next_box = object_attr_getobj(patcher, _sym_firstobject);

  while (next_box)
  {
    t_symbol* curr_classname = object_attr_getsym(next_box, _sym_maxclass);
    if (curr_classname == classname)
    {
      t_object* object_box = NULL;
      object_obex_lookup(object, gensym("#B"), &object_box);

      // the object itself cannot be stored into the hierachy
      if (next_box != object_box && next_box != nullptr)
      {
        object_base* o = (object_base*) jbox_get_object(next_box);

        // ignore dying object
        if (!o->m_dead)
          found.push_back(o);
      }
    }

    // if we're looking for ossia.view but found a model, remind it
    if ( classname == gensym("ossia.view")
         && curr_classname == gensym("ossia.model") )
      found_model = true;
    else if ( classname == gensym("ossia.model")
              && curr_classname == gensym("ossia.view") )
      found_view = true;

    // if there is a client or device in the current patcher
    // don't register anything
    if ( found_dev
         && (curr_classname == gensym("ossia.device")
             || curr_classname == gensym("ossia.client")) )
    {
      t_object* object_box = NULL;
      object_obex_lookup(object, gensym("#B"), &object_box);
      // the object itself cannot be stored into the hierachy
      if (next_box != object_box && next_box != nullptr)
      {
        object_base* o = (object_base*) jbox_get_object(next_box);

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
        bool _found_dev = false;
        std::vector<object_base*> found_tmp
            = find_children_to_register(object, patcher, classname, &_found_dev);

        if (!_found_dev)
        {
          found.insert(found.end(),found_tmp.begin(), found_tmp.end());
        }
      }

      next_box = object_attr_getobj(next_box, _sym_nextobject);
    }

    // 3: finally look for ossia.param / ossia.remote in the same pather
    next_box = object_attr_getobj(patcher, _sym_firstobject);

    while (next_box)
    {
      t_symbol* current = object_attr_getsym(next_box, _sym_maxclass);
      if (current == subclassname
          || ( !found_view && current == gensym("ossia.remote") ) )
      {
        t_object* object_box = NULL;
        object_obex_lookup(object, gensym("#B"), &object_box);

        // the object itself shouln't be stored
        if (object_box)
        {
          object_base* o = (object_base*) jbox_get_object(next_box);
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

  /*
  // if the box is in a bpatcher, the patcher is NULL
  if (!patcher)
  {
    // FIXME : this crashes too on [ossia.model root] in the toplevel patcher
    t_object* parentclass = object_attr_getobj(object, _sym_parentclass);
    t_symbol *cls = object_classname(parentclass);
    post("parentclasse name %s", cls->s_name);
    // this crashes if box is in the root patcher
    // patcher = object_attr_getobj(object, _sym_parentpatcher);
  }
  */

  return patcher;
}

std::vector<std::string> parse_tags_symbol(t_symbol** tags_symbol, long size)
{
  std::vector<std::string> tags;

  for(int i=0;i<size;i++)
  {
    tags.push_back(tags_symbol[i]->s_name);
  }

  return tags;
}

} // max namespace
} // ossia namespace
