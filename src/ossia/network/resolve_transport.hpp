#pragma once
#include <ossia/network/resolve.hpp>
#include <ossia/network/sockets/configuration.hpp>

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ip/udp.hpp>

namespace ossia
{

inline void resolve_host_in_transport(auto& conf)
{
  struct
  {
    using ret = void;
    ret operator()(ossia::net::udp_configuration& conf)
    {
      if(conf.remote)
      {
        auto& p = *conf.remote;
        if(auto res
           = resolve_sync_v4<boost::asio::ip::udp>(p.host, std::to_string(p.port)))
          conf.remote->host = res->host;
      }
    }
    void operator()(ossia::net::unix_dgram_configuration& conf) { }
    void operator()(ossia::net::unix_stream_configuration& conf) { }
    void operator()(ossia::net::serial_configuration& conf) { }
    void operator()(ossia::net::tcp_server_configuration& conf) { }
    void operator()(ossia::net::ws_server_configuration& conf) { }
    void operator()(ossia::net::tcp_client_configuration& conf)
    {
      auto& p = conf;
      if(auto res
         = resolve_sync_v4<boost::asio::ip::tcp>(p.host, std::to_string(p.port)))
        conf.host = res->host;
    }
    void operator()(ossia::net::ws_client_configuration& conf)
    {
      // not necessary as the ws library already does resolving
    }
  } vis;

  ossia::visit(vis, conf);
}
}
