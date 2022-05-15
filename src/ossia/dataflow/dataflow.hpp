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
bool apply_to_destination(
    const destination_t& address, const DeviceList_T& devices, Fun f, NodeFun nf)
{
  switch (address.which().index())
  {
    // ossia::net::parameter_base*
    case destination_t::index_of<ossia::net::parameter_base*>().index():
    {
      f(*address.target<ossia::net::parameter_base*>(), true);
      return true;
    }

    // ossia::traversal::path
    case destination_t::index_of<ossia::traversal::path>().index():
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
      return unique;
    }

    // ossia::net::node_base*
    case destination_t::index_of<ossia::net::node_base*>().index():
    {
      nf(*address.target<ossia::net::node_base*>(), true);
      return true;
    }
    default:
    {
      return true;
    }
  }
}
}
