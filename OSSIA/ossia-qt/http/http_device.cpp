#include <ossia-qt/http/http_device.hpp>
#include <ossia-qt/http/http_protocol.hpp>

namespace ossia
{
namespace net
{

http_device::http_device(std::unique_ptr<http_protocol> protocol, std::string name):
  device_base(std::move(protocol)),
  http_node{http_address_data{name}, *this}
{
  m_protocol->set_device(*this);
}

http_device::~http_device()
{

}

}
}
