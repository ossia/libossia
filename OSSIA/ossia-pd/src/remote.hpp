#pragma once
#include "ossia-pd.hpp"
#include "device.hpp"
#include <boost/optional.hpp>

using namespace ossia;

struct t_remote
{
    t_eobj      x_obj;

    t_symbol*   x_name;
    t_outlet*   x_setout;
    t_outlet*   x_dataout;
    t_outlet*   x_dumpout;
    bool        x_absolute;

    ossia::net::node_base* x_node;
    void setValue(const ossia::value& val);
    bool register_node(ossia::net::node_base* node);
    bool unregister();

    boost::optional<ossia::callback_container<ossia::value_callback>::iterator> x_callbackit;
    void error(){    logpost((t_object*)this,4,"[ossia.remote %s] is not registered to any parameter",x_name->s_name); }

    void addressRemovingHandler(const ossia::net::address_base& address) {
        x_callbackit = boost::none;
        unregister();
    };

    void quarantining();
    void dequarantining();

    static std::vector<t_remote*>& remote_quarantine(){
        static std::vector<t_remote*> quarantine;
        return quarantine;
    }
};
