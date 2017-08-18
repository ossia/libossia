// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia-qt/http/http_parameter.hpp>

namespace ossia
{
namespace net
{

http_parameter::http_parameter(
    const http_parameter_data& data, ossia::net::node_base& node_base)
    : generic_parameter{data, node_base}, mData{data}
{
}

http_parameter::~http_parameter()
{
  callback_container<value_callback>::callbacks_clear();
}
}
}
