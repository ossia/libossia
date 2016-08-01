#include <ossia/network/generic/generic_address.hpp>
#include <ossia/network/generic/generic_node.hpp>
namespace ossia
{
namespace net
{
address_base::~address_base() = default;

static void
getAddressFromNode_rec(const node_base& n, std::vector<std::string>& str)
{
  if (auto p = n.getParent())
    getAddressFromNode_rec(*p, str);

  str.push_back(n.getName());
}

std::string address_string_from_node(const ossia::net::node_base& node)
{
  std::vector<std::string> vec;
  getAddressFromNode_rec(node, vec);

  // vec cannot be empty.

  std::size_t i = 0u;

  std::string str;
  str.reserve(vec.size() * 5);
  str.append(vec.at(i++));
  str.append(":/");

  auto n = vec.size();
  for (; i < n - 1; i++)
  {
    str.append(vec.at(i));
    str.append("/");
  }
  if ((n - 1) > 0)
    str.append(vec.at(n - 1));

  return str;
}
}
}
