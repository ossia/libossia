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

extern "C"
{
    OSSIA_MAX_EXPORT void ossia_attribute_setup();
    OSSIA_MAX_EXPORT void ossia_client_setup();
    OSSIA_MAX_EXPORT void ossia_device_setup();
    OSSIA_MAX_EXPORT void ossia_logger_setup();
    OSSIA_MAX_EXPORT void ossia_model_setup();
    OSSIA_MAX_EXPORT void ossia_parameter_setup();
    OSSIA_MAX_EXPORT void ossia_remote_setup();
    OSSIA_MAX_EXPORT void ossia_view_setup();
    OSSIA_MAX_EXPORT void ossia_ossia_setup();
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
        std::string s(msg.raw.data(), msg.raw.size());
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
  static ossia::net::generic_device* get_default_device()
  {
    return &instance().m_device;
  }

  static void register_nodes(void* x);

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

  t_class* ossia_client_class{};
  t_class* ossia_attribute_class{};
  t_class* ossia_device_class{};
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

  ossia::safe_set<model*> model_quarantine;
  ossia::safe_set<view*> view_quarantine;
  ossia::safe_set<parameter*> parameter_quarantine;
  ossia::safe_set<remote*> remote_quarantine;
  ossia::safe_set<attribute*> attribute_quarantine;

  // this is used at loadbang to mark a patcher loaded
  // and trigger its registration
  struct root_descriptor{
    bool is_loadbanged{};
    unsigned long count{}; // number of object under this root

    unsigned long inc(){ return ++count;}
    unsigned long dec(){ return --count;}
  };

  typedef std::map<t_object*, root_descriptor> RootMap;

  RootMap root_patcher;
  void* m_reg_clock{};

private:
  ossia_max();
  ~ossia_max();

  ossia::net::local_protocol* m_localProtocol{};
  ossia::net::generic_device m_device;
  string_map<std::shared_ptr<ossia::websocket_threaded_connection>> m_connections;
  std::shared_ptr<max_msp_log_sink> m_log_sink;
};

#pragma mark -
#pragma mark Templates

template <typename T>
extern void object_quarantining(T*);

template <typename T>
extern void object_dequarantining(T*);

template <typename T>
extern bool object_is_quarantined(T*);

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
 * @return std::vector<t_pd*> containing pointer to t_pd struct of the
 * corresponding classname
 */
std::vector<object_base*> find_children_to_register(
    t_object* object, t_object* patcher, t_symbol* classname, bool search_dev = false);

/**
 * @brief             Convenient method to easily get the patcher where a box
 * is
 */
t_object* get_patcher(t_object* object);

/**
 * @brief return relative path to corresponding object
 * @param x
 * @param classname
 * @param found_obj
 * @param address
 * @return
 */
//    bool get_relative_path(t_object* x, t_symbol* classname, t_class**
//    found_obj, std::string& address);

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
