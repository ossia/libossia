#pragma once
#include <ossia/network/osc/detail/osc_utils.hpp>
#include <ossia/network/value/value.hpp>

#include <boost/endian/conversion.hpp>

#include <oscpack/osc/OscOutboundPacketStream.h>
#include <oscpack/osc/OscTypes.h>

namespace ossia::net
{

template <typename T>
struct osc_always_bundled_policy : T
{
  using bundled = std::true_type;
};
template <typename T>
using osc_never_bundled_policy = T;
// Handling for types compatible with all OSC version

// This class is an implementation detail. It is used to send things that work
// for all versions of OSC. Used for the dynamic std::vector<ossia::value>
// case.
struct osc_common_outbound_dynamic_policy
{
  oscpack::OutboundPacketStream& p;
  const ossia::unit_t& unit;

  void operator()(int32_t i) const { p << i; }

  void operator()(float f) const { p << f; }

  void operator()(const std::string& str) const { p << (std::string_view)str; }

  void operator()(oscpack::Blob str) const { p << str; }

  void operator()(vec2f vec) const { p << vec[0] << vec[1]; }

  void operator()(vec3f vec) const { p << vec[0] << vec[1] << vec[2]; }

  void operator()(vec4f vec) const { p << vec[0] << vec[1] << vec[2] << vec[3]; }

  void operator()() const { }
};

// Since most of the time we have known types, we can optimize a fair bit the
// writing of the osc messages.
// Mirrors ossia::net::osc_outbound_array_visitor
struct osc_common_outbound_static_policy
{
  const ossia::unit_t& unit;
  std::size_t operator()(char* buffer, float v) const noexcept
  {
    buffer[0] = ',';
    buffer[1] = oscpack::FLOAT_TYPE_TAG;
    buffer[2] = '\0';
    buffer[3] = '\0';

    boost::endian::endian_store<float, 4, boost::endian::order::big>(
        (unsigned char*)buffer + 4, v);
    return 8;
  }

  std::size_t operator()(char* buffer, int32_t v) const noexcept
  {
    buffer[0] = ',';
    buffer[1] = oscpack::INT32_TYPE_TAG;
    buffer[2] = '\0';
    buffer[3] = '\0';

    boost::endian::endian_store<int32_t, 4, boost::endian::order::big>(
        (unsigned char*)buffer + 4, v);
    return 8;
  }

  std::size_t operator()(char* buffer, const std::string& v) const noexcept
  {
    buffer[0] = ',';
    buffer[1] = oscpack::STRING_TYPE_TAG;
    buffer[2] = '\0';
    buffer[3] = '\0';

    std::size_t written = write_string(v, buffer + 4);
    return 4 + written;
  }

  std::size_t operator()(char* buffer, const oscpack::Blob& v) const noexcept
  {
    buffer[0] = ',';
    buffer[1] = oscpack::BLOB_TYPE_TAG;
    buffer[2] = '\0';
    buffer[3] = '\0';

    std::size_t written
        = write_string(std::string_view((const char*)v.data, v.size), buffer + 4);
    return 4 + written;
  }

  std::size_t operator()(char* buffer, vec2f v) const noexcept
  {
    buffer[0] = ',';
    buffer[1] = oscpack::FLOAT_TYPE_TAG;
    buffer[2] = oscpack::FLOAT_TYPE_TAG;
    buffer[3] = '\0';

    boost::endian::endian_store<float, 4, boost::endian::order::big>(
        (unsigned char*)buffer + 4, v[0]);
    boost::endian::endian_store<float, 4, boost::endian::order::big>(
        (unsigned char*)buffer + 8, v[1]);

    return 12;
  }

  std::size_t operator()(char* buffer, vec3f v) const noexcept
  {
    buffer[0] = ',';
    buffer[1] = oscpack::FLOAT_TYPE_TAG;
    buffer[2] = oscpack::FLOAT_TYPE_TAG;
    buffer[3] = oscpack::FLOAT_TYPE_TAG;
    buffer[4] = '\0';
    buffer[5] = '\0';
    buffer[6] = '\0';
    buffer[7] = '\0';

    boost::endian::endian_store<float, 4, boost::endian::order::big>(
        (unsigned char*)buffer + 8, v[0]);
    boost::endian::endian_store<float, 4, boost::endian::order::big>(
        (unsigned char*)buffer + 12, v[1]);
    boost::endian::endian_store<float, 4, boost::endian::order::big>(
        (unsigned char*)buffer + 16, v[2]);

    return 20;
  }

  std::size_t operator()(char* buffer, vec4f v) const noexcept
  {
    buffer[0] = ',';
    buffer[1] = oscpack::FLOAT_TYPE_TAG;
    buffer[2] = oscpack::FLOAT_TYPE_TAG;
    buffer[3] = oscpack::FLOAT_TYPE_TAG;
    buffer[4] = oscpack::FLOAT_TYPE_TAG;
    buffer[5] = '\0';
    buffer[6] = '\0';
    buffer[7] = '\0';

    boost::endian::endian_store<float, 4, boost::endian::order::big>(
        (unsigned char*)buffer + 8, v[0]);
    boost::endian::endian_store<float, 4, boost::endian::order::big>(
        (unsigned char*)buffer + 12, v[1]);
    boost::endian::endian_store<float, 4, boost::endian::order::big>(
        (unsigned char*)buffer + 16, v[2]);
    boost::endian::endian_store<float, 4, boost::endian::order::big>(
        (unsigned char*)buffer + 20, v[3]);

    return 24;
  }
};

}
