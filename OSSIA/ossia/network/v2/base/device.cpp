#include "Device.hpp"
#include "Protocol.hpp"

namespace OSSIA
{
namespace v2
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
