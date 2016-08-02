#pragma once
#include <ossia/editor/value/value.hpp>
#include <ossia/network/base/address.hpp>
#include <ossia/network/domain/domain.hpp>
#include <ossia/network/generic/generic_address.hpp>
#include <ossia/network/osc/detail/string_view.hpp>
#include <oscpack/osc/OscOutboundPacketStream.h>
#include <oscpack/osc/OscReceivedElements.h>

namespace oscpack
{
inline oscpack::OutboundPacketStream&
operator<<(oscpack::OutboundPacketStream& p, const ossia::value& val);
}

namespace ossia
{
namespace net
{
struct OSCOutboundVisitor
{
  oscpack::OutboundPacketStream& p;
  void operator()(ossia::Impulse) const
  {
  }
  void operator()(ossia::Int i) const
  {
    p << int32_t(i.value);
  }
  void operator()(ossia::Float f) const
  {
    p << f.value;
  }
  void operator()(ossia::Bool b) const
  {
    p << int32_t(b.value);
  }
  void operator()(ossia::Char c) const
  {
    p << int32_t(c.value);
  }
  void operator()(const ossia::String& str) const
  {
    p << boost::string_ref(str.value);
  }
  void operator()(ossia::Vec2f vec) const
  {
    p << vec.value[0] << vec.value[1];
  }
  void operator()(ossia::Vec3f vec) const
  {
    p << vec.value[0] << vec.value[1] << vec.value[2];
  }
  void operator()(ossia::Vec4f vec) const
  {
    p << vec.value[0] << vec.value[1] << vec.value[2];
  }
  void operator()(const ossia::Destination& d) const
  {
  }
  void operator()(const ossia::Behavior&) const
  {
  }
  void operator()(const ossia::Tuple& t) const
  {
    for (const auto& val : t.value)
    {
      p << val;
    }
  }

  void operator()() const
  {
  }
};

struct OSCInboundVisitor
{
  OSCInboundVisitor(
    oscpack::ReceivedMessageArgumentIterator cur, 
    oscpack::ReceivedMessageArgumentIterator beg, 
    oscpack::ReceivedMessageArgumentIterator end, 
    int n = 1): cur_it{cur}, beg_it{beg}, end_it{end}, numArguments{n} {}

  oscpack::ReceivedMessageArgumentIterator cur_it;
  oscpack::ReceivedMessageArgumentIterator beg_it;
  oscpack::ReceivedMessageArgumentIterator end_it;
  int numArguments = 1;

  ossia::value operator()(ossia::Impulse imp) const
  {
    return imp;
  }
  ossia::value operator()(ossia::Int i) const
  {
    switch (cur_it->TypeTag())
    {
      case oscpack::INT32_TYPE_TAG:
        return ossia::Int{cur_it->AsInt32Unchecked()};
      case oscpack::INT64_TYPE_TAG:
        return ossia::Int{int32_t(cur_it->AsInt64Unchecked())};
      case oscpack::FLOAT_TYPE_TAG:
        return ossia::Int{int32_t(cur_it->AsFloatUnchecked())};
      case oscpack::DOUBLE_TYPE_TAG:
        return ossia::Int{int32_t(cur_it->AsDoubleUnchecked())};
      case oscpack::CHAR_TYPE_TAG:
        return ossia::Int{int32_t(cur_it->AsCharUnchecked())};
      case oscpack::TRUE_TYPE_TAG:
        return ossia::Int{1};
      case oscpack::FALSE_TYPE_TAG:
        return ossia::Int{0};
      default:
        return i;
    }
  }
  ossia::value operator()(ossia::Float f) const
  {
    switch (cur_it->TypeTag())
    {
      case oscpack::INT32_TYPE_TAG:
        return ossia::Float{float(cur_it->AsInt32Unchecked())};
      case oscpack::INT64_TYPE_TAG:
        return ossia::Float{float(cur_it->AsInt64Unchecked())};
      case oscpack::FLOAT_TYPE_TAG:
        return ossia::Float{float(cur_it->AsFloatUnchecked())};
      case oscpack::DOUBLE_TYPE_TAG:
        return ossia::Float{float(cur_it->AsDoubleUnchecked())};
      case oscpack::CHAR_TYPE_TAG:
        return ossia::Float{float(cur_it->AsCharUnchecked())};
      case oscpack::TRUE_TYPE_TAG:
        return ossia::Float{1.};
      case oscpack::FALSE_TYPE_TAG:
        return ossia::Float{0.};
      default:
        return f;
    }
  }

