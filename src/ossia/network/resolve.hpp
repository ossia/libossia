#pragma once

#include <ossia/detail/logger.hpp>

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/basic_resolver.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace ossia
{
inline bool resolve_sync_v4(std::string& host, std::string_view port)
{
  try
  {
    boost::asio::io_context io_service;

    boost::asio::ip::tcp::resolver resolver(io_service);
    auto results = resolver.resolve(
        boost::asio::ip::tcp::v4(), host, port,
        boost::asio::ip::resolver_base::numeric_service);

    for(auto& result : results)
    {
      if(result.endpoint().address().is_loopback())
      {
        host = "127.0.0.1";
        return true;
      }
      else if(result.endpoint().address().is_v4())
      {
        host = result.endpoint().address().to_string();
      }
    }
    return true;
  }
  catch(const std::exception& e)
  {
    ossia::logger().error("Could not resolve host: {} => {}", host, e.what());
  }
  return false;
}
}
