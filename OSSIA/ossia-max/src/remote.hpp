#pragma once
#include "ossia_obj_base.hpp"
#include "device.hpp"
#include <boost/optional.hpp>

namespace ossia { namespace pd {

struct t_remote : t_obj_base
{
    bool register_node(ossia::net::node_base* node);
    bool do_registration(ossia::net::node_base* node);
    bool unregister();

    boost::optional<ossia::callback_container<ossia::value_callback>::iterator> x_callbackit;

    void isDeleted(const ossia::net::node_base& n){
        if (!x_dead){
          x_node = nullptr;
          obj_quarantining<t_remote>(this);
          // FIXME this crash on findChild()
          // obj_register<t_remote>(this);
        }
    }

    static std::vector<t_remote*>& quarantine(){
        static std::vector<t_remote*> quarantine;
        return quarantine;
    }
};

} } // namespace
