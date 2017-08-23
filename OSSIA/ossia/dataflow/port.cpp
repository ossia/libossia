// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/dataflow/graph.hpp>
#include <ossia/dataflow/port.hpp>

namespace ossia
{

void outlet::write(execution_state& e)
{
  if (auto addr_ptr = address.target<ossia::net::parameter_base*>())
  {
    ossia::net::parameter_base* addr = *addr_ptr;
    if (scope & port::scope_t::local)
    {
      graph::copy_to_local(data, *addr, e);
    }
    else if (scope & port::scope_t::global)
    {
      graph::copy_to_global(data, *addr, e);
    }
  }
  else if (auto pattern = address.target<std::string>())
  {
    // TODO
  }
}
}
