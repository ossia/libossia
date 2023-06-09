#pragma once
#include <ossia/detail/parse_strict.hpp>
#include <ossia/detail/string_view.hpp>
#include <ossia/detail/to_string.hpp>
#include <ossia/network/base/parameter.hpp>
#include <ossia/network/common/value_bounding.hpp>
#include <ossia/network/domain/domain.hpp>
#include <ossia/network/osc/detail/osc_1_0_policy.hpp>
#include <ossia/network/osc/detail/osc_fwd.hpp>
#include <ossia/network/value/value.hpp>

#include <oscpack/osc/OscOutboundPacketStream.h>
#include <oscpack/osc/OscReceivedElements.h>
namespace ossia::net
{

struct osc_utilities
{
  static float get_float(oscpack::ReceivedMessageArgumentIterator it, float f)
  {
    switch(it->TypeTag())
    {
      case oscpack::INT32_TYPE_TAG:
        return it->AsInt32Unchecked();
      case oscpack::INT64_TYPE_TAG:
        return it->AsInt64Unchecked();
      case oscpack::FLOAT_TYPE_TAG:
        return it->AsFloatUnchecked();
      case oscpack::DOUBLE_TYPE_TAG:
        return it->AsDoubleUnchecked();
      case oscpack::TIME_TAG_TYPE_TAG:
        return it->AsTimeTagUnchecked();
      case oscpack::CHAR_TYPE_TAG:
        return it->AsCharUnchecked();
      case oscpack::TRUE_TYPE_TAG:
        return 1.f;
      case oscpack::FALSE_TYPE_TAG:
        return 0.f;
      case oscpack::STRING_TYPE_TAG:
        if(auto res = parse_strict<float>(it->AsStringUnchecked()))
          return *res;
        return f;
      case oscpack::SYMBOL_TYPE_TAG:
        if(auto res = parse_strict<float>(it->AsSymbolUnchecked()))
          return *res;
        return f;
      default:
        return f;
    }
  }

  static int get_int(oscpack::ReceivedMessageArgumentIterator it, int i)
  {
    switch(it->TypeTag())
    {
      case oscpack::INT32_TYPE_TAG:
        return it->AsInt32Unchecked();
      case oscpack::INT64_TYPE_TAG:
        return int32_t(it->AsInt64Unchecked());
      case oscpack::FLOAT_TYPE_TAG:
        return int32_t(it->AsFloatUnchecked());
      case oscpack::DOUBLE_TYPE_TAG:
        return int32_t(it->AsDoubleUnchecked());
      case oscpack::TIME_TAG_TYPE_TAG:
        return int32_t(it->AsTimeTagUnchecked());
      case oscpack::CHAR_TYPE_TAG:
        return int32_t(it->AsCharUnchecked());
      case oscpack::TRUE_TYPE_TAG:
        return 1;
      case oscpack::FALSE_TYPE_TAG:
        return 0;
      case oscpack::STRING_TYPE_TAG:
        if(auto res = parse_strict<int>(it->AsStringUnchecked()))
          return *res;
        return i;
      case oscpack::SYMBOL_TYPE_TAG:
        if(auto res = parse_strict<int>(it->AsSymbolUnchecked()))
          return *res;
        return i;
      default:
        return i;
    }
  }

  static bool get_bool(oscpack::ReceivedMessageArgumentIterator it, bool b)
  {
    using namespace std::literals;
    switch(it->TypeTag())
    {
      case oscpack::INT32_TYPE_TAG:
        return it->AsInt32Unchecked();
      case oscpack::INT64_TYPE_TAG:
        return it->AsInt64Unchecked();
      case oscpack::FLOAT_TYPE_TAG:
        return it->AsFloatUnchecked();
      case oscpack::DOUBLE_TYPE_TAG:
        return it->AsDoubleUnchecked();
      case oscpack::TIME_TAG_TYPE_TAG:
        return it->AsTimeTagUnchecked();
      case oscpack::CHAR_TYPE_TAG:
        return it->AsCharUnchecked();
      case oscpack::TRUE_TYPE_TAG:
        return true;
      case oscpack::FALSE_TYPE_TAG:
        return false;
      case oscpack::STRING_TYPE_TAG:
        return (std::string_view(it->AsStringUnchecked()) == "1"sv);
      case oscpack::SYMBOL_TYPE_TAG:
        return (std::string_view(it->AsSymbolUnchecked()) == "1"sv);
      default:
        return b;
    }
  }

