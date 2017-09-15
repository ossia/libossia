#pragma once
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/detail/safe_vec.hpp>

#include <ossia-pd/src/attribute.hpp>
#include <ossia-pd/src/parameter.hpp>
#include <ossia-pd/src/model.hpp>
#include <ossia-pd/src/remote.hpp>
#include <ossia-pd/src/view.hpp>
#include <ossia-pd/src/device.hpp>
#include <ossia-pd/src/client.hpp>

extern "C" {
#include <cicm_wrapper.h>
}

namespace ossia
{
struct websocket_threaded_connection;
namespace pd
{

extern "C" void setup_ossia0x2eattribute(void);
extern "C" void setup_ossia0x2eclient(void);
extern "C" void setup_ossia0x2edevice(void);
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

    static t_eclass* attribute_class;
    static t_eclass* client_class;
    static t_eclass* device_class;
    static t_eclass* logger_class;
    static t_eclass* model_class;
    static t_eclass* param_class;
    static t_eclass* remote_class;
    static t_eclass* view_class;
    static t_eclass* ossia_class;

    ossia::safe_vector<attribute*> attributes;
    ossia::safe_vector<parameter*> params;
    ossia::safe_vector<remote*> remotes;
    ossia::safe_vector<model*> models;
    ossia::safe_vector<view*> views;
    ossia::safe_vector<device*> devices;
    ossia::safe_vector<client*> clients;

    ossia::safe_vector<t_select_clock*> select_clocks;

    t_symbol* sym_addr;
    t_symbol* sym_set;

    ossia::safe_set<attribute*> attribute_quarantine;
    ossia::safe_set<model*> model_quarantine;
    ossia::safe_set<view*> view_quarantine;
    ossia::safe_set<parameter*> parameter_quarantine;
    ossia::safe_set<remote*> remote_quarantine;

private:
    ossia_pd(); // constructor
    ~ossia_pd(); // destructor

    ossia::net::local_protocol* m_localProtocol{};
    ossia::net::generic_device m_device;
    string_map<std::shared_ptr<ossia::websocket_threaded_connection>> m_connections;
};

}
} // namespace
