#include <ossia-qt/serial/serial_address.hpp>
#include <ossia-qt/serial/serial_node.hpp>

namespace ossia
{
namespace net
{

serial_address::serial_address(
    const serial_address_data& p,
    serial_node& parent):
  generic_address{p, parent},
  mData{p}
{
}

void serial_address::valueCallback(const value& val)
{
  this->setValue(val);
  send(m_value);
}


}
}