  static char get_char(oscpack::ReceivedMessageArgumentIterator it, char c)
  {
    switch(it->TypeTag())
    {
      case oscpack::INT32_TYPE_TAG:
        return char(it->AsInt32Unchecked());
      case oscpack::INT64_TYPE_TAG:
        return char(it->AsInt64Unchecked());
      case oscpack::FLOAT_TYPE_TAG:
        return char(it->AsFloatUnchecked());
      case oscpack::DOUBLE_TYPE_TAG:
        return char(it->AsDoubleUnchecked());
      case oscpack::TIME_TAG_TYPE_TAG:
        return char(it->AsTimeTagUnchecked());
      case oscpack::CHAR_TYPE_TAG:
        return char(it->AsCharUnchecked());
      case oscpack::TRUE_TYPE_TAG:
        return char{'T'};
      case oscpack::FALSE_TYPE_TAG:
        return char{'F'};
      case oscpack::STRING_TYPE_TAG:
        return char{it->AsStringUnchecked()[0]};
      case oscpack::SYMBOL_TYPE_TAG:
        return char{it->AsSymbolUnchecked()[0]};
      default:
        return c;
    }
  }

  static std::string get_blob(oscpack::ReceivedMessageArgumentIterator it)
  {
    const void* data{};
    oscpack::osc_bundle_element_size_t size{};
    it->AsBlobUnchecked(data, size);
    if(size > 0)
      return std::string{(const char*)data, (std::size_t)size};
    return std::string{};
  }

  static ossia::value create_value(oscpack::ReceivedMessageArgumentIterator it)
  {
    switch(it->TypeTag())
    {
      case oscpack::INT32_TYPE_TAG:
        return int32_t{it->AsInt32Unchecked()};
      case oscpack::INT64_TYPE_TAG:
        return int32_t{(int)it->AsInt64Unchecked()};
      case oscpack::FLOAT_TYPE_TAG:
        return float{it->AsFloatUnchecked()};
      case oscpack::DOUBLE_TYPE_TAG:
        return float{(float)it->AsDoubleUnchecked()};
      case oscpack::TIME_TAG_TYPE_TAG:
        return int32_t(it->AsTimeTagUnchecked());
      case oscpack::CHAR_TYPE_TAG:
        return char{it->AsCharUnchecked()};
      case oscpack::TRUE_TYPE_TAG:
        return bool{true};
      case oscpack::FALSE_TYPE_TAG:
        return bool{false};
      case oscpack::STRING_TYPE_TAG:
        return std::string{it->AsStringUnchecked()};
      case oscpack::SYMBOL_TYPE_TAG:
        return std::string{it->AsSymbolUnchecked()};
      case oscpack::BLOB_TYPE_TAG:
        return get_blob(it);
      case oscpack::RGBA_COLOR_TYPE_TAG: {
        auto c = it->AsRgbaColorUnchecked();
        return make_vec(
            uint8_t(c >> 24 & 0xFF), uint8_t(c >> 16 & 0xFF), uint8_t(c >> 8 & 0xFF),
            uint8_t(c & 0xFF));
      }
      default:
        return ossia::impulse{};
    }
  }

  static std::vector<ossia::value> create_list_(
      oscpack::ReceivedMessageArgumentIterator& it,
      oscpack::ReceivedMessageArgumentIterator& end)
  {
    std::vector<ossia::value> t;
    for(; it != end; ++it)
    {
      switch(it->TypeTag())
      {
        case oscpack::INT32_TYPE_TAG:
          t.emplace_back(int32_t{it->AsInt32Unchecked()});
          break;
        case oscpack::INT64_TYPE_TAG:
          t.emplace_back(int32_t{(int)it->AsInt64Unchecked()});
          break;
        case oscpack::FLOAT_TYPE_TAG:
          t.emplace_back(float{it->AsFloatUnchecked()});
          break;
        case oscpack::DOUBLE_TYPE_TAG:
          t.emplace_back(float{(float)it->AsDoubleUnchecked()});
          break;
        case oscpack::TIME_TAG_TYPE_TAG:
          t.emplace_back(int32_t(it->AsTimeTagUnchecked()));
          break;
        case oscpack::CHAR_TYPE_TAG:
          t.emplace_back(char{it->AsCharUnchecked()});
          break;
        case oscpack::TRUE_TYPE_TAG:
          t.emplace_back(bool{true});
          break;
        case oscpack::FALSE_TYPE_TAG:
          t.emplace_back(bool{false});
          break;
        case oscpack::STRING_TYPE_TAG:
          t.push_back(std::string{it->AsStringUnchecked()});
          break;
        case oscpack::SYMBOL_TYPE_TAG:
          t.push_back(std::string{it->AsSymbolUnchecked()});
          break;
        case oscpack::BLOB_TYPE_TAG:
          t.emplace_back(get_blob(it));
          break;
        case oscpack::RGBA_COLOR_TYPE_TAG: {
          auto c = it->AsRgbaColorUnchecked();
          t.emplace_back(make_vec(
              uint8_t(c >> 24 & 0xFF), uint8_t(c >> 16 & 0xFF), uint8_t(c >> 8 & 0xFF),
              uint8_t(c & 0xFF)));
          break;
        }
        case oscpack::ARRAY_BEGIN_TYPE_TAG: {
          ++it;
          t.emplace_back(create_list_(it, end));
          break;
        }
        case oscpack::ARRAY_END_TYPE_TAG: {
          // don't call ++it here :  it will be increased in the parent's
          // for(....)
          return t;
        }
        default:
          t.push_back(ossia::impulse{});
          break;
      }
    }
    return t;
  }

