#pragma once
#include <ossia/network/value/value.hpp>
#include <ossia/network/osc/detail/message_generator.hpp>
#include <ossia/network/osc/detail/osc_utils.hpp>
#include <ossia/network/osc/detail/osc_packet_processor.hpp>
#include <ossia/network/osc/detail/osc_messages.hpp>

#include <ossia/detail/buffer_pool.hpp>

#include <boost/endian/conversion.hpp>
#include <oscpack/osc/OscOutboundPacketStream.h>
#include <oscpack/osc/OscReceivedElements.h>

namespace ossia::net
{
// TODO have an OutboundPacketStream which knows at construction the length of the address
// and typetag to allow to skip some mallocs, memcpy, memmove

// Since most of the time we have known types, we can optimize a fair bit the
// writing of the osc messages.
// Mirrors ossia::net::osc_outbound_array_visitor
struct osc_value_write_visitor
{
  std::size_t operator()(char* buffer, float v) const noexcept
  {
    buffer[0] = ',';
    buffer[1] = oscpack::FLOAT_TYPE_TAG;
    buffer[2] = '\0';
    buffer[3] = '\0';

    boost::endian::endian_store<float, 4, boost::endian::order::big>((unsigned char*) buffer + 4, v);
    return 8;
  }

  std::size_t operator()(char* buffer, int v) const noexcept
  {
    buffer[0] = ',';
    buffer[1] = oscpack::INT32_TYPE_TAG;
    buffer[2] = '\0';
    buffer[3] = '\0';

    boost::endian::endian_store<int32_t, 4, boost::endian::order::big>((unsigned char*) buffer + 4, v);
    return 8;
  }

  std::size_t operator()(char* buffer, ossia::impulse v) const noexcept
  {
    buffer[0] = ',';
    buffer[1] = '\0';
    buffer[2] = '\0';
    buffer[3] = '\0';

    return 4;
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

  std::size_t operator()(char* buffer, bool v) const noexcept
  {
    return operator()(buffer, int32_t{v});
  }

  std::size_t operator()(char* buffer, char v) const noexcept
  {
    return operator()(buffer, int32_t{v});
  }

  std::size_t operator()(char* buffer, vec2f v) const noexcept
  {
    buffer[0] = ',';
    buffer[1] = oscpack::FLOAT_TYPE_TAG;
    buffer[2] = oscpack::FLOAT_TYPE_TAG;
    buffer[3] = '\0';

    boost::endian::endian_store<float, 4, boost::endian::order::big>((unsigned char*) buffer + 4, v[0]);
    boost::endian::endian_store<float, 4, boost::endian::order::big>((unsigned char*) buffer + 8, v[1]);

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

    boost::endian::endian_store<float, 4, boost::endian::order::big>((unsigned char*) buffer + 8, v[0]);
    boost::endian::endian_store<float, 4, boost::endian::order::big>((unsigned char*) buffer + 12, v[1]);
    boost::endian::endian_store<float, 4, boost::endian::order::big>((unsigned char*) buffer + 16, v[2]);

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

    boost::endian::endian_store<float, 4, boost::endian::order::big>((unsigned char*) buffer + 8, v[0]);
    boost::endian::endian_store<float, 4, boost::endian::order::big>((unsigned char*) buffer + 12, v[1]);
    boost::endian::endian_store<float, 4, boost::endian::order::big>((unsigned char*) buffer + 16, v[2]);
    boost::endian::endian_store<float, 4, boost::endian::order::big>((unsigned char*) buffer + 20, v[3]);

    return 24;
  }
};


template<typename Sender>
struct osc_value_send_visitor
{
  ossia::string_view address_pattern;
  Sender& sender;

  template<typename T>
  void operator()(T v) const noexcept
  {
    const std::size_t sz = pattern_size(address_pattern.size()) + 8 + oscpack::RoundUp4(sizeof(v));
    char* buffer = (char*) alloca(sz);
    std::size_t i = write_string(address_pattern, buffer);

    i += osc_value_write_visitor{}(buffer + i, v);

    sender.send(buffer, i);
  }

  void operator()(const std::string& v) const noexcept
  {
    const std::size_t sz = pattern_size(address_pattern.size()) + 4 + pattern_size(v.size());
    if(sz < 16384) {
      char* buffer = (char*) alloca(sz);
      std::size_t i = write_string(address_pattern, buffer);

      i += osc_value_write_visitor{}(buffer + i, v);

      sender.send(buffer, i);
    }
    else
    {
      auto& pool = buffer_pool::instance();
      auto buffer = pool.acquire();
      buffer.resize(sz);
      std::size_t i = write_string(address_pattern, buffer.data());

      i += osc_value_write_visitor{}(buffer.data() + i, v);

      sender.send(buffer.data(), i);
    }
  }

  void operator()(const std::vector<ossia::value>& v) const noexcept
  {
    auto& pool = buffer_pool::instance();
    auto buf = pool.acquire();
    while (buf.size() < max_osc_message_size)
    {
      try
      {
        oscpack::OutboundPacketStream p{buf.data(), buf.size()};
        oscpack::osc_message_generator<osc_outbound_visitor> generate_message{p};
        generate_message(address_pattern, v);
        sender.send(p.Data(), p.Size());
        break;
      }
      catch(...)
      {
        auto n = buf.size();
        buf.clear();
        buf.resize(n * 2 + 1);
      }
    }

    pool.release(std::move(buf));
  }

  void operator()()
  {

  }
};
}
