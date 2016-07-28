#include <ossia/network/local/local.hpp>
#include <ossia/network/generic/generic_address.hpp>
namespace impl
{
bool Local2::pull(OSSIA::net::Address& address)
{
    return false;
}

bool Local2::push(const OSSIA::net::Address& address)
{
    bool b = true;
    for(auto& proto : mExposed)
        b &= proto->push(address);

    return b;
}

bool Local2::observe(OSSIA::net::Address& address, bool enable)
{
    return false;
}

bool Local2::update(OSSIA::net::Node& node)
{
    return false;
}
}
