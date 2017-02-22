#pragma once

#include "ossia-pd.hpp"
#include "ossia_obj_base.hpp"

namespace ossia { namespace pd {

struct Protocol_Settings {
    struct t_minuit {
        std::string remoteip{"locahost"};
        unsigned int remoteport=13579;
        unsigned int localport=9998;
    };

    struct t_oscquery {
        unsigned int oscport=9999;
        unsigned int wsport=5678;
    };

    struct t_osc {
        std::string remoteip{"localhost"};
        unsigned int remoteport=9997;
        unsigned int localport=9996;
    };

    t_minuit minuit;
    t_oscquery oscquery;
    t_osc osc;

    static void print_protocol_help(){
        post("Available protocols (case sensitive): Minuit, oscquery, osc");
        post("Protocols parameters :");
        post("Minuit :\n"
             "\tremoteip : ip of target device\n"
             "\tremoteport: port on which packet should be send\n"
             "\tlocalport: port this device is listening to");
    }
};

struct t_device : ossia_obj_base
{
    ossia::net::generic_device* x_device{};
    ossia::net::local_protocol x_local_proto;

    void register_children();
    void unregister_children();

    Protocol_Settings x_settings;

    // void addressCreationHandler(const ossia::net::address_base& n);
    // void nodeCreationHandler(const ossia::net::node_base& n);
};

} } // namespace
