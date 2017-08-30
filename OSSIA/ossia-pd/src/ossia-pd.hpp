#pragma once
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/detail/safe_vec.hpp>

#include "parameter.hpp"
#include "model.hpp"
#include "remote.hpp"
#include "view.hpp"
#include "device.hpp"
#include "client.hpp"

extern "C" {
#include <cicm_wrapper.h>
}

namespace ossia
{
struct websocket_threaded_connection;
namespace pd
{

extern "C" void setup_ossia0x2eclient(void);
extern "C" void setup_ossia0x2edevice(void);
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

    t_eclass* client{};
    t_eclass* device{};
    t_eclass* logger{};
    t_eclass* model{};
    t_eclass* param{};
    t_eclass* remote{};
    t_eclass* view{};
    t_eclass* ossia{};

    ossia::safe_vector<t_param*> params;
    ossia::safe_vector<t_remote*> remotes;
    ossia::safe_vector<t_model*> models;
    ossia::safe_vector<t_view*> views;
    ossia::safe_vector<t_device*> devices;
    ossia::safe_vector<t_client*> clients;

    ossia::safe_vector<t_select_clock*> select_clocks;

    t_symbol* sym_addr;


    ossia::safe_vector<t_model*> model_quarantine;
    ossia::safe_vector<t_view*> view_quarantine;
    ossia::safe_vector<t_param*> parameter_quarantine;
    ossia::safe_vector<t_remote*> remote_quarantine;

private:
    ossia_pd(); // constructor
    ~ossia_pd(); // destructor

    ossia::net::local_protocol* m_localProtocol{};
    ossia::net::generic_device m_device;
    string_map<std::shared_ptr<ossia::websocket_threaded_connection>> m_connections;
};

}
} // namespace
