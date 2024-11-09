#pragma once
#include <ossia/network/base/osc_address.hpp>
#include <ossia/network/base/parameter.hpp>
#include <ossia/network/base/parameter_data.hpp>
#include <ossia/network/osc/detail/bundle.hpp>
#include <ossia/network/osc/detail/osc.hpp>
#include <ossia/network/osc/detail/osc_1_1_extended_policy.hpp>
#include <ossia/network/osc/detail/osc_value_write_visitor.hpp>
#include <ossia/network/sockets/udp_socket.hpp>
#include <ossia/network/sockets/writers.hpp>

namespace ossia::oscquery
{
template <typename OscVersion>
struct oscquery_protocol_common
{
  template <typename Protocol, typename Socket, typename Addr>
  static void osc_send_message(
      Protocol& proto, Socket& socket, const Addr& addr, const ossia::value& val)
  {
    using namespace ossia::net;
    using send_visitor
        = osc_value_send_visitor<Addr, OscVersion, socket_writer<udp_send_socket>>;
    send_visitor vis{addr, ossia::net::osc_address(addr), {socket}};
    val.apply(vis);
  }
  template <typename Protocol, typename Addr>
  static void
  osc_send_message(Protocol& proto, const Addr& addr, const ossia::value& val)
  {
    osc_send_message(proto, proto.osc_sender(), addr, val);
  }

  template <typename Protocol, typename Socket, typename Addr>
  static void ws_send_binary_message(
      Protocol& proto, Socket&& socket, const Addr& addr, const ossia::value& val)
  {
    using namespace ossia::net;
    using write_visitor = osc_value_write_visitor<Addr, OscVersion>;

    auto& pool = buffer_pool::instance();
    auto buf = pool.acquire();

    write_visitor vis{addr, ossia::net::osc_address(addr), buf};
    val.apply(vis);

    socket.send_binary_message({buf.data(), buf.size()});

    pool.release(std::move(buf));
  }

  template <typename Protocol, typename Addr>
  static void
  ws_send_binary_message(Protocol& proto, const Addr& addr, const ossia::value& val)
  {
    using namespace ossia::net;
    using write_visitor = osc_value_write_visitor<Addr, OscVersion>;

    auto& pool = buffer_pool::instance();
    auto buf = pool.acquire();

    write_visitor vis{addr, ossia::net::osc_address(addr), buf};
    val.apply(vis);
    proto.ws_client().send_binary_message({buf.data(), buf.size()});

    pool.release(std::move(buf));
  }

  template <typename Protocol, typename Addr>
  static bool push(Protocol& proto, const Addr& addr, const ossia::value& v)
  {
    if(auto val = bound_value(addr, v); val.valid())
    {
      // Push to server
      const bool critical = addr.get_critical();
      const bool has_ws = proto.ws_connected();
      const bool has_osc = proto.osc_connected();
      if((!critical || !has_ws) && has_osc)
      { /*
         if (m_logger.outbound_logger)
         {
           m_logger.outbound_logger->info("Out: {} {}", addr.address, val);
         }*/
        osc_send_message(proto, addr, val);
        return true;
      }
      else if(has_ws)
      { /*
         if (m_logger.outbound_logger)
         {
           m_logger.outbound_logger->info("Out: {} {}", addr.address, val);
         }*/

        ws_send_binary_message(proto, addr, val);
        return true;
      }

      /*
      if (m_logger.outbound_listened_logger)
        m_logger.outbound_listened_logger->info("Out: {0}", addr, val);
        */
    }
    return false;
  }
};

template <typename OscVersion>
struct oscquery_protocol_client : oscquery_protocol_common<OscVersion>
{
  template <typename T, typename Addr_T, typename Val_T>
  static bool push(T& self, const Addr_T& addr, Val_T&& v)
  {
    if(addr.get_access() == ossia::access_mode::GET)
      return false;

    return oscquery_protocol_common<OscVersion>::push(
        self, addr, std::forward<Val_T>(v));
  }

  template <typename Protocol, typename Addresses>
    requires(!requires(Protocol p) { p.ws_connected(); })
  static bool push_bundle(Protocol& proto, const Addresses& addresses)
  {
    // FIXME most likely we shouldn't do anything here but maybe
    // the concept heuristic isn't the best
    return false;
  }

  template <typename Protocol, typename Addresses>
    requires requires(Protocol p) { p.ws_connected(); }
  static bool push_bundle(Protocol& proto, const Addresses& addresses)
  {
    if(auto bundle = ossia::net::make_bundle(
           ossia::net::bundle_client_policy<OscVersion>{}, addresses))
    {
      const bool critical = bundle->critical;
      const bool has_ws = proto.ws_connected();
      const bool has_osc = proto.osc_connected();
      if((!critical || !has_ws) && has_osc)
      {
        proto.osc_sender().write(bundle->data.data(), bundle->data.size());
        ossia::buffer_pool::instance().release(std::move(bundle->data));
      }
      else if(has_ws)
      {
        proto.ws_client().send_binary_message(
            {bundle->data.data(), bundle->data.size()});
      }
      return true;
    }
    return false;
  }
};

/*
template<typename OscVersion>
struct oscquery_protocol_server: oscquery_protocol_common<OscVersion>
{

  template<typename Protocol, typename Addresses>
  static bool push_bundle(Protocol& proto, const Addresses& addresses)
  {
    if(auto bundle =
ossia::net::make_bundle(ossia::net::bundle_client_policy<OscVersion>{},
addresses))
    {
      const bool critical = bundle->critical;
      const bool has_ws = proto.ws_connected();
      const bool has_osc = proto.osc_connected();
      if ((!critical || !has_ws) && has_osc)
      {
        proto.osc_sender().write(bundle->data.data(), bundle->data.size());
        ossia::buffer_pool::instance().release(std::move(bundle->data));
      }
      else if (has_ws)
      {
        proto.ws_client().send_binary_message({bundle->data.data(),
bundle->data.size()});
      }
      return true;
    }
    return false;
  }
};
*/
}
