#pragma once

#include "ossia_object_base.hpp"

namespace ossia {
namespace max {

# pragma mark -
# pragma mark t_device structure declaration

struct t_device : t_object_base
{
    ossia::net::generic_device* m_device{};
    ossia::net::local_protocol m_local_proto;

    static void register_children(t_device*);
    void unregister_children();
    
    static void loadbang(t_device*);
};
    
# pragma mark -
# pragma mark protocol setting stuctures declaration
    
namespace protocol_settings {
        
struct minuit
{
    std::string remoteip {"locahost"};
    unsigned int remoteport = 13579;
    unsigned int localport = 9998;
};

struct oscquery
{
    unsigned int oscport = 9999;
    unsigned int wsport = 5678;
};

struct osc
{
    std::string remoteip {"localhost"};
    unsigned int remoteport = 9997;
    unsigned int localport = 9996;
};

static void print_protocol_help()
{
    post("expose <protocol> <args> ...");
    post("Available protocols (case sensitive): Minuit, oscquery, osc");
    
    post("Protocols parameters :");
    
    post("Minuit <remoteip> <remoteport> <localport> :\n"
         "\tremoteip (symbol): ip of target device\n"
         "\tremoteport (float): port on which packet should be send\n"
         "\tlocalport (float): port to which this device is listening");
    
    post("oscquery <oscport> <wsport> :\n"
         "\toscport (float) : post on which osc packet are sent\n"
         "\twsport (float) : WebSocket port on which distant application will connect.");
    
    post("osc <remoteip> <remoteport> <localpoort>\n"
         "\tremoteip (symbol): ip of target device\n"
         "\tremoteport (float): port on which packet should be send\n"
         "\tlocalport (port): port this device is listening.");
}

} // protocol_setting namespace
} // max namespace
} // ossia namespace

# pragma mark -
# pragma mark ossia_client class declaration

extern "C"
{
    void* ossia_device_new(t_symbol*, long, t_atom*);
    void ossia_device_free(ossia::max::t_device*);
    void ossia_device_dump(ossia::max::t_device*);
    void ossia_device_expose(ossia::max::t_device*, t_symbol*, long, t_atom*);
}
