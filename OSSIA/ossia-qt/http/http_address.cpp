// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia-qt/http/http_address.hpp>

namespace ossia
{
namespace net
{

http_address::http_address(
    const http_address_data& data, ossia::net::node_base& node_base)
    : generic_address{data, node_base}, mData{data}
{
}

http_address::~http_address()
{
  callback_container<value_callback>::callbacks_clear();
}
}
}
