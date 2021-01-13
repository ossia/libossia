#pragma once
#include <ossia/dataflow/dataflow_fwd.hpp>
#include <ossia/network/base/node.hpp>
#include <ossia/network/common/path.hpp>

#include <vector>

namespace ossia
{
struct do_nothing_for_nodes {
  void operator()(ossia::net::node_base* node, bool) const noexcept { }
};

template <typename Fun, typename NodeFun, typename DeviceList_T>
bool apply_to_destination_impl(
    const destination_t& address, const DeviceList_T& devices, const Fun& f, const NodeFun& nf, bool default_unique = true)
{
  switch (address.which())
  {
    // ossia::net::parameter_base*
    case 0:
    {
      f(*address.target<ossia::net::parameter_base*>(), default_unique);
      return default_unique;
    }

    // ossia::traversal::path
    case 1:
    {
      std::vector<ossia::net::node_base*> roots{};

      for (auto n : devices)
        roots.push_back(&n->get_root_node());

      auto& p = *address.target<ossia::traversal::path>();
      ossia::traversal::apply(p, roots);

      const bool unique = roots.size() == 1;
      for (auto n : roots)
        if (auto addr = n->get_parameter())
          f(addr, unique);
        else
          nf(n, unique);
      return unique && default_unique;
    }

    // ossia::net::node_base*
    case 2:
    {
      nf(*address.target<ossia::net::node_base*>(), default_unique);
      return default_unique;
    }
    default:
    {
      return default_unique;
    }
  }
}


template <typename T, typename... Args>
bool apply_to_destination(
    const T& addresses, Args&&... args)
{
  switch(addresses.size())
  {
    case 0:
    {
      return true;
    }
    case 1:
    {
      return apply_to_destination_impl(addresses[0], std::forward<Args>(args)..., true);
    }
    default:
    {
      for(const destination_t& addr : addresses)
        apply_to_destination_impl(addr, std::forward<Args>(args)..., false);
      return false;
    }
  }
}
}
