#include <ossia/detail/logger.hpp>
#include <ossia/network/base/osc_address.hpp>
#include <ossia/network/osc/detail/osc_messages.hpp>
#include <ossia/network/osc/detail/osc_receive.hpp>

namespace ossia::net
{
namespace
{
template <std::size_t N>
static bool is_vec(std::vector<ossia::value>& t)
{
  return t.size() == N && ossia::all_of(t, [](const ossia::value& val) {
           return val.get_type() == ossia::val_type::FLOAT;
         });
}
}

node_base* osc_learn(node_base* n, const oscpack::ReceivedMessage& m)
{
  // TODO this does not support rgba yet
  // TODO put them in a hash map instead.
  // Find-or-add algorithm
  std::string_view addr = m.AddressPattern();
  // TODO string -> string_view
  std::vector<std::string> v = address_parts(addr);

  bool is_new = false;
  for(const auto& part : v)
  {
    auto cld = n->find_child(part);
    if(cld)
    {
      n = cld;
    }
    else
    {
      // Start adding
      n = n->create_child(part);
      is_new = true;
    }
  }

  if(!is_new)
  {
    return n;
  }

  // Set-up address
  switch(m.ArgumentCount())
  {
    case 0: {
      n->create_parameter();
      break;
    }
    case 1: {
      auto val = osc_utilities::create_value(m.ArgumentsBegin());
      auto addr = n->create_parameter(val.get_type());
      addr->set_value(val);
      break;
    }
    case 2: {
      auto val = osc_utilities::create_list(m.ArgumentsBegin(), m.ArgumentsEnd());
      if(is_vec<2>(val))
      {
        auto addr = n->create_parameter(ossia::val_type::VEC2F);
        addr->set_value(convert<ossia::vec2f>(val));
      }
      else
      {
        auto addr = n->create_parameter(ossia::val_type::LIST);
        addr->set_value(
            value{osc_utilities::create_list(m.ArgumentsBegin(), m.ArgumentsEnd())});
      }
      break;
    }
    case 3: {
      auto val = osc_utilities::create_list(m.ArgumentsBegin(), m.ArgumentsEnd());
      if(is_vec<3>(val))
      {
        auto addr = n->create_parameter(ossia::val_type::VEC3F);
        addr->set_value(convert<ossia::vec3f>(val));
      }
      else
      {
        auto addr = n->create_parameter(ossia::val_type::LIST);
        addr->set_value(
            value{osc_utilities::create_list(m.ArgumentsBegin(), m.ArgumentsEnd())});
      }
      break;
    }
    case 4: {
      auto val = osc_utilities::create_list(m.ArgumentsBegin(), m.ArgumentsEnd());
      if(is_vec<4>(val))
      {
        auto addr = n->create_parameter(ossia::val_type::VEC4F);
        addr->set_value(convert<ossia::vec4f>(val));
      }
      else
      {
        auto addr = n->create_parameter(ossia::val_type::LIST);
        addr->set_value(
            value{osc_utilities::create_list(m.ArgumentsBegin(), m.ArgumentsEnd())});
      }
      break;
    }
    default: {
      auto addr = n->create_parameter(ossia::val_type::LIST);
      addr->set_value(
          value{osc_utilities::create_list(m.ArgumentsBegin(), m.ArgumentsEnd())});
      break;
    }
  }

  return nullptr;
}

}
