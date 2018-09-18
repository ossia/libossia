// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/network/base/node.hpp>
#include <ossia/network/base/parameter.hpp>
#include <ossia/network/common/debug.hpp>
#include <ossia/network/dataspace/dataspace_visitors.hpp>
#include <ossia/network/domain/domain.hpp>
#include <ossia/network/value/value.hpp>

namespace ossia::net
{
void debug_recursively(std::string& str, const node_base& n)
{
  str += fmt::format("{}\n", n);

  for (auto& cld : n.children())
    debug_recursively(str, *cld);
}

}
