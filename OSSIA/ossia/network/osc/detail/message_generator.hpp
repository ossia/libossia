#pragma once
#include <ossia/network/osc/detail/string_view.hpp>
#include <boost/container/small_vector.hpp>
#include <oscpack/osc/OscOutboundPacketStream.h>
#include <ossia/network/base/address.hpp>

#include <array>
#include <iostream>
#include <vector>
namespace oscpack
{
inline oscpack::OutboundPacketStream& operator<<(
    oscpack::OutboundPacketStream& p,
    const std::vector<ossia::string_view>& values)
{
  for (auto val : values)
  {
    p << val;
  }

  return p;
}

inline oscpack::OutboundPacketStream& operator<<(
    oscpack::OutboundPacketStream& p,
    const ossia::net::address_base& address)
{
  auto addr = ossia::net::address_string_from_node(address);
  auto begin = addr.find(':') + 1;
  p << ossia::string_view(addr.data() + begin, addr.size() - begin);

  return p;
}

inline oscpack::OutboundPacketStream& operator<<(
    oscpack::OutboundPacketStream& p,
    const std::vector<std::string>& values)
{
  for (const auto& val : values)
  {
    p << ossia::string_view(val);
  }

  return p;
}


template <int BufferSize = 2048>
class MessageGenerator
{
public:
  MessageGenerator() = default;

  template <typename... T>
  MessageGenerator(const std::string& name, const T&... args)
  {
    operator()(name, args...);
  }

  void clear()
  {
    std::fill_n(buffer.begin(), p.Size(), 0);
    p.Clear();
  }

  template <typename... T>
  const oscpack::OutboundPacketStream&
  operator()(const std::string& name, const T&... args)
  {
    p << oscpack::BeginMessageN(name);
    subfunc(args...);
    p << oscpack::EndMessage();
    return p;
  }

  template <typename... T>
  const oscpack::OutboundPacketStream&
  operator()(ossia::string_view name, const T&... args)
  {
    p << oscpack::BeginMessageN(name);
    subfunc(args...);
    p << oscpack::EndMessage();
    return p;
  }

  template <int N, typename... T>
  const oscpack::OutboundPacketStream&
  operator()(small_string_base<N> name, const T&... args)
  {
    p << oscpack::BeginMessageN(name);
    subfunc(args...);
    p << oscpack::EndMessage();
    return p;
  }

  template <typename Val_T>
  const oscpack::OutboundPacketStream&
  operator()(const std::string& name, const std::vector<Val_T>& values)
  {
    p << oscpack::BeginMessageN(name) << values << oscpack::EndMessage();
    return p;
  }

  const oscpack::OutboundPacketStream& stream() const
  {
    return p;
  }

private:
  void subfunc()
  {
  }

  template <typename Arg1, typename... Args>
  void subfunc(Arg1&& arg1, Args&&... args)
  {
    static_assert(
        !std::is_pointer<std::remove_cv_t<std::remove_reference_t<Arg1>>>::
            value,
        "Do not send raw string literals");
    p << arg1;
    subfunc(args...);
  }

  alignas(128) std::array<char, BufferSize> buffer;
  oscpack::OutboundPacketStream p{buffer.data(), buffer.size()};
};


// TODO have a queue of dynamic messages
class DynamicMessageGenerator
{
public:
  DynamicMessageGenerator() = default;

  template <typename... T>
  DynamicMessageGenerator(
      const std::string& name,
      const T&... args)
  {
    operator()(name, args...);
  }

  template <typename... T>
  const oscpack::OutboundPacketStream&
  operator()(const std::string& name, const T&... args)
  {
    p << oscpack::BeginMessageN(name);
    subfunc(args...);
    p << oscpack::EndMessage();
    return p;
  }

  template <typename... T>
  const oscpack::OutboundPacketStream&
  operator()(ossia::string_view name, const T&... args)
  {
    p << oscpack::BeginMessageN(name);
    subfunc(args...);
    p << oscpack::EndMessage();
    return p;
  }

  template <int N, typename... T>
  const oscpack::OutboundPacketStream&
  operator()(small_string_base<N> name, const T&... args)
  {
    p << oscpack::BeginMessageN(name);
    subfunc(args...);
    p << oscpack::EndMessage();
    return p;
  }

  template <typename Val_T>
  const oscpack::OutboundPacketStream&
  operator()(const std::string& name, const std::vector<Val_T>& values)
  {
    p << oscpack::BeginMessageN(name) << values << oscpack::EndMessage();
    return p;
  }

  const oscpack::OutboundPacketStream& stream() const
  {
    return p;
  }

private:
  void subfunc()
  {
  }

  template <typename Arg1, typename... Args>
  void subfunc(Arg1&& arg1, Args&&... args)
  {
    static_assert(
        !std::is_pointer<std::remove_cv_t<std::remove_reference_t<Arg1>>>::
            value,
        "Do not send raw string literals");
    p << arg1;
    subfunc(args...);
  }

  std::unique_ptr<char[]> buffer{std::make_unique<char[]>(1024*1024)};
  oscpack::OutboundPacketStream p{buffer.get(), 1024 * 1024};
};
}
