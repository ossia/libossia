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

struct learned_type
{
  ossia::val_type type{};
  ossia::value value{};
};

//! The type an address takes from the message that introduces it.
learned_type learn_type(const oscpack::ReceivedMessage& m)
{
  // TODO this does not support rgba yet
  switch(m.ArgumentCount())
  {
    case 0:
      return {ossia::val_type::IMPULSE, {}};
    case 1: {
      auto val = osc_utilities::create_value(m.ArgumentsBegin());
      const auto type = val.get_type();
      return {type, std::move(val)};
    }
    case 2: {
      auto val = osc_utilities::create_list(m.ArgumentsBegin(), m.ArgumentsEnd());
      if(is_vec<2>(val))
        return {ossia::val_type::VEC2F, convert<ossia::vec2f>(val)};
      return {ossia::val_type::LIST, value{std::move(val)}};
    }
    case 3: {
      auto val = osc_utilities::create_list(m.ArgumentsBegin(), m.ArgumentsEnd());
      if(is_vec<3>(val))
        return {ossia::val_type::VEC3F, convert<ossia::vec3f>(val)};
      return {ossia::val_type::LIST, value{std::move(val)}};
    }
    case 4: {
      auto val = osc_utilities::create_list(m.ArgumentsBegin(), m.ArgumentsEnd());
      if(is_vec<4>(val))
        return {ossia::val_type::VEC4F, convert<ossia::vec4f>(val)};
      return {ossia::val_type::LIST, value{std::move(val)}};
    }
    default:
      return {
          ossia::val_type::LIST,
          value{osc_utilities::create_list(m.ArgumentsBegin(), m.ArgumentsEnd())}};
  }
}
}

node_base* osc_learn(node_base* n, const oscpack::ReceivedMessage& m)
{
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
    // An address introduced by an argument-less message carries no type yet: a
    // sender with nothing to say for now -- an empty list, a detector with
    // nothing in view -- must not pin it to an impulse for the rest of the
    // session.
    if(auto* param = n->get_parameter();
       param && param->get_value_type() == ossia::val_type::IMPULSE)
    {
      if(auto [type, val] = learn_type(m); type != ossia::val_type::IMPULSE)
      {
        param->set_value_type(type);
        param->set_value(std::move(val));
      }
    }
    return n;
  }

  auto [type, val] = learn_type(m);
  auto addr_p = n->create_parameter(type);
  if(type != ossia::val_type::IMPULSE)
    addr_p->set_value(std::move(val));

  return nullptr;
}

}