  static std::vector<ossia::value> create_list(
      oscpack::ReceivedMessageArgumentIterator it,
      oscpack::ReceivedMessageArgumentIterator end)
  {
    return create_list_(it, end);
  }

  static ossia::value create_any(
      oscpack::ReceivedMessageArgumentIterator cur_it,
      oscpack::ReceivedMessageArgumentIterator end, int numArguments)
  {
    switch(numArguments)
    {
      case 0:
        return ossia::impulse{};
      case 1:
        return create_value(cur_it);
      default:
        return value{create_list(cur_it, end)};
    }
  }
};

struct osc_inbound_visitor
{
  osc_inbound_visitor(
      oscpack::ReceivedMessageArgumentIterator cur,
      oscpack::ReceivedMessageArgumentIterator beg,
      oscpack::ReceivedMessageArgumentIterator end, int n = 1)
      : cur_it{cur}
      , beg_it{beg}
      , end_it{end}
      , numArguments{n}
  {
  }

  oscpack::ReceivedMessageArgumentIterator cur_it;
  oscpack::ReceivedMessageArgumentIterator beg_it;
  oscpack::ReceivedMessageArgumentIterator end_it;
  int numArguments = 1;

  ossia::value operator()(ossia::impulse imp) const { return imp; }

  ossia::value operator()(int32_t i) const { return osc_utilities::get_int(cur_it, i); }

  ossia::value operator()(float f) const { return osc_utilities::get_float(cur_it, f); }

  ossia::value operator()(bool b) const { return osc_utilities::get_bool(cur_it, b); }

  ossia::value operator()(char c) const { return osc_utilities::get_char(cur_it, c); }

  ossia::value operator()(const std::string& str) const
  {
    switch(cur_it->TypeTag())
    {
      case oscpack::INT32_TYPE_TAG:
        return ossia::to_string(cur_it->AsInt32Unchecked());
      case oscpack::INT64_TYPE_TAG:
        return ossia::to_string(cur_it->AsInt64Unchecked());
      case oscpack::FLOAT_TYPE_TAG:
        return ossia::to_string(cur_it->AsFloatUnchecked());
      case oscpack::DOUBLE_TYPE_TAG:
        return ossia::to_string(cur_it->AsDoubleUnchecked());
      case oscpack::CHAR_TYPE_TAG:
        return std::string(1, cur_it->AsCharUnchecked());
      case oscpack::TRUE_TYPE_TAG:
        return std::string{"true"};
      case oscpack::FALSE_TYPE_TAG:
        return std::string{"false"};
      case oscpack::STRING_TYPE_TAG:
        return std::string{cur_it->AsStringUnchecked()};
      case oscpack::SYMBOL_TYPE_TAG:
        return std::string{cur_it->AsSymbolUnchecked()};
      default:
        return str;
    }
  }

  template <std::size_t N>
  ossia::value operator()(std::array<float, N> vec) const
  {
    if(numArguments == N)
    {
      std::array<float, N> ret;
      std::size_t i = 0;
      auto vec_it = beg_it;
      auto vec_end = end_it;
      for(; vec_it != vec_end; ++vec_it)
      {
        ret[i] = osc_utilities::get_float(vec_it, vec[i]);
        i++;
      }
      return ret;
    }
    else
    {
      if constexpr(N == 4)
      {
        if(cur_it->TypeTag() == oscpack::RGBA_COLOR_TYPE_TAG)
        {
          auto c = cur_it->AsRgbaColorUnchecked();
          return make_vec(
              uint8_t(c >> 24 & 0xFF), uint8_t(c >> 16 & 0xFF), uint8_t(c >> 8 & 0xFF),
              uint8_t(c & 0xFF));
        }
      }
      return vec;
    }
  }

