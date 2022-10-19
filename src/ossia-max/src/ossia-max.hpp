#pragma once
#define OSSIA_MAX_AUTOREGISTER 1
#include "ext.h"
#include "ext_obex.h"
#include "jpatcher_api.h"

#include <ossia/detail/config.hpp>
#undef error
#undef post

#include "ZeroconfMinuitListener.hpp"
#include "ZeroconfOscqueryListener.hpp"
#include "assert.hpp"
#include "attribute.hpp"
#include "client.hpp"
#include "device.hpp"
#include "explorer.hpp"
#include "fuzzysearch.hpp"
#include "logger.hpp"
#include "model.hpp"
#include "monitor.hpp"
#include "ossia_object.hpp"
#include "parameter.hpp"
#include "remote.hpp"
#include "router.hpp"
#include "search.hpp"
#include "view.hpp"

#include <ossia/detail/safe_vec.hpp>
#include <ossia/network/common/websocket_log_sink.hpp>

#include <ossia-max/src/object_base.hpp>

#include <ossia-max_export.h>

extern "C" {
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
OSSIA_MAX_EXPORT void ossia_search_setup();
OSSIA_MAX_EXPORT void ossia_monitor_setup();
OSSIA_MAX_EXPORT void ossia_fuzzysearch_setup();
OSSIA_MAX_EXPORT void ossia_assert_setup();
OSSIA_MAX_EXPORT void ossia_equals_setup();
}

namespace ossia
{
namespace max_binding
{

#pragma mark -
#pragma mark Library

struct max_msp_log_sink final : public spdlog::sinks::sink
{
  void log(const spdlog::details::log_msg& msg) override
  {
    std::string s(msg.payload.data(), msg.payload.size());
    switch(msg.level)
    {
      case spdlog::level::warn:
      case spdlog::level::err: {
        error("%s", s.c_str());
        break;
      }

      default:
        post("%s", s.c_str());
        break;
    }
  }

  void flush() override { }

  void set_pattern(const std::string& pattern) override { }
  void set_formatter(std::unique_ptr<spdlog::formatter> sink_formatter) override { }
};

struct patcher_descriptor
{
  ossia::safe_set<parameter*> parameters{};
  ossia::safe_set<remote*> remotes{};
  ossia::safe_set<attribute*> attributes{};
  model* model{};
  view* view{};
  device* device{};
  client* client{};

  t_object* parent_patcher{};
  ossia::safe_set<t_object*> subpatchers{};

  bool loadbanged{}; // true if patcher have been loadbanged already

  long poly_index{
      -1}; // 0 when not in a poly~, instance index otherwise, -1 uninitialized

  bool empty() const
  {
    return parameters.empty() && remotes.empty() && attributes.empty()
           && model != nullptr && view != nullptr && device != nullptr
           && client != nullptr;
  }

  auto size() const
  {
    return parameters.size() + remotes.size() + attributes.size() + (model ? 1 : 0)
           + (view ? 1 : 0) + (device ? 1 : 0) + (client ? 1 : 0);
  }

  bool has_no_master_node()
  {
    return model == nullptr && view == nullptr && device == nullptr && client == nullptr;
  }
};

struct configuration
{
  bool defer_by_default = true;
};

class ossia_max
{
public:
  static ossia_max& instance();
  static const std::shared_ptr<ossia::net::generic_device>& get_default_device()
  {
    return instance().m_device;
  }

  // static void register_nodes(ossia_max* x);
  static void discover_network_devices(ossia_max* x);

  template <typename T>
  t_class* get_class()
  {
    if(std::is_same<T, parameter>::value)
      return ossia_parameter_class;
    if(std::is_same<T, remote>::value)
      return ossia_remote_class;
    if(std::is_same<T, model>::value)
      return ossia_model_class;
    if(std::is_same<T, view>::value)
      return ossia_view_class;
    if(std::is_same<T, device>::value)
      return ossia_device_class;
    if(std::is_same<T, client>::value)
      return ossia_client_class;
    if(std::is_same<T, attribute>::value)
      return ossia_attribute_class;
    if(std::is_same<T, ossia_object>::value)
      return ossia_ossia_class;
    if(std::is_same<T, ossia::max_binding::logger>::value)
      return ossia_logger_class;
    if(std::is_same<T, ossia::max_binding::explorer>::value)
      return ossia_explorer_class;
    if(std::is_same<T, ossia::max_binding::monitor>::value)
      return ossia_monitor_class;
    if(std::is_same<T, ossia::max_binding::search>::value)
      return ossia_search_class;
    if(std::is_same<T, ossia::max_binding::router>::value)
      return ossia_router_class;
    if(std::is_same<T, ossia::max_binding::fuzzysearch>::value)
      return ossia_fuzzysearch_class;
    if(std::is_same<T, ossia::max_binding::oassert>::value)
      return ossia_assert_class;
    return nullptr;
  }

  void set_log_level(t_symbol* log_sym)
  {
    std::vector<spdlog::string_view_t> vec SPDLOG_LEVEL_NAMES;
    auto it = std::find(vec.begin(), vec.end(), log_sym->s_name);
    if(it != vec.end())
    {
      int level = it - vec.begin();
      m_log_sink.get()->set_level(static_cast<spdlog::level::level_enum>(level));
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
  t_class* ossia_assert_class{};
  t_class* ossia_equals_class{};
  static t_class* ossia_patcher_listener_class;

  // keep list of all objects
  // TODO is it still needed ?
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
  ossia::safe_vector<logger*> loggers;
  ossia::safe_vector<oassert*> oasserts;
/*
  // TODO remove all those nr* vectors, should not be needed anymore
  // list of non-registered objects
  ossia::safe_set<parameter*> nr_parameters;
  ossia::safe_set<remote*> nr_remotes;
  ossia::safe_set<model*> nr_models;
  ossia::safe_set<view*> nr_views;
  ossia::safe_set<device*> nr_devices;
  ossia::safe_set<client*> nr_clients;
  ossia::safe_set<attribute*> nr_attributes;
  ossia::safe_set<monitor*> nr_monitors;
*/
  static std::map<ossia::net::node_base*, ossia::safe_set<matcher*>> s_node_matchers_map;
  static std::recursive_mutex s_node_matchers_mut;

  // TODO is this still needed ?
  bool registering_nodes = false;

  std::map<t_object*, patcher_descriptor> patchers;

  static patcher_descriptor& get_patcher_descriptor(t_object* patcher);
  static void remove_patcher_descriptor(t_object* patcher);

  void* m_reg_clock{};
  static void* s_browse_clock;

  static ZeroconfOscqueryListener s_zeroconf_oscq_listener;
  static ZeroconfMinuitListener s_zeroconf_minuit_listener;

  configuration config;

private:
  ossia_max();
  ~ossia_max();
  ossia_max(const ossia_max&) = delete;
  ossia_max(ossia_max&&) = delete;

  ossia::net::local_protocol* m_localProtocol{};
  std::shared_ptr<ossia::net::generic_device> m_device;
  string_map<std::shared_ptr<ossia::websocket_threaded_connection>> m_connections;
  std::shared_ptr<max_msp_log_sink> m_log_sink;

  t_object* m_patcher_listener;
};

template <typename T, typename... Args>
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
