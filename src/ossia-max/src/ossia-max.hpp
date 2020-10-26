#pragma once
#define OSSIA_MAX_AUTOREGISTER 1
#include <ossia/detail/config.hpp>
#include "ext.h"
#include "ext_obex.h"
#include "jpatcher_api.h"
#undef error
#undef post

#include <ossia-max/src/object_base.hpp>

#include <ossia/network/common/websocket_log_sink.hpp>
#include <ossia/detail/safe_vec.hpp>
#include <ossia-max_export.h>

#include "attribute.hpp"
#include "parameter.hpp"
#include "model.hpp"
#include "remote.hpp"
#include "view.hpp"
#include "device.hpp"
#include "client.hpp"
#include "ossia_object.hpp"
#include "logger.hpp"
#include "explorer.hpp"
#include "monitor.hpp"
#include "search.hpp"
#include "router.hpp"
#include "fuzzysearch.hpp"

#include "ZeroconfOscqueryListener.hpp"
#include "ZeroconfMinuitListener.hpp"

extern "C"
{
    OSSIA_MAX_EXPORT void ossia_router_setup();
    OSSIA_MAX_EXPORT void ossia_attribute_setup();
    OSSIA_MAX_EXPORT void ossia_client_setup();
    OSSIA_MAX_EXPORT void ossia_device_setup();
    OSSIA_MAX_EXPORT void ossia_logger_setup();
    OSSIA_MAX_EXPORT void ossia_model_setup();
    OSSIA_MAX_EXPORT void ossia_parameter_setup();
    OSSIA_MAX_EXPORT void ossia_remote_setup();
    OSSIA_MAX_EXPORT void ossia_view_setup();
    OSSIA_MAX_EXPORT void ossia_ossia_setup();
    OSSIA_MAX_EXPORT void ossia_explorer_setup();
}

namespace ossia
{
namespace max
{

#pragma mark -
#pragma mark Library

struct max_msp_log_sink final :
        public spdlog::sinks::sink
{
    void log(const spdlog::details::log_msg& msg) override
    {
        std::string s(msg.payload.data(), msg.payload.size());
        switch(msg.level)
        {
          case spdlog::level::warn:
          case spdlog::level::err:
          {
            error("%s", s.c_str());
            break;
          }

          default:
            post("%s", s.c_str());
            break;
          }
      }

    void flush() override
    {
    }

    void set_pattern(const std::string &pattern) override { }
    void set_formatter(std::unique_ptr<spdlog::formatter> sink_formatter) override { }
};

class ossia_max
{
public:
  static ossia_max& instance();
  static const std::shared_ptr<ossia::net::generic_device>& get_default_device()
  {
    return instance().m_device;
  }

  static void register_nodes(ossia_max* x);
  static void poll_all_queues(ossia_max* x);
  static void discover_network_devices(ossia_max* x);

  template<typename T>
  t_class* get_class() {
    if(std::is_same<T, parameter>::value) return ossia_parameter_class;
    if(std::is_same<T, remote>::value) return ossia_remote_class;
    if(std::is_same<T, model>::value) return ossia_model_class;
    if(std::is_same<T, view>::value) return ossia_view_class;
    if(std::is_same<T, device>::value) return ossia_device_class;
    if(std::is_same<T, client>::value) return ossia_client_class;
    if(std::is_same<T, attribute>::value) return ossia_attribute_class;
    if(std::is_same<T, ossia_object>::value) return ossia_ossia_class;
    if(std::is_same<T, ossia::max::logger>::value) return ossia_logger_class;
    if(std::is_same<T, ossia::max::explorer>::value) return ossia_explorer_class;
    if(std::is_same<T, ossia::max::monitor>::value) return ossia_monitor_class;
    if(std::is_same<T, ossia::max::search>::value) return ossia_search_class;
    if(std::is_same<T, ossia::max::router>::value) return ossia_router_class;
    if(std::is_same<T, ossia::max::fuzzysearch>::value) return ossia_fuzzysearch_class;
    return nullptr;
  }

  void set_log_level(t_symbol* log_sym)
  {
    std::vector<std::string> vec = SPDLOG_LEVEL_NAMES;
    auto it = std::find(vec.begin(), vec.end(), log_sym->s_name);
    if(it != vec.end())
    {
      int level = it - vec.begin();
      m_log_sink.get()->set_level(
                  static_cast<spdlog::level::level_enum>(level));
    }
    else
    {
      error("Unknown log level : %s", log_sym->s_name);
    }
  }

  t_class* ossia_router_class{};
  t_class* ossia_client_class{};
  t_class* ossia_attribute_class{};
  t_class* ossia_device_class{};
  t_class* ossia_explorer_class{};
  t_class* ossia_monitor_class{};
  t_class* ossia_search_class{};
  t_class* ossia_fuzzysearch_class{};
  t_class* ossia_logger_class{};
  t_class* ossia_model_class{};
  t_class* ossia_parameter_class{};
  t_class* ossia_remote_class{};
  t_class* ossia_view_class{};
  t_class* ossia_ossia_class{};