  ossia::value operator()(const std::vector<ossia::value>&)
  {
    /* This code preserves type info, this is not what we want.
  int n = t.size();
  if (numArguments == n)
  {
    for (int i = 0; i < n; i++)
    {
      auto res = ossia::apply_nonnull(*this, t[i].v);
      t[i] = std::move(res);
      ++cur_it;
    }
  }
  */
    return value{osc_utilities::create_list(cur_it, end_it)};
  }

  ossia::value operator()(const value_map_type&) { return value{value_map_type{}}; }

  ossia::value operator()() const { return {}; }
};

struct osc_inbound_impulse_visitor
{
  // If our address is any type, e.g. float, etc., we treat an "empty" message as a bang on it
  template <typename T>
  ossia::value operator()(T&& t) const
  {
    return ossia::value{std::forward<T>(t)};
  }

  // If the address is a list otoh, it means that we're getting an empty list
  ossia::value operator()(const std::vector<ossia::value>& t) const
  {
    return ossia::value{std::vector<ossia::value>{}};
  }

  ossia::value operator()() const { return {}; }
};

inline ossia::value to_value(
    const ossia::value& current, oscpack::ReceivedMessageArgumentIterator beg_it,
    oscpack::ReceivedMessageArgumentIterator end_it, int N)
{
  if(beg_it != end_it)
    return current.apply(osc_inbound_visitor{beg_it, beg_it, end_it, N});
  else
    return current.apply(osc_inbound_impulse_visitor{});
}

inline ossia::value get_filtered_value(
    ossia::net::parameter_base& addr, oscpack::ReceivedMessageArgumentIterator beg_it,
    oscpack::ReceivedMessageArgumentIterator end_it, int N)
{
  return bound_value(
      addr.get_domain(), ossia::net::to_value(addr.value(), beg_it, end_it, N),
      addr.get_bounding());
}

inline ossia::value get_filtered_value(
    ossia::net::parameter_base& addr, const oscpack::ReceivedMessage& mess)
{
  return get_filtered_value(
      addr, mess.ArgumentsBegin(), mess.ArgumentsEnd(), mess.ArgumentCount());
}

struct osc_write_domain_visitor
{
  ossia::net::osc_1_0_outbound_stream_visitor vis;
  template <typename T>
  void operator()(const T& dom)
  {
    if(dom.min && dom.max)
    {
      vis(*dom.min);
      vis(*dom.max);
    }
  }

  template <std::size_t N>
  void operator()(const vecf_domain<N>& dom)
  {
    if(dom.min[0] && dom.max[0])
    {
      vis(*dom.min[0]);
      vis(*dom.max[0]);
    }
  }

  void operator()(const vector_domain& dom)
  {
    if(!dom.min.empty() && !dom.max.empty() && dom.min[0].valid() && dom.max[0].valid())
    {
      vis(ossia::convert<float>(dom.min[0]));
      vis(ossia::convert<float>(dom.max[0]));
    }
  }

  void operator()(const domain_base<ossia::value>& dom)
  {
    if(dom.min && dom.max)
    {
      vis(ossia::convert<float>(*dom.min));
      vis(ossia::convert<float>(*dom.max));
    }
  }

  void operator()(const domain_base<bool>& dom)
  {
    vis(false);
    vis(true);
  }

  void operator()(const domain_base<ossia::impulse>& dom) { }

  void operator()(const domain_base<std::string>& dom) { }

  void operator()() { }
};
}

namespace oscpack
{
/*
inline oscpack::OutboundPacketStream&
operator<<(oscpack::OutboundPacketStream& p, const ossia::value& val)
{
  val.apply(ossia::net::osc_outbound_visitor{p});
  return p;
}
*/

inline oscpack::OutboundPacketStream&
operator<<(oscpack::OutboundPacketStream& p, const ossia::domain& dom)
{
  ossia::apply(
      ossia::net::osc_write_domain_visitor{
          ossia::net::osc_1_0_outbound_stream_visitor{p, ossia::unit_t{}}},
      dom);

  return p;
}
}
