#pragma once
#include <ossia/dataflow/dataflow_fwd.hpp>
#include <ossia/network/base/node.hpp>
#include <ossia/network/common/path.hpp>
#include <ossia/detail/apply.hpp>
#include <vector>

namespace ossia
{
template<typename Fun, typename DeviceList_T>
bool apply_to_destination(
    const destination_t& address,
    const DeviceList_T& devices,
    Fun f)
{
  struct
  {
      Fun& f;
      const DeviceList_T& devices;
      bool operator()(ossia::net::parameter_base* addr) const
      {
        f(addr, true);
        return true;
      }
      bool operator()(ossia::net::node_base* addr) const
      {
        return true;
      }

      bool operator()(const ossia::traversal::path& path) const
      {
        std::vector<ossia::net::node_base*> roots{};
        for(auto n : devices)
          roots.push_back(&n->get_root_node());

        ossia::traversal::apply(path, roots);

        const bool unique = roots.size() == 1;
        for(auto n : roots)
          if(auto addr = n->get_parameter())
            f(addr, unique);
        return unique;
      }

      bool operator()() const
      {
        return true;
      }
  } vis{f, devices};
  return ossia::apply(vis, address);
}
}
