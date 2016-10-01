#include <ossia/network/generic/generic_address.hpp>
#include <ossia/network/generic/generic_node.hpp>
#include <ossia/editor/dataspace/detail/make_value.hpp>

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

std::string address_string_from_node(const ossia::net::address_base& addr)
{
    return address_string_from_node(addr.getNode());
}

value address_base::fetchValue()
{
  pullValue();
  return cloneValue();
}

std::vector<std::string> address_base::getTags() const { return {}; }

address_base& address_base::setTags(const std::vector<std::string>& v) { return *this; }

std::string address_base::getDescription() const { return {}; }

address_base& address_base::setDescription(const std::string& v) { return *this; }

value address_base::getDefaultValue() const { return {}; }

address_base& address_base::setDefaultValue(const value& v) { return *this; }

unit_t address_base::getUnit() const { return {}; }

address_base& address_base::setUnit(const unit_t& v) { return *this; }

value_with_unit get_value(address_base& addr)
{
  auto v = addr.cloneValue();
  auto u = addr.getUnit();
  if(!u || !v.valid())
    return v;

  return eggs::variants::apply([&] (const auto& dataspace) -> ossia::value_with_unit {
    return eggs::variants::apply(make_value_with_unit_visitor{}, v.v, dataspace);
  }, u);
}

}
}
