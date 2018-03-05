// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <boost/range/algorithm_ext/erase.hpp>

#include <ossia-max/src/ossia-max.hpp>
#include <ossia-max/src/utils.hpp>

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

  m_device.on_attribute_modified.connect<&device_base::on_attribute_modified_callback>();

  parameters.reserve(2048);
  remotes.reserve(1024);
  attributes.reserve(512);
  models.reserve(512);
  views.reserve(512);
  devices.reserve(8);
  clients.reserve(8);

  m_reg_clock = clock_new(this, (method) ossia_max::register_nodes);

  post("OSSIA library for Max is loaded");
  post("build SHA : %s", ossia::get_commit_sha().c_str());
}

// ossia-max library destructor
ossia_max::~ossia_max()
{
  m_device.on_attribute_modified.disconnect<&device_base::on_attribute_modified_callback>();

  for (auto x : devices.copy())
  {
    auto& multiplex = static_cast<ossia::net::multiplex_protocol&>(
                x->m_device->get_protocol());
    auto& protos = multiplex.get_protocols();
    for (auto& proto : protos)
    {
      multiplex.stop_expose_to(*proto);
    }
    x->m_protocols.clear();
    x->disconnect_slots();
  }
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

void ossia_max::register_nodes(void* x)
{
  auto& inst = ossia_max::instance();
  auto& map = inst.root_patcher;
  for (auto it = map.begin(); it != map.end(); it++)
  {
    t_object* patcher = it->first;
    it->second = true;
    for (auto dev : inst.devices.reference())
    {
      if(dev->m_patcher_hierarchy.back() == patcher)
        ossia::max::device::register_children(dev);
    }
    for (auto model : inst.models.reference())
    {
      if ( model->m_patcher_hierarchy.back() == patcher
            && model->m_matchers.empty())
        ossia_register(model);
    }
    for (auto param : inst.parameters.reference())
    {
      if ( param->m_patcher_hierarchy.back() == patcher
            && param->m_matchers.empty())
        ossia_register(param);
    }

    for (auto client : inst.clients.reference())
    {
      if(client->m_patcher_hierarchy.back() == patcher)
        ossia::max::client::register_children(client);
    }
    for (auto view : inst.views.reference())
    {
      if ( view->m_patcher_hierarchy.back() == patcher
            && view->m_matchers.empty())
        ossia_register(view);
    }
    for (auto remote : inst.remotes.reference())
    {
      if ( remote->m_patcher_hierarchy.back() == patcher
            && remote->m_matchers.empty())
        ossia_register(remote);
    }
    for (auto attr : inst.attributes.reference())
    {
      if ( attr->m_patcher_hierarchy.back() == patcher
            && attr->m_matchers.empty())
        ossia_register(attr);
    }

    for (auto dev : inst.devices.reference())
    {
      if(dev->m_patcher_hierarchy.back() == patcher)
        node_base::push_default_value(dev);
    }

    std::vector<ossia::net::node_base*> list;

    // send default value for default device's child
    // TODO make this a method of ossia Max's object
    auto n = &inst.get_default_device()->get_root_node();
    list = ossia::net::list_all_child(n);

    for (ossia::net::node_base* child : list)
    {
      if (auto param = child->get_parameter())
      {
        auto val = ossia::net::get_default_value(*child);
        if(val)
        {
          param->push_value(*val);
          trig_output_value(child);
        }
      }
    }
  }

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
    ossia_register<ossia::max::model>(
          static_cast<ossia::max::model*>(model));
  }

  for (auto parameter : parameter::quarantine().copy())
  {
    ossia_register<ossia::max::parameter>(
          static_cast<ossia::max::parameter*>(parameter));
  }

  for (auto view : view::quarantine().copy())
  {
    ossia_register<ossia::max::view>(
          static_cast<ossia::max::view*>(view));
  }

  for (auto remote : remote::quarantine().copy())
  {
    ossia_register<ossia::max::remote>(
          static_cast<ossia::max::remote*>(remote));
  }
}

std::vector<object_base*> find_children_to_register(
    t_object* caller, t_object* root_patcher, t_symbol* search_symbol, bool search_dev)
{
  t_symbol* subclassname = search_symbol == gensym("ossia.model")
                               ? gensym("ossia.parameter")
                               : gensym("ossia.remote");

  std::vector<object_base*> found;
  bool found_model = false;
  bool found_view = false;

  // 1: look for [classname] objects into the patcher
  t_object* next_box = object_attr_getobj(root_patcher, _sym_firstobject);

  t_object* object_box = NULL;
  object_obex_lookup(caller, gensym("#B"), &object_box);

  while (next_box)
  {
    if(next_box != object_box)
    {
      t_symbol* curr_classname = object_attr_getsym(next_box, _sym_maxclass);
      if (curr_classname == search_symbol)
      {
          object_base* o = (object_base*) jbox_get_object(next_box);

          // ignore dying object
          if (!o->m_dead)
            found.push_back(o);

      }

      // if we're looking for ossia.view but found a model, remind it
      if ( search_symbol == gensym("ossia.view")
           && curr_classname == gensym("ossia.model") )
        found_model = true;
      else if ( search_symbol == gensym("ossia.model")
                && curr_classname == gensym("ossia.view") )
        found_view = true;

      // if there is a client or device in the current patcher
      // don't register anything
      if ( search_dev
           && ( curr_classname == gensym("ossia.device")
             || curr_classname == gensym("ossia.client") ))
      {
        return {};
      }
    }
    next_box = object_attr_getobj(next_box, _sym_nextobject);
  }

  // 2: if there is no ossia.model / ossia.view in the current patch, look into
  // the subpatches
  if (found.empty())
  {
    next_box = object_attr_getobj(root_patcher, _sym_firstobject);

    while (next_box)
    {
      t_object* object = jbox_get_object(next_box);
      t_symbol* classname = object_classname(object);

      // jpatcher or bpatcher case
      if (classname == _sym_jpatcher
          || classname == _sym_bpatcher)
      {
        std::vector<object_base*> found_tmp
            = find_children_to_register(caller, object, search_symbol, true);

        found.insert(found.end(),found_tmp.begin(), found_tmp.end());

      }
      else if (classname == gensym("poly~"))
      {
        long idx = 0;
        t_object* subpatcher = (t_object*)object_method(object, gensym("subpatcher"), idx++, 0);
        while(subpatcher)
        {
            std::vector<object_base*> found_tmp
                = find_children_to_register(caller, subpatcher, search_symbol, true);

            found.insert(found.end(),found_tmp.begin(), found_tmp.end());

            subpatcher = (t_object*)object_method(object, gensym("subpatcher"), idx++, 0);
        }
      }

      next_box = object_attr_getobj(next_box, _sym_nextobject);
    }

    // 3: finally look for ossia.param / ossia.remote in the same pather
    next_box = object_attr_getobj(root_patcher, _sym_firstobject);

    while (next_box)
    {
      if (object_box != next_box)
      {
        t_symbol* current = object_attr_getsym(next_box, _sym_maxclass);
        if (current == subclassname
            || ( !found_view && current == gensym("ossia.remote") ) )
        {

          // the object itself shouln't be stored
          if (object_box != next_box)
          {
            object_base* o = (object_base*) jbox_get_object(next_box);
            found.push_back(o);
          }
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
