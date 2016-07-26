#include <ossia/network/v2/generic/generic_device.hpp>
#include <ossia/network/v2/generic/generic_node.hpp>

namespace impl
{
BasicDevice::BasicDevice(
        std::unique_ptr<OSSIA::v2::Protocol> protocol,
        std::string name) :
    Device(std::move(protocol)),
    BasicNode(std::move(name), *this)
{
}

BasicDevice::~BasicDevice()
{
    mChildren.clear();
}
}
