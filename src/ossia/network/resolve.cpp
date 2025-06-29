#include "resolve.hpp"

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/basic_resolver.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ip/udp.hpp>

namespace ossia
{

static std::optional<resolved_url>
process_resolve_results(resolved_url ret, auto results)
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

  // 1. Give priority to localhost
  for(auto& result : results)
  {
    if(result.endpoint().address().is_loopback())
    {
      ossia::logger().info(
          "-> Resolved {} as a loopback ip", result.endpoint().address().to_string());
      // ret.host = "127.0.0.1";
      // Some software using shitty Java network APIs actually hardcode the
      // IP they want to be talked to so we just use the found ip then, and hope for the best:
      ret.host = result.endpoint().address().to_string();
      return ret;
    }
  }

  // 2. Give priority to local addresses
  for(auto& result : results)
  {
    if(const auto& addr = result.endpoint().address(); addr.is_v4())
    {
      auto ip = addr.to_v4().to_uint();
      if((ip >= local_range[0].min && ip <= local_range[0].max)
         || (ip >= local_range[1].min && ip <= local_range[1].max)
         || (ip >= local_range[2].min && ip <= local_range[2].max))
      {
        ret.host = addr.to_string();
        ossia::logger().info("-> Resolved {} as a local network address", ret.host);
        return ret;
      }
    }
  }

  // 3. Anything else
  for(auto& result : results)
  {
    if(const auto& addr = result.endpoint().address(); addr.is_v4())
    {
      ret.host = addr.to_string();
      ossia::logger().info("-> Resolved {} as an internet address", ret.host);
      return ret;
    }
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
    ret.protocol = Proto::v4().protocol();
    ret.family = Proto::v4().family();
    ret.port = port;

    // Maybe we were already fed an IP:
    {
      boost::system::error_code ec;
      // this calls inet_pton which is the os-level ip validation
      boost::asio::ip::make_address(host, ec);
      if(!ec)
      {
        ret.host = host;
        return ret;
      }
    }

    // Fallback to proper resolving.
    boost::asio::io_context io_service;
    typename Proto::resolver resolver(io_service);
    auto results = resolver.resolve(
        Proto::v4(), host, port, boost::asio::ip::resolver_base::numeric_service);

    return process_resolve_results(ret, results);
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
