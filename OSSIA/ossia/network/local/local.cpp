#include <ossia/network/local/local.hpp>
#include <ossia/network/generic/generic_address.hpp>

namespace ossia
{
namespace net
{
local_protocol::local_protocol()
{

}

local_protocol::~local_protocol()
{

}

bool local_protocol::pull(ossia::net::address_base& address)
{
    return false;
}

bool local_protocol::push(const ossia::net::address_base& address)
{
    bool b = true;
    for(auto& proto : mExposed)
        b &= proto->push(address);

    return b;
}

bool local_protocol::observe(ossia::net::address_base& address, bool enable)
{
    return false;
}

bool local_protocol::update(ossia::net::node_base& node)
{
    return false;
}
}
}
