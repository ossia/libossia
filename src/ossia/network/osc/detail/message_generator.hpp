#pragma once
#include <ossia/detail/small_vector.hpp>
#include <ossia/network/base/node_functions.hpp>
#include <ossia/network/base/parameter.hpp>
#include <ossia/network/base/parameter_data.hpp>

#include <oscpack/osc/OscOutboundPacketStream.h>

#include <array>
#include <vector>
namespace oscpack
{
inline oscpack::OutboundPacketStream&
operator<<(oscpack::OutboundPacketStream& p, const std::vector<std::string_view>& values)
{
  for(auto val : values)
  {
    p << val;
  }

  return p;
}

inline oscpack::OutboundPacketStream&
operator<<(oscpack::OutboundPacketStream& p, const ossia::net::parameter_base& address)
{
  p << address.get_node().osc_address();

  return p;
}

inline oscpack::OutboundPacketStream&
operator<<(oscpack::OutboundPacketStream& p, const std::vector<std::string>& values)
{
  for(const auto& val : values)
  {
    p << std::string_view(val);
  }

  return p;
}

template <typename ValueWriter>
class osc_message_generator
{
public:
  osc_message_generator(oscpack::OutboundPacketStream& stream)
      : p{stream}
  {
  }

  const oscpack::OutboundPacketStream& operator()(std::string_view name, const char* v)
      = delete;

  void write(const char* arg) { p << std::string_view(arg); }

  template <typename Arg>
  void write(const Arg& arg)
  {
    p << arg;
  }
  void write(const ossia::value& v) { v.apply(ValueWriter{{p, ossia::unit_t{}}}); };
  void write(const std::vector<ossia::value>& v)
  {
    ValueWriter{{p, ossia::unit_t{}}}(v);
  };

  template <typename... Args>
  const oscpack::OutboundPacketStream&
  operator()(std::string_view name, const Args&... args)
  {
    p << oscpack::BeginMessageN(name);
    (write(args), ...);
    p << oscpack::EndMessage();
    return p;
  }

  const oscpack::OutboundPacketStream& stream() const { return p; }

  oscpack::OutboundPacketStream& p;
};

}
