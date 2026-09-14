#include "resolve.hpp"

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/basic_resolver.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ip/udp.hpp>

namespace ossia
{

static bool is_local_v4(boost::asio::ip::address_v4 addr)
{
  struct range
  {
    uint32_t min, max;
  };
  static const range local_range[3]
      = {{
             .min = boost::asio::ip::make_address_v4("10.0.0.0").to_uint(),
             .max = boost::asio::ip::make_address_v4("10.255.255.255").to_uint(),
         },
         {
             .min = boost::asio::ip::make_address_v4("172.16.0.0").to_uint(),
             .max = boost::asio::ip::make_address_v4("172.31.255.255").to_uint(),
         },
         {
             .min = boost::asio::ip::make_address_v4("192.168.0.0").to_uint(),
             .max = boost::asio::ip::make_address_v4("192.168.255.255").to_uint(),
         }};

  auto ip = addr.to_uint();
  return (ip >= local_range[0].min && ip <= local_range[0].max)
         || (ip >= local_range[1].min && ip <= local_range[1].max)
         || (ip >= local_range[2].min && ip <= local_range[2].max);
}

static bool is_local_v6(boost::asio::ip::address_v6 addr)
{
  // fc00::/7 — Unique Local Addresses (ULA), the IPv6 equivalent of RFC1918
  auto bytes = addr.to_bytes();
  return (bytes[0] & 0xfe) == 0xfc;
}

static bool is_local_address(const boost::asio::ip::address& addr)
{
  if(addr.is_v4())
    return is_local_v4(addr.to_v4());
  if(addr.is_v6())
    return is_local_v6(addr.to_v6());
  return false;
}

static std::optional<resolved_url>
process_resolve_results(resolved_url ret, auto results)
{
  // 1. Give priority to localhost
  for(auto& result : results)
  {
    if(result.endpoint().address().is_loopback())
    {
      ossia::logger().info(
          "-> Resolved {} as a loopback ip", result.endpoint().address().to_string());
      ret.host = result.endpoint().address().to_string();
      return ret;
    }
  }

  // 2. Give priority to local addresses (IPv4 private + IPv6 ULA)
  for(auto& result : results)
  {
    const auto& addr = result.endpoint().address();
    if(is_local_address(addr))
    {
      ret.host = addr.to_string();
      ossia::logger().info("-> Resolved {} as a local network address", ret.host);
      return ret;
    }
  }

  // 3. Anything else — prefer IPv4, then accept IPv6
  for(auto& result : results)
  {
    if(const auto& addr = result.endpoint().address(); addr.is_v4())
    {
      ret.host = addr.to_string();
      ossia::logger().info("-> Resolved {} as an internet address", ret.host);
      return ret;
    }
  }
  for(auto& result : results)
  {
    const auto& addr = result.endpoint().address();
    ret.host = addr.to_string();
    ossia::logger().info("-> Resolved {} as an internet address (v6)", ret.host);
    return ret;
  }

  ossia::logger().info("-> Failed to resolve", ret.host);
  return std::nullopt;
}

splitted_host_and_port url_to_host_and_port(std::string_view url)
{
  splitted_host_and_port ret;

  ret.host = url;
  if(ret.host.starts_with("http://"))
    ret.host = ret.host.substr(7);
  else if(ret.host.starts_with("ws://"))
    ret.host = ret.host.substr(5);

  auto port_idx = ret.host.find_last_of(':');
  if(port_idx != std::string::npos)
  {
    ret.port = ret.host.substr(port_idx + 1);
    ret.host = ret.host.substr(0, port_idx);
  }
  else
  {
    ret.port = "80";
  }

  return ret;
}

template <typename Proto>
std::optional<resolved_url>
resolve_sync_v4(const std::string_view host, const std::string_view port)
{
  ossia::logger().info("Resolving {}@{}", host, port);
  if(host.empty() || port.empty())
    return std::nullopt;

  try
  {
    resolved_url ret;
    ret.port = port;

    // Maybe we were already fed an IP:
    {
      boost::system::error_code ec;
      auto addr = boost::asio::ip::make_address(host, ec);
      if(!ec)
      {
        ret.host = host;
        ret.protocol = addr.is_v6() ? Proto::v6().protocol() : Proto::v4().protocol();
        ret.family = addr.is_v6() ? Proto::v6().family() : Proto::v4().family();
        return ret;
      }
    }

    // Resolve hostname — try all address families
    ret.protocol = Proto::v4().protocol();
    ret.family = Proto::v4().family();

    boost::asio::io_context io_service;
    typename Proto::resolver resolver(io_service);
    auto results = resolver.resolve(
        host, port, boost::asio::ip::resolver_base::numeric_service);

    auto resolved = process_resolve_results(ret, results);
    if(resolved)
    {
      // Update protocol/family based on the resolved address
      boost::system::error_code ec;
      auto addr = boost::asio::ip::make_address(resolved->host, ec);
      if(!ec && addr.is_v6())
      {
        resolved->protocol = Proto::v6().protocol();
        resolved->family = Proto::v6().family();
      }
    }
    return resolved;
  }
  catch(const std::exception& e)
  {
    ossia::logger().error("Could not resolve host: {} => {}", host, e.what());
  }
  catch(...)
  {
    ossia::logger().error("Could not resolve host: {} => unknown error");
  }
  return std::nullopt;
}

template OSSIA_EXPORT std::optional<resolved_url> resolve_sync_v4<boost::asio::ip::tcp>(
    const std::string_view host, const std::string_view port);
template OSSIA_EXPORT std::optional<resolved_url> resolve_sync_v4<boost::asio::ip::udp>(
    const std::string_view host, const std::string_view port);
}
