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
#include <boost/functional/hash.hpp>

#include <commonsyms.h>
#pragma mark -
#pragma mark library

#include <git_info.h>

namespace std
{
template <typename T, typename U>
class hash<std::pair<T*, U*>>
{
public:
  std::size_t operator()(const std::pair<T*, U*>& p) const
  {
    std::size_t seed = 0;
    boost::hash_combine(seed, p.first);
    boost::hash_combine(seed, p.second);
    return seed;
  }
};
}
using namespace ossia::max;

// ossia-max library constructor
ossia_max::ossia_max():
    m_localProtocol{new ossia::net::local_protocol},
    m_device{std::unique_ptr<ossia::net::protocol_base>(m_localProtocol), "ossia_max_device"}
{
  common_symbols_init();

  parameters.reserve(2048);
  remotes.reserve(1024);
  attributes.reserve(512);
  models.reserve(512);
  views.reserve(512);
  devices.reserve(4);
  clients.reserve(4);

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
void object_quarantining(T* x)
{
  x->m_node_selection.clear();
  if (!object_is_quarantined<T>(x))
    x->quarantine().push_back(x);
}

template void object_quarantining<parameter>(parameter*);
template void object_quarantining<attribute>(attribute*);
template void object_quarantining<model>(model*);
template void object_quarantining<remote>(remote*);
template void object_quarantining<view>(view*);

template <typename T>
void object_dequarantining(T* x)
{
  x->quarantine().remove_all(x);
}

template void object_dequarantining<attribute>(attribute*);
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
  static tsl::hopscotch_map<std::pair<t_object*, t_symbol*>, object_base*> parent_map;
  auto it = parent_map.find({object, classname});
  if(it != parent_map.end())
  {
    return it->second;
  }

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
  parent_map[std::make_pair(object, classname)] = parent;
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
