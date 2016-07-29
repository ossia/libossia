#include <ossia/network/base/device.hpp>
#include <ossia/network/base/protocol.hpp>

namespace ossia
{
namespace net
{
Device::~Device() = default;

Device::Device(std::unique_ptr<Protocol> proto):
    mProtocol{std::move(proto)}
{

}

Protocol &Device::getProtocol() const
{ return *mProtocol; }


}
}
