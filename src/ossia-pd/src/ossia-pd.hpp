#pragma once
#include "ZeroconfMinuitListener.hpp"
#include "ZeroconfOscqueryListener.hpp"

#include <ossia/detail/safe_vec.hpp>
#include <ossia/network/common/websocket_log_sink.hpp>
#include <ossia/network/generic/generic_device.hpp>

#include <ossia-pd/src/attribute.hpp>
#include <ossia-pd/src/client.hpp>
#include <ossia-pd/src/device.hpp>
#include <ossia-pd/src/model.hpp>
#include <ossia-pd/src/parameter.hpp>
#include <ossia-pd/src/remote.hpp>
#include <ossia-pd/src/view.hpp>

extern "C" {
#include <cicm_wrapper.h>
}

namespace ossia
{
struct websocket_threaded_connection;
namespace pd
{

struct pd_log_sink final : public spdlog::sinks::sink
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

extern "C" void setup_ossia0x2eassert(void);
extern "C" void setup_ossia0x2eattribute(void);
extern "C" void setup_ossia0x2eclient(void);
extern "C" void setup_ossia0x2edevice(void);
extern "C" void setup_ossia0x2eexplorer(void);
extern "C" void setup_ossia0x2elogger(void);
extern "C" void setup_ossia0x2emodel(void);
extern "C" void setup_ossia0x2eparam(void);
extern "C" void setup_ossia0x2eremote(void);
extern "C" void setup_ossia0x2eview(void);

class ossia_pd
{
public:
  static ossia_pd& instance();
  static ossia::net::generic_device* get_default_device()
  {
    return &instance().m_device;
  }
  static void register_nodes(void* x);
  static void discover_network_devices(ossia_pd* x);

  void set_log_level(t_symbol* log_sym)
  {
    std::vector<spdlog::string_view_t> vec = SPDLOG_LEVEL_NAMES;
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

  static t_eclass* attribute_class;
  static t_eclass* client_class;
  static t_eclass* device_class;
  static t_eclass* explorer_class;
  static t_eclass* logger_class;
  static t_eclass* model_class;
  static t_eclass* param_class;
  static t_eclass* remote_class;
  static t_eclass* view_class;
  static t_eclass* ossia_class;

  static t_symbol* o_sym_attribute;
  static t_symbol* o_sym_client;
  static t_symbol* o_sym_device;
  static t_symbol* o_sym_logger;
  static t_symbol* o_sym_model;
  static t_symbol* o_sym_param;
  static t_symbol* o_sym_remote;
  static t_symbol* o_sym_view;
  static t_symbol* o_sym_ossia;
  static t_symbol* o_sym_set;
  static t_symbol* o_sym_address;

  ossia::safe_vector<attribute*> attributes;
  ossia::safe_vector<parameter*> parameters;
  ossia::safe_vector<remote*> remotes;
  ossia::safe_vector<model*> models;
  ossia::safe_vector<view*> views;
  ossia::safe_vector<device*> devices;
  ossia::safe_vector<client*> clients;

  ossia::safe_vector<t_select_clock*> select_clocks;

  ossia::safe_set<attribute*> attribute_quarantine;
  ossia::safe_set<model*> model_quarantine;
  ossia::safe_set<view*> view_quarantine;
  ossia::safe_set<parameter*> parameter_quarantine;
  ossia::safe_set<remote*> remote_quarantine;

  // this is used at loadbang to mark a patcher loaded
  // and trigger its registration
  struct root_descriptor
  {
    bool is_loadbanged{};
    unsigned long count{}; // number of object under this root

    unsigned long inc() { return ++count; }
    unsigned long dec() { return --count; }
  };

  typedef std::map<t_canvas*, root_descriptor> RootMap;

  RootMap m_root_patcher{};
  t_clock* m_reg_clock{};
  static t_clock* browse_clock;

  bool m_testing{};

  static ZeroconfOscqueryListener zeroconf_oscq_listener;
  static ZeroconfMinuitListener zeroconf_minuit_listener;

private:
  ossia_pd();  // constructor
  ~ossia_pd(); // destructor

  ossia::net::local_protocol* m_localProtocol{};
  ossia::net::generic_device m_device;
  string_map<std::shared_ptr<ossia::websocket_threaded_connection>> m_connections;
  std::shared_ptr<pd_log_sink> m_log_sink;
};

}
} // namespace
