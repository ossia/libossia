#pragma once
#include <ossia/network/dataspace/color.hpp>
#include <ossia/network/value/value.hpp>

#include <oscpack/osc/OscOutboundPacketStream.h>
namespace oscpack
{
class OutboundPacketStream;
}
namespace ossia::oscquery
{
struct osc_outbound_visitor
{
public:
  explicit osc_outbound_visitor(oscpack::OutboundPacketStream& stream)
      : p{stream}
  {
  }

  oscpack::OutboundPacketStream& p;
  void operator()(impulse) const
  {
    if(m_depth > 0)
      p << oscpack::Infinitum();
  }

  void operator()(int32_t i) const { p << i; }

  void operator()(float f) const { p << f; }

  void operator()(bool b) const { p << b; }

  void operator()(char c) const { p << c; }

  void operator()(const std::string& str) const { p << std::string_view{str}; }

  void operator()(vec2f vec) const
  {
    if(m_depth > 0)
    {
      p << oscpack::BeginArray();
    }

    p << vec[0] << vec[1];

    if(m_depth > 0)
    {
      p << oscpack::EndArray();
    }
  }

  void operator()(vec3f vec) const
  {
    if(m_depth > 0)
    {
      p << oscpack::BeginArray();
    }

    p << vec[0] << vec[1] << vec[2];

    if(m_depth > 0)
    {
      p << oscpack::EndArray();
    }
  }

  void operator()(vec4f vec) const
  {
    if(m_depth > 0)
    {
      p << oscpack::BeginArray();
    }

    p << vec[0] << vec[1] << vec[2] << vec[3];

    if(m_depth > 0)
    {
      p << oscpack::EndArray();
    }
  }

  void operator()(const std::vector<value>& t) const
  {
    if(m_depth > 0)
    {
      p << oscpack::BeginArray();
    }

    m_depth++;
    for(const auto& val : t)
    {
      val.apply(*this);
    }
    m_depth--;

    if(m_depth > 0)
    {
      p << oscpack::EndArray();
    }
  }

  void operator()(const value_map_type& t) const { }

  template <typename T, typename U>
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

  void operator()() const { }

private:
  mutable int m_depth = 0;
};
}
