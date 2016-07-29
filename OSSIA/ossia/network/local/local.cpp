#include <ossia/network/local/local.hpp>
#include <ossia/network/generic/generic_address.hpp>
namespace impl
{
Local2::Local2()
{

}

Local2::~Local2()
{

}

bool Local2::pull(ossia::net::address& address)
{
    return false;
}

bool Local2::push(const ossia::net::address& address)
{
    bool b = true;
    for(auto& proto : mExposed)
        b &= proto->push(address);

    return b;
}

bool Local2::observe(ossia::net::address& address, bool enable)
{
    return false;
}

bool Local2::update(ossia::net::Node& node)
{
    return false;
}
}
