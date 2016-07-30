#include <ossia/network/base/device.hpp>
#include <ossia/network/base/protocol.hpp>

namespace ossia
{
namespace net
{
device::~device() = default;

device::device(std::unique_ptr<protocol> proto):
    mProtocol{std::move(proto)}
{

}

protocol &device::getProtocol() const
{ return *mProtocol; }


}
}
