#pragma once
#include <ossia/network/base/node.hpp>

namespace ossia::net
{

template <typename F>
void visit(ossia::net::node_base& root, F&& f)
{
  f(root);
  for (auto& cld : root.children())
  {
    visit(*cld, f);
  }
}

template <typename F>
void visit_parameters(ossia::net::node_base& root, F&& f)
{
  if (auto p = root.get_parameter())
    f(root, *p);
  for (auto& cld : root.children())
  {
    visit_parameters(*cld, f);
  }
}
}
