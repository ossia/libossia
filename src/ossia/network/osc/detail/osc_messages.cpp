#include <ossia/network/osc/detail/osc_messages.hpp>

namespace ossia::net
{

template <std::size_t N>
static bool is_vec(std::vector<ossia::value>& t)
{
  return t.size() == N && ossia::all_of(t, [](const ossia::value& val) {
           return val.get_type() == ossia::val_type::FLOAT;
         });
}

std::optional<buffer_packed_osc_stream> make_raw_bundle(const std::vector<full_parameter_data> &addresses)
{
  try
  {
    auto buffer{std::make_unique<char[]>(max_osc_message_size)};
    oscpack::OutboundPacketStream str(buffer.get(), max_osc_message_size);
    str << oscpack::BeginBundleImmediate();
    for (const auto& addr : addresses)
    {
      if (addr.get_access() == ossia::access_mode::GET)
        continue;

      ossia::value val = filter_value(addr, addr.value());
      if (val.valid())
      {
        str << oscpack::BeginMessageN(addr.address);
        val.apply(osc_outbound_visitor{{str}});
        str << oscpack::EndMessage();
      }
    }
    str << oscpack::EndBundle();
    return buffer_packed_osc_stream{std::move(buffer), std::move(str)};
  }
  catch (const oscpack::OutOfBufferMemoryException&)
  {
    return {};
  }
}

std::optional<buffer_packed_osc_stream> make_bundle(const std::vector<const parameter_base *> &addresses)
{
  try
  {
    auto buffer{std::make_unique<char[]>(max_osc_message_size)};
    oscpack::OutboundPacketStream str(buffer.get(), max_osc_message_size);
    str << oscpack::BeginBundleImmediate();
    for (auto a : addresses)
    {
      const ossia::net::parameter_base& addr = *a;
      if (addr.get_access() == ossia::access_mode::GET)
        continue;

      ossia::value val = filter_value(addr, addr.value());
      if (val.valid())
      {
        str << oscpack::BeginMessageN(addr.get_node().osc_address());
        val.apply(osc_outbound_visitor{{str}});
        str << oscpack::EndMessage();
      }
    }
    str << oscpack::EndBundle();
    return buffer_packed_osc_stream{std::move(buffer), std::move(str)};
  }
  catch (const oscpack::OutOfBufferMemoryException&)
  {
    return {};
  }
}

void osc_learn(node_base *n, const oscpack::ReceivedMessage &m)
{
  // TODO put them in a hash map instead.
  // Find-or-add algorithm
  ossia::string_view addr = m.AddressPattern();
  // TODO string -> string_view
  std::vector<std::string> v = address_parts(addr);

  bool is_new = false;
  for (const auto& part : v)
  {
    auto cld = n->find_child(part);
    if (cld)
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

  if (!is_new)
    return;

  // Set-up address
  switch (m.ArgumentCount())
  {
  case 0:
  {
    n->create_parameter();
    break;
  }
  case 1:
  {
    auto val = osc_utilities::create_value(m.ArgumentsBegin());
    auto addr = n->create_parameter(val.get_type());
    addr->set_value(val);
    break;
  }
  case 2:
  {
    auto val
        = osc_utilities::create_list(m.ArgumentsBegin(), m.ArgumentsEnd());
    if (is_vec<2>(val))
    {
      auto addr = n->create_parameter(ossia::val_type::VEC2F);
      addr->set_value(convert<ossia::vec2f>(val));
    }
    else
    {
      auto addr = n->create_parameter(ossia::val_type::LIST);
      addr->set_value(
            osc_utilities::create_list(m.ArgumentsBegin(), m.ArgumentsEnd()));
    }
    break;
  }
  case 3:
  {
    auto val
        = osc_utilities::create_list(m.ArgumentsBegin(), m.ArgumentsEnd());
    if (is_vec<3>(val))
    {
      auto addr = n->create_parameter(ossia::val_type::VEC3F);
      addr->set_value(convert<ossia::vec3f>(val));
    }
    else
    {
      auto addr = n->create_parameter(ossia::val_type::LIST);
      addr->set_value(
            osc_utilities::create_list(m.ArgumentsBegin(), m.ArgumentsEnd()));
    }
    break;
  }
  case 4:
  {
    auto val
        = osc_utilities::create_list(m.ArgumentsBegin(), m.ArgumentsEnd());
    if (is_vec<4>(val))
    {
      auto addr = n->create_parameter(ossia::val_type::VEC4F);
      addr->set_value(convert<ossia::vec4f>(val));
    }
    else
    {
      auto addr = n->create_parameter(ossia::val_type::LIST);
      addr->set_value(
            osc_utilities::create_list(m.ArgumentsBegin(), m.ArgumentsEnd()));
    }
    break;
  }
  default:
  {
    auto addr = n->create_parameter(ossia::val_type::LIST);
    addr->set_value(
          osc_utilities::create_list(m.ArgumentsBegin(), m.ArgumentsEnd()));
    break;
  }
  }
}

}
