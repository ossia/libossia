#pragma once
#include <ossia/dataflow/graph.hpp>
#include <ossia/dataflow/node_process.hpp>
#include <ossia/network/base/node.hpp>
#include <vector>

namespace ossia
{
template<typename Fun>
void apply_to_destination(const destination_t& address, const execution_state& e, Fun f)
{
  struct
  {
      Fun& f;
      const execution_state& e;
      void operator()(ossia::net::parameter_base* addr) const
      {
        f(addr);
      }
      void operator()(ossia::net::node_base* addr) const
      {
      }

      void operator()(const ossia::traversal::path& path) const
      {
        std::vector<ossia::net::node_base*> roots{};
        for(auto n : e.globalState)
          roots.push_back(&n->get_root_node());

        ossia::traversal::apply(path, roots);

        for(auto n : roots)
          if(auto addr = n->get_parameter())
            (*this)(addr);
      }

      void operator()() const
      {
      }
  } vis{f, e};
  ossia::apply(vis, address);
}
}
