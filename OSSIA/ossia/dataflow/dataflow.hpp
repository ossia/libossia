#pragma once
#include <ossia/dataflow/dataflow_fwd.hpp>
#include <ossia/network/base/node.hpp>
#include <ossia/network/common/path.hpp>
#include <vector>

namespace ossia
{
template<typename Fun, typename DeviceList_T>
bool apply_to_destination(
    const destination_t& address,
    const DeviceList_T& devices,
    Fun f)
{
  switch(address.which())
  {
    // ossia::net::parameter_base*
    case 0:
    {
      f(*address.target<ossia::net::parameter_base*>(), true);
      return true;
    }

    // ossia::traversal::path
    case 1:
    {
      std::vector<ossia::net::node_base*> roots{};
      auto& p = *address.target<ossia::traversal::path>();
      //if(p.pattern.)
      for(auto n : devices)
        roots.push_back(&n->get_root_node());

      ossia::traversal::apply(p, roots);

      const bool unique = roots.size() == 1;
      for(auto n : roots)
        if(auto addr = n->get_parameter())
          f(addr, unique);
      return unique;

    }

    // ossia::net::node_base*
    case 2:
    {
      return true;
    }
    default:
    {
      return true;
    }
  }
}
}