  // keep list of all objects
  ossia::safe_vector<parameter*> parameters;
  ossia::safe_vector<remote*> remotes;
  ossia::safe_vector<model*> models;
  ossia::safe_vector<view*> views;
  ossia::safe_vector<device*> devices;
  ossia::safe_vector<client*> clients;
  ossia::safe_vector<attribute*> attributes;
  ossia::safe_vector<explorer*> explorers;
  ossia::safe_vector<monitor*> monitors;
  ossia::safe_vector<search*> searchs;

  // list of non-registered objects
  ossia::safe_set<parameter*> nr_parameters;
  ossia::safe_set<remote*> nr_remotes;
  ossia::safe_set<model*> nr_models;
  ossia::safe_set<view*> nr_views;
  ossia::safe_set<device*> nr_devices;
  ossia::safe_set<client*> nr_clients;
  ossia::safe_set<attribute*> nr_attributes;
  ossia::safe_set<monitor*> nr_monitors;

  bool registering_nodes=false;

  // this is used at loadbang to mark a patcher loaded
  // and trigger its registration
  struct root_descriptor{
    bool is_loadbanged{};
    unsigned long count{}; // number of object under this root

    unsigned long inc(){ return ++count;}
    unsigned long dec(){ return --count;}
  };

  typedef std::map<t_object*, root_descriptor> RootMap;

  struct patcher_descriptor{
    ossia::safe_vector<parameter*> parameters{};
    ossia::safe_vector<remote*>    remotes{};
    ossia::safe_vector<attribute*> attributes{};
    ossia::safe_vector<model*>     models{};
    ossia::safe_vector<view*>      views{};
    ossia::safe_vector<device*>    devices{};
    ossia::safe_vector<client*>    clients{};

    t_object* parent_patcher;
    ossia::safe_set<t_object*> subpatchers;

    bool loadbanged; // true if patcher have been loadbanged already

    bool empty() const
    {
      return parameters.empty()
             && remotes.empty()
             && attributes.empty()
             && models.empty()
             && views.empty()
             && devices.empty()
             && clients.empty();
    }

  };
  std::map<t_object*, patcher_descriptor> patchers;

  RootMap root_patcher;
  void* m_reg_clock{};
  static void* browse_clock;

  static ZeroconfOscqueryListener zeroconf_oscq_listener;
  static ZeroconfMinuitListener   zeroconf_minuit_listener;

private:
  ossia_max();
  ~ossia_max();

  ossia::net::local_protocol* m_localProtocol{};
  std::shared_ptr<ossia::net::generic_device> m_device;
  string_map<std::shared_ptr<ossia::websocket_threaded_connection>> m_connections;
  std::shared_ptr<max_msp_log_sink> m_log_sink;
};

#pragma mark -
#pragma mark Templates

struct object_base;

void object_namespace(object_base* x);

#pragma mark -
#pragma mark Utilities

/**
 * @brief register_quarantinized Try to register all quarantinized objects
 */
void register_quarantinized();

/**
 * @brief Find all objects [classname] in the current patcher.
 * @param patcher : patcher in which we are looking for objects
 * @param classname : name of the object to search (ossia.model or ossia.view)
 * @return std::vector<object_base*> containing pointer to object_base struct of the
 * corresponding classname
 */
std::vector<object_base*> find_children_to_register(
    t_object* object, t_object* patcher, t_symbol* classname, bool search_dev = false);


/**
 * @brief register_objects_in_patcher_recursively : iterate over all patcher's objects and register them one by one recursively
 * @param root_patcher starting patcher
 * @param caller object that calls the function
 * @param matchers vector of t_matcher to register against
 */
void register_objects_in_patcher_recursively(t_object* root_patcher, object_base* caller,
                                             const std::vector<std::shared_ptr<t_matcher>>& matchers);


std::vector<std::shared_ptr<t_matcher>> find_parent_nodes_recursively(t_object* patcher, bool look_for_model_view = true);
/**
 * @brief             Convenient method to easily get the patcher where a box
 * is
 */
t_object* get_patcher(t_object* object);

/**
 */
std::vector<std::string> parse_tags_symbol(t_symbol** tags_symbol, long size);

template<typename T, typename... Args>
T* make_ossia(Args&&... args)
{
  auto obj = object_alloc(ossia_max::instance().get_class<T>());
  if(obj)
  {
    t_object tmp;
    memcpy(&tmp, obj, sizeof(t_object));
    auto x = new(obj) T{std::forward<Args>(args)...};
    memcpy(x, &tmp, sizeof(t_object));

    return x;
  }
  return nullptr;
}

} // max namespace
} // ossia namespace
