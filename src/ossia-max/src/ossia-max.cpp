// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <ossia/detail/config.hpp>


#include <ossia-max/src/ossia-max.hpp>
#include <ossia-max/src/utils.hpp>
#include <ossia/context.hpp>
#include <ossia/network/base/node.hpp>
#include <ossia/network/base/node_functions.hpp>

#include <commonsyms.h>
#pragma mark -
#pragma mark library

#include <git_info.h>

using namespace ossia::max;

void* ossia_max::s_browse_clock;
ZeroconfOscqueryListener ossia_max::s_zeroconf_oscq_listener;
ZeroconfMinuitListener ossia_max::s_zeroconf_minuit_listener;
std::map<ossia::net::node_base*, ossia::safe_set<matcher*>> ossia_max::s_node_matchers_map{};

// ossia-max library constructor
ossia_max::ossia_max():
    m_localProtocol{new ossia::net::local_protocol},
    m_device{std::make_shared<ossia::net::generic_device>(
        std::unique_ptr<ossia::net::protocol_base>(m_localProtocol), "ossia_max_device")},
    m_log_sink{std::make_shared<max_msp_log_sink>()}
{
  m_log_sink.get()->set_level(spdlog::level::err);
  ossia::context c{{m_log_sink}};
  common_symbols_init();

  m_device->on_attribute_modified.connect<&device_base::on_attribute_modified_callback>();

  parameters.reserve(2048);
  remotes.reserve(1024);
  attributes.reserve(512);
  models.reserve(512);
  views.reserve(512);
  devices.reserve(8);
  clients.reserve(8);
  explorers.reserve(128);
  oasserts.reserve(32);

  s_browse_clock = clock_new(this, (method) ossia_max::discover_network_devices);
  clock_delay(ossia_max::s_browse_clock, 100.);

  post("OSSIA library for Max is loaded");
  post("build SHA : %s", ossia::get_commit_sha().c_str());
}

// ossia-max library destructor
ossia_max::~ossia_max()
{
  m_device->on_attribute_modified.disconnect<&device_base::on_attribute_modified_callback>();

  for (auto x : devices.copy())
  {
    auto& multiplex = static_cast<ossia::net::multiplex_protocol&>(
                x->m_device->get_protocol());
    auto& protos = multiplex.get_protocols();
    for (auto& proto : protos)
    {
      multiplex.stop_expose_to(*proto);
    }
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

template<typename T>
void fill_nr_vector(const ossia::safe_vector<T*>& safe, ossia::safe_set<T*>& nr)
{
  nr.clear();
  nr.reserve(safe.size());
  for(auto ptr : safe.reference())
    nr.push_back(ptr);
};

template<typename T>
std::vector<T*> sort_by_depth(const ossia::safe_set<T*>& safe)
{
  std::vector<T*> list;
  list.reserve(safe.size());
  for(auto pt : safe.reference())
  {
    // need to update hierarchy here because
    // some object might have been inserted
    // after the initialization of some of their children
    // thus children have the wrong hierarchy, so update it
    // before sorting them
    pt->get_hierarchy();
    list.push_back(pt);
  }

  ossia::sort(list, [&](T* a, T*b)
  {
    return a->m_patcher_hierarchy.size() < b->m_patcher_hierarchy.size();
  });

  return list;
}
/*
void ossia_max::register_nodes(ossia_max*)
{
  auto& inst = ossia_max::instance();

  inst.registering_nodes = true;

  std::vector<t_object*> to_be_initialized;


  // we iterate through all objects in the patcher from the top to the bottom
  // registering ossia.device, ossia.client, ossia.model and ossia.parameter
  // remote and view will be registered
  auto& map = inst.root_patcher;
  for (auto it = map.begin(); it != map.end(); it++)
  {
    if(it->second.is_loadbanged)
      continue;

    t_object* patcher = it->first;

    to_be_initialized.push_back(patcher);

    std::vector<std::shared_ptr<matcher>> matchers{
        std::make_shared<matcher>(&ossia_max::instance().get_default_device()->get_root_node(), nullptr)};
    register_children_in_patcher_recursively(patcher, nullptr, matchers);

    // finally rise a flag to mark this patcher loadbangded
    it->second.is_loadbanged = true;
  }

  inst.registering_nodes = false;

  // push default value for all devices
  std::vector<std::shared_ptr<ossia::net::generic_device>> dev_list;
  dev_list.reserve(inst.devices.size() + 1);
  for(auto dev : inst.devices.reference())
  {
    dev_list.push_back(dev->m_device);
  }
  dev_list.push_back(inst.get_default_device());

  ossia::sort(dev_list, [&](std::shared_ptr<ossia::net::generic_device> a,
                            std::shared_ptr<ossia::net::generic_device> b)
  {
    auto prio_a = ossia::net::get_priority(a->get_root_node());
    auto prio_b = ossia::net::get_priority(b->get_root_node());

    if(!prio_a)
      prio_a = 0.;

    if(!prio_b)
      prio_b = 0.;

    return *prio_a > *prio_b;
  });

  for (auto dev : dev_list)
  {
    auto list = ossia::net::list_all_children(&dev->get_root_node());

    for (ossia::net::node_base* child : list)
    {
      if (auto param = child->get_parameter())
      {
        auto val = ossia::net::get_default_value(*child);
        if(val)
        {
          for(auto param : ossia_max::instance().parameters.reference())
          {
            for (auto& m : param->m_matchers)
            {
              if ( m->get_node() == child )
              {
                auto op = static_cast<parameter*>(m->get_parent());
                auto patcher = op->m_patcher_hierarchy.back();
                if(ossia::contains(to_be_initialized,patcher))
                {
                  param->push_parameter_value(child->get_parameter(), *val, false);
                  break;
                }
              }
            }
          }
        }
      }
    }
  }
}
*/

namespace ossia
{
namespace max
{

#pragma mark -
#pragma mark Utilities

std::vector<object_base*> find_children_to_register(
    t_object* caller, t_object* root_patcher, t_symbol* search_symbol, bool search_dev)
{
  t_symbol* subclassname = search_symbol == gensym("ossia.model")
                               ? gensym("ossia.parameter")
                               : gensym("ossia.remote");

  std::vector<object_base*> found;
  bool found_view = false;

  // 1: look for [classname] objects into the patcher
  t_object* next_box = object_attr_getobj(root_patcher, _sym_firstobject);

  t_object* object_box = NULL;
  if(caller)
    object_obex_lookup(caller, gensym("#B"), &object_box);

  while (next_box)
  {
    if(next_box != object_box)
    {
      object_base* object = (object_base*) jbox_get_object(next_box);

      t_symbol* curr_classname = object_attr_getsym(next_box, _sym_maxclass);
      if (curr_classname == search_symbol)
      {

          // ignore dying object
          if (!object->m_dead)
            found.push_back(object);
      }

      // if we're looking for ossia.view but found a model, remind it
      if ( search_symbol == gensym("ossia.model")
              && curr_classname == gensym("ossia.view") )
      found_view = true;

      // if there is a client or device in the current patcher
      // return only that object
      if ( search_dev
           && ( curr_classname == gensym("ossia.device")
             || curr_classname == gensym("ossia.client") ))
      {
        // ignore dying object
        if (!object->m_dead)
          return {object};
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

void ossia_max::discover_network_devices(ossia_max*)
{
  ossia_max::s_zeroconf_oscq_listener.browse();
  ossia_max::s_zeroconf_minuit_listener.browse();
  clock_delay(ossia_max::s_browse_clock, 100.);
}

} // max namespace
} // ossia namespace