  ossia::value operator()(ossia::Bool b) const
  {
    switch (cur_it->TypeTag())
    {
      case oscpack::INT32_TYPE_TAG:
        return ossia::Bool{bool(cur_it->AsInt32Unchecked())};
      case oscpack::INT64_TYPE_TAG:
        return ossia::Bool{bool(cur_it->AsInt64Unchecked())};
      case oscpack::FLOAT_TYPE_TAG:
        return ossia::Bool{bool(cur_it->AsFloatUnchecked())};
      case oscpack::DOUBLE_TYPE_TAG:
        return ossia::Bool{bool(cur_it->AsDoubleUnchecked())};
      case oscpack::CHAR_TYPE_TAG:
        return ossia::Bool{bool(cur_it->AsCharUnchecked())};
      case oscpack::TRUE_TYPE_TAG:
        return ossia::Bool{true};
      case oscpack::FALSE_TYPE_TAG:
        return ossia::Bool{false};
      default:
        return b;
    }
  }
  ossia::value operator()(ossia::Char c) const
  {
    switch (cur_it->TypeTag())
    {
      case oscpack::INT32_TYPE_TAG:
        return ossia::Char{char(cur_it->AsInt32Unchecked())};
      case oscpack::INT64_TYPE_TAG:
        return ossia::Char{char(cur_it->AsInt64Unchecked())};
      case oscpack::FLOAT_TYPE_TAG:
        return ossia::Char{char(cur_it->AsFloatUnchecked())};
      case oscpack::DOUBLE_TYPE_TAG:
        return ossia::Char{char(cur_it->AsDoubleUnchecked())};
      case oscpack::CHAR_TYPE_TAG:
        return ossia::Char{char(cur_it->AsCharUnchecked())};
      case oscpack::TRUE_TYPE_TAG:
        return ossia::Char{'T'};
      case oscpack::FALSE_TYPE_TAG:
        return ossia::Char{'F'};
      case oscpack::STRING_TYPE_TAG:
        return ossia::Char{cur_it->AsStringUnchecked()[0]};
      case oscpack::SYMBOL_TYPE_TAG:
        return ossia::Char{cur_it->AsSymbolUnchecked()[0]};
      default:
        return c;
    }
  }

  ossia::value operator()(const ossia::String& str) const
  {
    switch (cur_it->TypeTag())
    {
      case oscpack::INT32_TYPE_TAG:
        return ossia::String{std::to_string(cur_it->AsInt32Unchecked())};
      case oscpack::INT64_TYPE_TAG:
        return ossia::String{std::to_string(cur_it->AsInt64Unchecked())};
      case oscpack::FLOAT_TYPE_TAG:
        return ossia::String{std::to_string(cur_it->AsFloatUnchecked())};
      case oscpack::DOUBLE_TYPE_TAG:
        return ossia::String{std::to_string(cur_it->AsDoubleUnchecked())};
      case oscpack::CHAR_TYPE_TAG:
        return ossia::String{std::to_string(cur_it->AsCharUnchecked())};
      case oscpack::TRUE_TYPE_TAG:
        return ossia::String{"true"};
      case oscpack::FALSE_TYPE_TAG:
        return ossia::String{"false"};
      case oscpack::STRING_TYPE_TAG:
        return ossia::String{cur_it->AsStringUnchecked()};
      case oscpack::SYMBOL_TYPE_TAG:
        return ossia::String{cur_it->AsSymbolUnchecked()};
      default:
        return str;
    }
  }

