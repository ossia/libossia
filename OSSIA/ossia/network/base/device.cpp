#include <ossia/network/base/device.hpp>
#include <ossia/network/base/protocol.hpp>

namespace ossia
{
namespace net
{
device_base::~device_base() = default;

device_base::device_base(std::unique_ptr<protocol_base> proto):
    mProtocol{std::move(proto)}
{

}

protocol_base &device_base::getProtocol() const
{ return *mProtocol; }


}
}
