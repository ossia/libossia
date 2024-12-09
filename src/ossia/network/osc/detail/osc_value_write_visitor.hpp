#pragma once
#include <ossia/detail/buffer_pool.hpp>
#include <ossia/network/base/node_attributes.hpp>
#include <ossia/network/osc/detail/message_generator.hpp>
#include <ossia/network/osc/detail/osc_messages.hpp>
#include <ossia/network/osc/detail/osc_packet_processor.hpp>
#include <ossia/network/osc/detail/osc_utils.hpp>
#include <ossia/network/value/value.hpp>

#include <boost/endian/conversion.hpp>

#include <oscpack/osc/OscOutboundPacketStream.h>
#include <oscpack/osc/OscReceivedElements.h>

namespace ossia::net
{
template <typename Parameter, typename OscPolicy, typename Writer>
#if __cpp_lib_concepts >= 201907L
  requires std::is_invocable_v<Writer, const char*, std::size_t>
#endif
struct osc_value_send_visitor
{
  const Parameter& parameter;
  std::string_view address_pattern;
  Writer writer;

  using static_policy = typename OscPolicy::static_policy;
  using dynamic_policy = typename OscPolicy::dynamic_policy;

  template <typename T>
  void operator()(T v) const noexcept
  try
  {
    const std::size_t sz
        = pattern_size(address_pattern.size()) + 8 + oscpack::RoundUp4(sizeof(v));
    char* buffer = (char*)alloca(sz);
    std::size_t i = write_string(address_pattern, buffer);

    i += static_policy{parameter.get_unit()}(buffer + i, v);

    writer(buffer, i);
  }
  catch(const std::exception& e)
  {
    ossia::logger().error("osc_value_send_visitor: {}", e.what());
  }
  catch(...)
  {
    ossia::logger().error("osc_value_send_visitor: unknown error");
  }

  void operator()(ossia::impulse v) const noexcept
  try
  {
    const std::size_t sz
        = pattern_size(address_pattern.size()) + 8 + oscpack::RoundUp4(sizeof(v));
    char* buffer = (char*)alloca(sz);
    std::size_t i = write_string(address_pattern, buffer);

    if(auto ep = ossia::net::get_extended_type(parameter))
      i += static_policy{parameter.get_unit()}(buffer + i, v, *ep);
    else
      i += static_policy{parameter.get_unit()}(buffer + i, v);

    writer(buffer, i);
  }
  catch(const std::exception& e)
  {
    ossia::logger().error("osc_value_send_visitor: {}", e.what());
  }
  catch(...)
  {
    ossia::logger().error("osc_value_send_visitor: unknown error");
  }

  void operator()(const std::string& v) const noexcept
  try
  {
    const std::size_t sz
        = pattern_size(address_pattern.size()) + 4 + pattern_size(v.size());
    if(sz < 16384)
    {
      char* buffer = (char*)alloca(sz);
      std::size_t i = write_string(address_pattern, buffer);

      if(is_blob(parameter))
        i += static_policy{parameter.get_unit()}(
            buffer + i, oscpack::Blob(v.data(), v.size()));
      else
        i += static_policy{parameter.get_unit()}(buffer + i, v);

      writer(buffer, i);
    }
    else
    {
      auto& pool = buffer_pool::instance();
      auto buffer = pool.acquire();
      buffer.resize(sz);
      std::size_t i = write_string(address_pattern, buffer.data());

      if(is_blob(parameter))
        i += static_policy{parameter.get_unit()}(
            buffer.data() + i, oscpack::Blob(v.data(), v.size()));
      else
        i += static_policy{parameter.get_unit()}(buffer.data() + i, v);

      writer(buffer.data(), i);
    }
  }
  catch(const std::exception& e)
  {
    ossia::logger().error("osc_value_send_visitor: {}", e.what());
  }
  catch(...)
  {
    ossia::logger().error("osc_value_send_visitor: unknown error");
  }

  void operator()(const std::vector<ossia::value>& v) const noexcept
  try
  {
    auto& pool = buffer_pool::instance();
    auto buf = pool.acquire();
    while(buf.size() < max_osc_message_size)
    {
      try
      {
        oscpack::OutboundPacketStream p{buf.data(), buf.size()};

        p << oscpack::BeginMessageN(address_pattern);

        bool ok = false;
        if(auto ep = ossia::net::get_extended_type(parameter))
          ok = process_extended_array(*ep, p, v);
        if(!ok)
          dynamic_policy{{p, parameter.get_unit()}}(v);
        p << oscpack::EndMessage();

        writer(p.Data(), p.Size());
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
  catch(const std::exception& e)
  {
    ossia::logger().error("osc_value_send_visitor: {}", e.what());
  }
  catch(...)
  {
    ossia::logger().error("osc_value_send_visitor: unknown error");
  }

  void operator()(const value_map_type& v) const noexcept { }
  void operator()() { }

  bool process_extended_array(
      const ossia::extended_type& e, oscpack::OutboundPacketStream& out,
      const std::vector<ossia::value>& v) const
  {
    if(e == u8_blob_type())
    {
      std::string data;
      for(auto& val : v)
      {
        data += static_cast<unsigned char>(ossia::convert<int>(val));
      }

      oscpack::Blob b(data.data(), data.size());
      out << b;
      return true;
    }
    return false;
  }
};

template <typename Parameter, typename OscPolicy>
struct osc_value_write_visitor
{
  const Parameter& parameter;
  std::string_view address_pattern;
  ossia::buffer_pool::buffer& result;

  using static_policy = typename OscPolicy::static_policy;
  using dynamic_policy = typename OscPolicy::dynamic_policy;

  template <typename T>
  void operator()(T v) const noexcept
  {
    const std::size_t sz
        = pattern_size(address_pattern.size()) + 8 + oscpack::RoundUp4(sizeof(v));
    result.resize(sz);
    std::size_t i = write_string(address_pattern, result.data());

    i += static_policy{parameter.get_unit()}(result.data() + i, v);

    result.resize(i);
  }

  void operator()(const std::string& v) const noexcept
  {
    const std::size_t sz
        = pattern_size(address_pattern.size()) + 4 + pattern_size(v.size());
    result.resize(sz);
    std::size_t i = write_string(address_pattern, result.data());

    if(is_blob(parameter))
      i += static_policy{parameter.get_unit()}(
          result.data() + i, oscpack::Blob(v.data(), v.size()));
    else
      i += static_policy{parameter.get_unit()}(result.data() + i, v);

    result.resize(i);
  }

  void operator()(const std::vector<ossia::value>& v) const noexcept
  {
    // OPTIMIZEME
    while(result.size() < max_osc_message_size)
    {
      try
      {
        oscpack::OutboundPacketStream p{result.data(), result.size()};

        p << oscpack::BeginMessageN(address_pattern);
        dynamic_policy{{p, parameter.get_unit()}}(v);
        p << oscpack::EndMessage();

        result.resize(p.Size());
        break;
      }
      catch(...)
      {
        auto n = result.size();
        result.clear();
        result.resize(n * 2 + 1);
      }
    }
  }

  void operator()(const value_map_type& v) const noexcept { }

  void operator()() { }
};
}
