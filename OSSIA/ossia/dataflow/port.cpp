#include <ossia/dataflow/port.hpp>
#include <ossia/dataflow/graph.hpp>

namespace ossia
{

void outlet::write(execution_state& e)
{
    if(auto addr_ptr = address.target<ossia::net::address_base*>())
    {
        ossia::net::address_base* addr = *addr_ptr;
        if(scope & port::scope_t::local)
        {
            graph::copy_to_local(data, *addr, e);
        }
        else if(scope & port::scope_t::global)
        {
            graph::copy_to_global(data, *addr, e);
        }
    }
    else if(auto pattern = address.target<std::string>())
    {
        // TODO
    }
}

}
