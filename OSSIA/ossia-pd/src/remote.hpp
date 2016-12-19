#pragma once
#include "ossia_obj_base.hpp"
#include "device.hpp"
#include <boost/optional.hpp>

using namespace ossia;

struct t_remote : ossia_obj_base
{
    bool register_node(ossia::net::node_base* node);
    bool unregister();
    // void isDeleted(const ossia::net::node_base& n);

    boost::optional<ossia::callback_container<ossia::value_callback>::iterator> x_callbackit;

    void addressRemovingHandler(const ossia::net::address_base& address) {
        x_callbackit = boost::none;
        unregister();
    };

    void isDeleted(const ossia::net::node_base& n){
        x_callbackit = boost::none;
        unregister();
    }

    void quarantining();
    void dequarantining();

    static std::vector<t_remote*>& remote_quarantine(){
        static std::vector<t_remote*> quarantine;
        return quarantine;
    }
};

static void remote_loadbang(t_remote *x){
    bool res = obj_register<t_remote>(x);
    // if (res) remote_bang(x); // if correctly registered then pull the value
}
