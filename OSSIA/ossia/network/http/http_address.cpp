#include <ossia/network/http/http_address.hpp>

namespace ossia
{
namespace net
{

http_address::http_address(
    const http_address_data& data,
    ossia::net::node_base& node_base):
  generic_address{data, node_base},
  mData{data}
{
}

http_address::~http_address()
{
}

}
}
