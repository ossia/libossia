// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <ossia/detail/config.hpp>

#include <ossia/context.hpp>
#include <ossia/network/base/node.hpp>
#include <ossia/network/base/node_functions.hpp>

#include <ossia-max/src/ossia-max.hpp>
#include <ossia-max/src/utils.hpp>

#include <rapidjson/istreamwrapper.h>

#include <commonsyms.h>
#include <git_info.h>

#include <codecvt>
#include <fstream>
#include <istream>
#include <locale>
#include <string>

#if defined(_WIN32)
#if defined(LF_FACESIZE)
#undef NOGDI
#undef LF_FACESIZE
#endif
#include <Shtypes.h>
#include <shlobj.h>
#include <shlwapi.h>
#endif
using namespace ossia::max_binding;

void* ossia_max::s_browse_clock;
ZeroconfOscqueryListener ossia_max::s_zeroconf_oscq_listener;
ZeroconfMinuitListener ossia_max::s_zeroconf_minuit_listener;
ossia::hash_map<ossia::net::node_base*, ossia::safe_set<matcher*>>
    ossia_max::s_node_matchers_map{};
std::recursive_mutex ossia_max::s_node_matchers_mut;
t_class* ossia_max::ossia_patcher_listener_class;

void patcher_listener_notify(
    t_object* x, t_symbol* s, t_symbol* msg, t_object* sender, void* data)
{
  if((msg == gensym("willfree")) || (msg == gensym("free")))
  {
    ossia_max::remove_patcher_descriptor(sender);
  }
}

void patcher_listener_free(t_object* x)
{
  object_unregister(x);
}

void* patcher_listener_new()
{
  return (t_object*)object_alloc(ossia_max::ossia_patcher_listener_class);
}

static configuration read_configuration_file()
{
  using namespace std;
  using namespace std::literals;
  using namespace rapidjson;

// Open the config file
#if defined(__APPLE__)
  std::string path
      = getenv("HOME") + "/Library/Application Support/ossia-max/ossia-max.conf"s;
#elif defined(_WIN32)
  wchar_t szPath[MAX_PATH] = {0};
  if(!SUCCEEDED(SHGetFolderPathW(nullptr, CSIDL_APPDATA, nullptr, 0, szPath)))
  {
    return {};
  }
  std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
  std::string path = converter.to_bytes(szPath) + "\\ossia-max\\ossia-max.conf";
#endif

  std::fstream fs;
  fs.open(path, std::ios_base::in);

  if(!fs.is_open())
    return {};

  // Parse it with rapidjson
  IStreamWrapper isw{fs};
  Document doc;
  doc.ParseStream(isw);

  if(!doc.IsObject())
    return {};

  configuration conf;
  auto read_bool = [&](auto member, std::string_view name) {
    if(auto it = doc.FindMember(name.data()); it != doc.MemberEnd())
    {
      if(it->value.IsBool())
      {
        conf.*member = it->value.GetBool();
      }
    }
  };

  read_bool(&configuration::defer_by_default, "default_deferlow");
  read_bool(&configuration::autoregister, "autoregister");

  return conf;
}

// ossia-max library constructor
ossia_max::ossia_max()
    : m_localProtocol{new ossia::net::local_protocol}
    , m_device{std::make_shared<ossia::net::generic_device>(
          std::unique_ptr<ossia::net::protocol_base>(m_localProtocol),
          "ossia_max_device")}
    , m_log_sink{std::make_shared<max_msp_log_sink>()}
{
  m_log_sink.get()->set_level(spdlog::level::err);
  ossia::context c{{m_log_sink}};
  common_symbols_init();

  m_device->on_attribute_modified
      .connect<&device_base::on_attribute_modified_callback>();

  remotes.reserve(1024);
  views.reserve(512);
  devices.reserve(8);
  clients.reserve(8);

  s_browse_clock = clock_new(this, (method)ossia_max::discover_network_devices);
  clock_delay(ossia_max::s_browse_clock, 100.);

  ossia_patcher_listener_class = class_new(
      "ossia.patcher_listener", (method)patcher_listener_new,
      (method)patcher_listener_free, sizeof(t_object), 0L, A_CANT, 0);
  class_addmethod(
      ossia_patcher_listener_class, (method)patcher_listener_notify, "notify", 0);
  class_register(CLASS_NOBOX, ossia_patcher_listener_class);

  m_patcher_listener
      = (t_object*)object_new(CLASS_NOBOX, gensym("ossia.patcher_listener"), nullptr);

  // Load the configuration file if any
  this->config = read_configuration_file();

  post("OSSIA library for Max is loaded");
  post("build SHA : %s", ossia::get_commit_sha().c_str());
}

