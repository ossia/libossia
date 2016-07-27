#include <ossia/network/v2/local/local.hpp>
#include <ossia/network/v2/generic/generic_address.hpp>
namespace impl
{
bool Local2::pullAddressValue(OSSIA::v2::Address& address)
{
    return false;
}

bool Local2::pushAddressValue(const OSSIA::v2::Address& address)
{
    bool b = true;
    for(auto& proto : mExposed)
        b &= proto->pushAddressValue(address);

    return b;
}

bool Local2::observeAddressValue(OSSIA::v2::Address& address, bool enable)
{
    return false;
}

bool Local2::updateChildren(OSSIA::v2::Node& node)
{
    return false;
}
}