  template <int N>
  ossia::value operator()(ossia::Vec<float, N> vec) const
  {
    if (numArguments == N)
    {
      ossia::Vec<float, N> ret;
      int i = 0;
      auto vec_it = beg_it;
      auto vec_end = end_it;
      for (; vec_it != vec_end; ++vec_it)
      {
        if (vec_it->IsFloat())
          ret.value[i] = vec_it->AsFloatUnchecked();
        else if (vec_it->IsDouble())
          ret.value[i] = vec_it->AsDoubleUnchecked();
        else
          return vec;

        i++;
      }
      return ret;
    }
    else
    {
      return vec;
    }
  }

  ossia::value operator()(const ossia::Destination& d) const
  {
    return d;
  }
  ossia::value operator()(const ossia::Behavior& b) const
  {
    return b;
  }
  ossia::value operator()(ossia::Tuple t)
  {
    int n = t.value.size();
    if (numArguments == n)
    {
      for (int i = 0; i < n; i++)
      {
        auto res = eggs::variants::apply(*this, t.value[i].v);
        t.value[i] = std::move(res);
        ++cur_it;
      }
    }
    return std::move(t);
  }

  ossia::value operator()() const
  {
    return {};
  }
};

inline ossia::value filterValue(
    const ossia::net::domain& dom, const ossia::value& base_val,
    ossia::bounding_mode mode)
{
  if (dom)
  {
    auto res = ossia::net::clamp(dom, mode, base_val);
    if (res.valid())
      return res;
    else
      return {};
  }
  else
  {
    return base_val;
  }
}

inline ossia::value filterValue(const ossia::net::generic_address& addr)
{
  auto val = addr.cloneValue();
  if (addr.getRepetitionFilter() == ossia::repetition_filter::ON
      && val == addr.PreviousValue)
    return {};

  return filterValue(
      addr.getDomain(), std::move(val), addr.getBoundingMode());
}

inline boost::string_ref getOSCAddress(const ossia::net::address_base& address)
{
  auto& addr = address.getTextualAddress();
  auto begin = addr.find(':') + 1;
  return boost::string_ref(addr.data() + begin, addr.size() - begin);
}

inline std::string
getOSCAddressAsString(const ossia::net::address_base& address)
{
  auto& addr = address.getTextualAddress();
  return addr.substr(addr.find(':') + 1);
}

inline std::string getOSCAddressAsString(const ossia::net::node_base& node)
{
  auto addr = ossia::net::address_string_from_node(node);
  return addr.substr(addr.find(':') + 1);
}

inline ossia::value toValue(
    const ossia::value& current,
    oscpack::ReceivedMessageArgumentIterator beg_it,
    oscpack::ReceivedMessageArgumentIterator end_it, int N)
{
  return current.apply(OSCInboundVisitor{beg_it, beg_it, end_it, N});
}

inline void updateValue(
    ossia::net::address_base& addr,
    oscpack::ReceivedMessageArgumentIterator beg_it,
    oscpack::ReceivedMessageArgumentIterator end_it, int N)
{
  if (addr.getAccessMode() == ossia::access_mode::SET)
    return;

  auto res = filterValue(
      addr.getDomain(),
      ossia::net::toValue(addr.cloneValue(), beg_it, end_it, N),
      addr.getBoundingMode());

  if (res.valid())
  {
    addr.setValue(std::move(res));
  }
}

inline void updateValue(
    ossia::net::address_base& addr, const oscpack::ReceivedMessage& mess)
{
  return updateValue(
      addr, mess.ArgumentsBegin(), mess.ArgumentsEnd(), mess.ArgumentCount());
}
}
}

namespace oscpack
{
inline oscpack::OutboundPacketStream&
operator<<(oscpack::OutboundPacketStream& p, const ossia::value& val)
{
  val.apply(ossia::net::OSCOutboundVisitor{p});
  return p;
}

inline oscpack::OutboundPacketStream&
operator<<(oscpack::OutboundPacketStream& p, const ossia::net::domain& dom)
{
  auto dom_min = ossia::net::min(dom);
  auto dom_max = ossia::net::max(dom);
  if (bool(dom_min.v) && bool(dom_max.v))
    p << dom_min << dom_max;

  return p;
}
}