// ossia-max library destructor
ossia_max::~ossia_max()
{
  /*
  m_device->on_attribute_modified
      .disconnect<&device_base::on_attribute_modified_callback>();

  for(auto x : devices.copy())
  {
    auto& multiplex
        = static_cast<ossia::net::multiplex_protocol&>(x->m_device->get_protocol());
    auto& protos = multiplex.get_protocols();
    for(auto& proto : protos)
    {
      multiplex.stop_expose_to(*proto);
    }
    x->disconnect_slots();
  }
  for(auto x : views.copy())
  {
    x->m_matchers.clear();
  }
  for(auto x : remotes.copy())
  {
    x->m_matchers.clear();
  }
  for(auto x : models.copy())
  {
    x->m_matchers.clear();
  }
  for(auto x : parameters.copy())
  {
    x->m_matchers.clear();
  }
  */
}

// ossia-max library instance
ossia_max& ossia_max::instance()
{
  static ossia_max library_instance;
  return library_instance;
}

patcher_descriptor& ossia_max::get_patcher_descriptor(t_object* patcher)
{
  auto& map = ossia_max::instance().patchers;
  auto it = map.find(patcher);
  if(it != map.end())
  {
    object_attach_byptr_register(
        ossia_max::instance().m_patcher_listener, patcher, CLASS_NOBOX);
  }

  return map[patcher];
}

void ossia_max::remove_patcher_descriptor(t_object* patcher)
{
  auto& map = ossia_max::instance().patchers;

  auto it = map.find(patcher);
  if(it != map.end())
  {
    auto parent = it->second.parent_patcher;
    if(parent)
    {
      auto parent_it = map.find(parent);
      if(parent_it != map.end())
      {
        parent_it->second.subpatchers.remove_all(patcher);
      }
    }
    map.erase(it);
  }
}

template <typename T>
void fill_nr_vector(const ossia::safe_vector<T*>& safe, ossia::safe_set<T*>& nr)
{
  nr.clear();
  nr.reserve(safe.size());
  for(auto ptr : safe.reference())
    nr.push_back(ptr);
};

template <typename T>
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

  ossia::sort(list, [&](T* a, T* b) {
    return a->m_patcher_hierarchy.size() < b->m_patcher_hierarchy.size();
  });

  return list;
}

namespace ossia
{
namespace max_binding
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

  while(next_box)
  {
    if(next_box != object_box)
    {
      object_base* object = (object_base*)jbox_get_object(next_box);

      t_symbol* curr_classname = object_attr_getsym(next_box, _sym_maxclass);
      if(curr_classname == search_symbol)
      {

        // ignore dying object
        if(!object->m_dead)
          found.push_back(object);
      }

      // if we're looking for ossia.view but found a model, remind it
      if(search_symbol == gensym("ossia.model")
         && curr_classname == gensym("ossia.view"))
        found_view = true;

      // if there is a client or device in the current patcher
      // return only that object
      if(search_dev
         && (curr_classname == gensym("ossia.device")
             || curr_classname == gensym("ossia.client")))
      {
        // ignore dying object
        if(!object->m_dead)
          return {object};
      }
    }
    next_box = object_attr_getobj(next_box, _sym_nextobject);
  }

  // 2: if there is no ossia.model / ossia.view in the current patch, look into
  // the subpatches
  if(found.empty())
  {
    next_box = object_attr_getobj(root_patcher, _sym_firstobject);

    while(next_box)
    {
      t_object* object = jbox_get_object(next_box);
      t_symbol* classname = object_classname(object);

      // jpatcher or bpatcher case
      if(classname == _sym_jpatcher || classname == _sym_bpatcher)
      {
        std::vector<object_base*> found_tmp
            = find_children_to_register(caller, object, search_symbol, true);

        found.insert(found.end(), found_tmp.begin(), found_tmp.end());
      }
      else if(classname == gensym("poly~"))
      {
        long idx = 0;
        t_object* subpatcher
            = (t_object*)object_method(object, gensym("subpatcher"), idx++, 0);
        while(subpatcher)
        {
          std::vector<object_base*> found_tmp
              = find_children_to_register(caller, subpatcher, search_symbol, true);

          found.insert(found.end(), found_tmp.begin(), found_tmp.end());

          subpatcher = (t_object*)object_method(object, gensym("subpatcher"), idx++, 0);
        }
      }

      next_box = object_attr_getobj(next_box, _sym_nextobject);
    }

    // 3: finally look for ossia.param / ossia.remote in the same pather
    next_box = object_attr_getobj(root_patcher, _sym_firstobject);

    while(next_box)
    {
      if(object_box != next_box)
      {
        t_symbol* current = object_attr_getsym(next_box, _sym_maxclass);
        if(current == subclassname || (!found_view && current == gensym("ossia.remote")))
        {

          // the object itself shouldn't be stored
          if(object_box != next_box)
          {
            object_base* o = (object_base*)jbox_get_object(next_box);
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
