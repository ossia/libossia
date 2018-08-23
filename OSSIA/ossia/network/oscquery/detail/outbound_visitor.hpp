#pragma once
#include <ossia/network/value/value.hpp>
#include <oscpack/osc/OscOutboundPacketStream.h>
#include <ossia/network/dataspace/color.hpp>
namespace oscpack
{
class OutboundPacketStream;
}
namespace ossia
{
namespace oscquery
{
struct osc_outbound_visitor
{
public:
  oscpack::OutboundPacketStream& p;
  void operator()(impulse) const
  {
  }

  void operator()(int32_t i) const
  {
    p << i;
  }

  void operator()(float f) const
  {
    p << f;
  }

  void operator()(bool b) const
  {
    p << b;
  }

  void operator()(char c) const
  {
    p << c;
  }

  void operator()(const std::string& str) const
  {
    p << std::string_view{str};
  }

  void operator()(vec2f vec) const
  {
    p << vec[0] << vec[1];
  }

  void operator()(vec3f vec) const
  {
    p << vec[0] << vec[1] << vec[2];
  }

  void operator()(vec4f vec) const
  {
    p << vec[0] << vec[1] << vec[2] << vec[3];
  }

  void operator()(const std::vector<value>& t) const
  {
    for (const auto& val : t)
    {
      val.apply(*this);
    }
  }

  template<typename T, typename U>
  void operator()(const T& t, const U& u) const
  {
    (*this)(t);
  }

  void operator()(const vec4f& t, const ossia::rgba8_u& u) const
  {
    uint32_t r = (uint32_t)t[0] << 24;
    uint32_t g = (uint32_t)t[1] << 16;
    uint32_t b = (uint32_t)t[2] << 8;
    uint32_t a = (uint32_t)t[3];
    p << oscpack::RgbaColor(r + g + b + a);
  }

  void operator()() const
  {
  }
};
}
}
