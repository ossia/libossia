#pragma once
#include <ossia/network/osc/detail/osc_common_policy.hpp>
#include <ossia/network/osc/detail/osc_utils.hpp>
#include <ossia/network/value/value.hpp>

#include <boost/endian/conversion.hpp>

#include <oscpack/osc/OscOutboundPacketStream.h>
#include <oscpack/osc/OscTypes.h>

// This handles all the other cases: char, rgb color, etc... as well as arrays
namespace ossia::net
{
struct osc_extended_outbound_dynamic_policy : osc_common_outbound_dynamic_policy
{
  using osc_common_outbound_dynamic_policy::operator();
  mutable int m_depth = 0;
  void operator()(impulse) const { p << oscpack::Infinitum(); }

  void operator()(bool b) const { p << b; }

  void operator()(char c) const { p << c; }

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
    if(unit == ossia::rgba8_u{})
    {
      p << to_osc_rgba({vec});
    }
    else
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
  }

  // Arrays are nested
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

  void operator()(const value_map_type& t) const
  {
    if(m_depth > 0)
    {
      p << oscpack::BeginArray();
    }
    m_depth++;

    for(const auto& [k, v] : t)
    {
      p << oscpack::BeginArray();
      m_depth++;
      (*this)(k);
      v.apply(*this);
      m_depth--;
      p << oscpack::EndArray();
    }

    m_depth--;
    if(m_depth > 0)
    {
      p << oscpack::EndArray();
    }
  }
};

struct osc_extended_outbound_static_policy : osc_common_outbound_static_policy
{
  using osc_common_outbound_static_policy::operator();
  std::size_t operator()(
      char* buffer, ossia::impulse v, const ossia::extended_type& t) const noexcept
  {
    // NOTE: this is a change wrt the old ossia::oscquery::osc_outbound_visitor
    buffer[0] = ',';
    if(t.empty())
      buffer[1] = oscpack::INFINITUM_TYPE_TAG;
    else if(t == "nil")
      buffer[1] = oscpack::NIL_TYPE_TAG;
    else if(t == "empty")
      buffer[1] = '\0';
    buffer[2] = '\0';
    buffer[3] = '\0';

    return 4;
  }

  std::size_t operator()(char* buffer, ossia::impulse v) const noexcept
  {
    // NOTE: this is a change wrt the old ossia::oscquery::osc_outbound_visitor
    buffer[0] = ',';
    buffer[1] = '\0';
    buffer[2] = '\0';
    buffer[3] = '\0';

    return 4;
  }

  std::size_t operator()(char* buffer, bool v) const noexcept
  {
    buffer[0] = ',';
    buffer[1] = v ? oscpack::TRUE_TYPE_TAG : oscpack::FALSE_TYPE_TAG;
    buffer[2] = '\0';
    buffer[3] = '\0';

    return 4;
  }

  std::size_t operator()(char* buffer, char v) const noexcept
  {
    buffer[0] = ',';
    buffer[1] = oscpack::CHAR_TYPE_TAG;
    buffer[2] = '\0';
    buffer[3] = '\0';

    boost::endian::endian_store<int32_t, 4, boost::endian::order::big>(
        (unsigned char*)buffer + 4, (int32_t)v);

    return 8;
  }

  std::size_t operator()(char* buffer, oscpack::RgbaColor v) const noexcept
  {
    buffer[0] = ',';
    buffer[1] = oscpack::RGBA_COLOR_TYPE_TAG;
    buffer[2] = '\0';
    buffer[3] = '\0';

    boost::endian::endian_store<uint32_t, 4, boost::endian::order::big>(
        (unsigned char*)buffer + 4, v.value);
    return 8;
  }

  std::size_t operator()(char* buffer, vec4f t) const noexcept
  {
    if(unit == ossia::rgba8_u{})
    {
      return (*this)(buffer, to_osc_rgba({t}));
    }
    else
    {
      return osc_common_outbound_static_policy::operator()(buffer, t);
    }
  }
};

struct osc_extended_policy
{
  using static_policy = osc_extended_outbound_static_policy;
  using dynamic_policy = osc_extended_outbound_dynamic_policy;
};

}
