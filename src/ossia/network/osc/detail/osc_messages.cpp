#include <ossia/network/osc/detail/osc_messages.hpp>
#include <ossia/network/base/osc_address.hpp>
#include <ossia/detail/logger.hpp>

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

struct bundle_common_policy
{
  template<typename Addr_T>
  void operator()(oscpack::OutboundPacketStream& str, ossia::value& val, const Addr_T& addr)
  {
    if (val = filter_value(addr, addr.value()); val.valid())
    {
      str << oscpack::BeginMessageN(osc_address(addr));
      val.apply(osc_1_0_outbound_stream_visitor{{str}});
      str << oscpack::EndMessage();
    }
  }
};
struct bundle_client_policy
{
  template<typename Addr_T>
  void operator()(oscpack::OutboundPacketStream& str, ossia::value& val, const Addr_T& addr)
  {
    if (addr.get_access() == ossia::access_mode::GET)
      return;

    bundle_common_policy{}(str, val, addr);
  }
};

using bundle_server_policy = bundle_common_policy;


template<typename Policy_T>
std::optional<buffer_packed_osc_stream> make_raw_bundle(Policy_T add_element_to_bundle, const std::vector<full_parameter_data> &addresses)
try
{
  auto buffer{std::make_unique<char[]>(max_osc_message_size)};
  oscpack::OutboundPacketStream str(buffer.get(), max_osc_message_size);
  str << oscpack::BeginBundleImmediate();

  ossia::value val;
  for (const auto& addr : addresses)
  {
    add_element_to_bundle(str, val, addr);
  }
  str << oscpack::EndBundle();
  return buffer_packed_osc_stream{std::move(buffer), std::move(str)};
}
catch (const oscpack::OutOfBufferMemoryException&)
{
  ossia::logger().error("make_raw_bundle_client: message too large (limit is {} bytes)", max_osc_message_size);
  return {};
}
catch (const std::runtime_error& e)
{
  ossia::logger().error("make_raw_bundle_client: {}", e.what());
  return {};
}
catch (...)
{
  ossia::logger().error("make_raw_bundle_client: unknown error");
  return {};
}

template<typename Policy_T>
std::optional<buffer_packed_osc_stream> make_bundle(Policy_T add_element_to_bundle, const std::vector<const parameter_base *> &addresses)
try
{
  auto buffer{std::make_unique<char[]>(max_osc_message_size)};
  oscpack::OutboundPacketStream str(buffer.get(), max_osc_message_size);
  str << oscpack::BeginBundleImmediate();

  ossia::value val;
  for (auto a : addresses)
  {
    const ossia::net::parameter_base& addr = *a;
    add_element_to_bundle(str, val, addr);
  }
  str << oscpack::EndBundle();
  return buffer_packed_osc_stream{std::move(buffer), std::move(str)};
}
catch (const oscpack::OutOfBufferMemoryException&)
{
  ossia::logger().error("make_bundle_client: message too large (limit is {} bytes)", max_osc_message_size);
  return {};
}
catch (const std::runtime_error& e)
{
  ossia::logger().error("make_bundle_client: {}", e.what());
  return {};
}
catch (...)
{
  ossia::logger().error("make_bundle_client: unknown error");
  return {};
}
}

std::optional<buffer_packed_osc_stream> make_raw_bundle_client(const std::vector<full_parameter_data> &addresses)
{
  return make_raw_bundle(bundle_client_policy{}, addresses);
}
std::optional<buffer_packed_osc_stream> make_raw_bundle_server(const std::vector<full_parameter_data> &addresses)
{
  return make_raw_bundle(bundle_server_policy{}, addresses);
}

std::optional<buffer_packed_osc_stream> make_bundle_client(const std::vector<const parameter_base *> &addresses)
{
  return make_bundle(bundle_client_policy{}, addresses);
}
std::optional<buffer_packed_osc_stream> make_bundle_server(const std::vector<const parameter_base *> &addresses)
{
  return make_bundle(bundle_server_policy{}, addresses);
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
