#include <ossia/network/generic/generic_address.hpp>
#include <ossia/network/generic/generic_node.hpp>
#include <ossia/editor/dataspace/dataspace_visitors.hpp>


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

value_with_unit get_value(const ossia::Destination& d)
{
  ossia::net::address_base& addr = d.value.get();

  return make_value(addr.cloneValue(d.index), addr.getUnit());
}

void push_value(const Destination& d, const value_with_unit& v)
{
  ossia::net::address_base& addr = d.value.get();
  addr.pushValue(ossia::to_value(v)); // TODO what about destination_index ??
}


}
}
