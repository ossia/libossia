#include <ossia/network/v2/local/local.hpp>
#include <ossia/network/v2/generic/generic_address.hpp>
namespace impl
{
bool Local2::pullAddressValue(OSSIA::v2::Address& address) const
{
    return false;
}

bool Local2::pushAddressValue(const OSSIA::v2::Address& address) const
{
    // Update the address and notify

    return false;
}

bool Local2::observeAddressValue(OSSIA::v2::Address& address, bool enable) const
{
    return false;
}

bool Local2::updateChildren(OSSIA::v2::Node& node) const
{
    return false;
}
}
