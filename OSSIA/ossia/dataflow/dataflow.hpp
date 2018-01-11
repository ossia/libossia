#pragma once
#include <ossia/dataflow/dataflow_fwd.hpp>
#include <ossia/network/base/node.hpp>
#include <ossia/network/common/path.hpp>
#include <ossia/detail/apply.hpp>
#include <vector>

namespace ossia
{
template<typename Fun, typename DeviceList_T>
void apply_to_destination(
    const destination_t& address,
    const DeviceList_T& devices,
    Fun f)
{
  struct
  {
      Fun& f;
      const DeviceList_T& devices;
      void operator()(ossia::net::parameter_base* addr) const
      {
        f(addr, true);
      }
      void operator()(ossia::net::node_base* addr) const
      {
      }

      void operator()(const ossia::traversal::path& path) const
      {
        std::vector<ossia::net::node_base*> roots{};
        for(auto n : devices)
          roots.push_back(&n->get_root_node());

        ossia::traversal::apply(path, roots);

        const bool unique = roots.size() == 1;
        for(auto n : roots)
          if(auto addr = n->get_parameter())
            f(addr, unique);
      }

      void operator()() const
      {
      }
  } vis{f, devices};
  ossia::apply(vis, address);
}
}
