// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "oscquery_server.hpp"
#include <ossia/network/common/node_visitor.hpp>
#include <ossia/network/generic/generic_parameter.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/generic/generic_node.hpp>

#include <ossia/network/osc/detail/osc.hpp>
#include <ossia/network/osc/detail/receiver.hpp>
#include <ossia/network/osc/detail/sender.hpp>
#include <ossia/network/common/network_logger.hpp>

#include <ossia/network/oscquery/detail/server.hpp>
#include <ossia/network/oscquery/detail/get_query_parser.hpp>
#include <ossia/network/oscquery/detail/json_query_parser.hpp>
#include <ossia/network/oscquery/detail/json_writer.hpp>
#include <ossia/network/oscquery/detail/query_parser.hpp>
#include <ossia/network/oscquery/detail/outbound_visitor.hpp>
#include <ossia/network/osc/detail/osc_receive.hpp>

#include <ossia/detail/mutex.hpp>
#include <ossia/detail/string_map.hpp>
namespace ossia
{
namespace oscquery
{
oscquery_server_protocol::oscquery_server_protocol(
    uint16_t osc_port, uint16_t ws_port)
    : m_oscServer{std::make_unique<osc::receiver>(
          osc_port,
          [=](const oscpack::ReceivedMessage& m,
              const oscpack::IpEndpointName& ip) {
            this->on_OSCMessage(m, ip);
          })}
    , m_websocketServer{std::make_unique<websocket_server>()}
    , m_oscPort{(uint16_t)m_oscServer->port()}
    , m_wsPort{ws_port}
{
  m_websocketServer->set_open_handler(
      [&](connection_handler hdl) { on_connectionOpen(hdl); });
  m_websocketServer->set_close_handler(
      [&](connection_handler hdl) { on_connectionClosed(hdl); });
  m_websocketServer->set_message_handler([&](
      const connection_handler& hdl, websocketpp::frame::opcode::value op, const std::string& str) {
    switch (op)
    {
      case websocketpp::frame::opcode::value::TEXT:
      {
        auto res = on_WSrequest(hdl, str);

        if (!res.data.empty()
            && res.type != server_reply::data_type::binary
            && m_logger.outbound_logger)
          m_logger.outbound_logger->info("OSCQuery WS Out: {}", res.data);

        return res;
      }
      case websocketpp::frame::opcode::value::BINARY:
      {
        return on_BinaryWSrequest(hdl, str);
      }
      default:
        return oscquery::server_reply{};
    }
  });

  m_websocketServer->listen(m_wsPort);
  m_serverThread = std::thread{[&] {
    try
    {
      m_websocketServer->run();
    }
    catch (const std::exception& e)
    {
      ossia::logger().error("Error in websocket processing: {}", e.what());
    }
    catch (...)
    {
      ossia::logger().error("Error in websocket processing");
    }
  }};
  m_oscServer->run();
}

oscquery_server_protocol::~oscquery_server_protocol()
{
  if(m_device)
  {
    auto& dev = *m_device;
    dev.on_node_created
        .disconnect<&oscquery_server_protocol::on_nodeCreated>(
            this);
    dev.on_node_removing
       .disconnect<&oscquery_server_protocol::on_nodeRemoved>(
            this);
    dev.on_attribute_modified
       .disconnect<&oscquery_server_protocol::on_attributeChanged>(
            this);
    dev.on_node_renamed
       .disconnect<&oscquery_server_protocol::on_nodeRenamed>(
           this);
  }
  stop();
}

bool oscquery_server_protocol::pull(net::parameter_base&)
{
  //! The server cannot pull because it can have multiple clients.
  return false;
}

std::future<void> oscquery_server_protocol::pull_async(net::parameter_base&)
{
  // Do nothing
  return {};
}

void oscquery_server_protocol::request(net::parameter_base&)
{
  // Do nothing
}

template<typename T>
bool oscquery_server_protocol::push_impl(const T& addr)
{
  auto val = net::filter_value(addr);
  if (val.valid())
  {
    // Push to all clients
    auto critical = addr.get_critical();
    if (!critical)
    {
      lock_t lock(m_clientsMutex);
      for (auto& client : m_clients)
      {
        if(client.sender)
        {
          ossia::oscquery::osc_writer::send_message(addr, val, m_logger, client.sender->socket());
        }
        else
        {
          m_websocketServer->send_binary_message(
                client.connection,
                osc_writer::send_message(addr, val, m_logger));
        }
      }
    }
    else
    {
      lock_t lock(m_clientsMutex);
      for (auto& client : m_clients)
      {
        m_websocketServer->send_binary_message(
            client.connection, osc_writer::send_message(addr, val, m_logger));
      }
    }

    return true;
  }
  return false;
}

bool oscquery_server_protocol::push(const net::parameter_base& addr)
{
  return push_impl(addr);
}

bool oscquery_server_protocol::push_raw(const net::full_parameter_data& addr)
{
  return push_impl(addr);
}

bool oscquery_server_protocol::push_bundle(const std::vector<const ossia::net::parameter_base*>& addresses)
{
  // TODO
  /*
  json_bundle_builder b;
  for(auto a : addresses)
  {
    const ossia::net::parameter_base& addr = *a;
    ossia::value val = net::filter_value(addr);
    if (val.valid())
    {
      b.add_message(addr, val);
    }
  }

  const auto str = b.finish();

  {
    lock_t lock(m_clientsMutex);
    for (auto& client : m_clients)
    {
      m_websocketServer->send_message(client.connection, str);
    }
  }
  */
  return true;
}

bool oscquery_server_protocol::push_raw_bundle(const std::vector<ossia::net::full_parameter_data>& addresses)
{
  // TODO
  /*
  json_bundle_builder b;
  for(const auto& addr : addresses)
  {
    ossia::value val = net::filter_value(addr);
    if (val.valid())
    {
      b.add_message(addr, val);
    }
  }

  const auto str = b.finish();

  {
    lock_t lock(m_clientsMutex);
    for (auto& client : m_clients)
    {
      m_websocketServer->send_message(client.connection, str);
    }
  }
  */
  return true;
}

bool oscquery_server_protocol::observe(net::parameter_base& address, bool enable)
{
  if (enable)
  {
    m_listening.insert(
        std::make_pair(address.get_node().osc_address(), &address));
  }
  else
  {
    m_listening.erase(address.get_node().osc_address());
  }

  return true;
}

bool oscquery_server_protocol::observe_quietly(net::parameter_base& addr, bool b)
{
  return observe(addr, b);
}

bool oscquery_server_protocol::update(net::node_base&)
{
  return false;
}

void oscquery_server_protocol::set_device(net::device_base& dev)
{
  if(m_device)
  {
    auto& old = *m_device;
    old.on_node_created
       .disconnect<&oscquery_server_protocol::on_nodeCreated>(
           this);
    old.on_node_removing
       .disconnect<&oscquery_server_protocol::on_nodeRemoved>(
           this);
    old.on_attribute_modified
       .disconnect<&oscquery_server_protocol::on_attributeChanged>(
           this);
    old.on_node_renamed
       .disconnect<&oscquery_server_protocol::on_nodeRenamed>(
           this);


    ossia::net::visit_parameters(
          old.get_root_node()
        , [&] (ossia::net::node_base& n, ossia::net::parameter_base& p) {
      if(p.callback_count() > 0)
        observe(p, false);
    });
  }
  m_device = &dev;

  // TODO renamed, etc
  dev.on_node_created
      .connect<&oscquery_server_protocol::on_nodeCreated>(
          this);
  dev.on_node_removing
      .connect<&oscquery_server_protocol::on_nodeRemoved>(
          this);
  dev.on_attribute_modified
      .connect<&oscquery_server_protocol::on_attributeChanged>(
          this);
  dev.on_node_renamed
      .connect<&oscquery_server_protocol::on_nodeRenamed>(
          this);

  update_zeroconf();

  ossia::net::visit_parameters(
        dev.get_root_node()
      , [&] (ossia::net::node_base& n, ossia::net::parameter_base& p) {
    if(p.callback_count() > 0)
      observe(p, true);
  });
}

void oscquery_server_protocol::stop()
{
    try
    {
        m_oscServer->stop();
    }
    catch (...)
    {
        logger().error("Error when stopping osc server");
    }

    try
    {
        // close client-connections before stopping
        auto it = m_clients.begin();
        while (it != m_clients.end())
        {
            auto con = m_websocketServer->impl().get_con_from_hdl(it->connection);
            con->close(websocketpp::close::status::going_away, "Server shutdown");
            it = m_clients.erase(it);
        }
    }
    catch (...)
    {
        logger().error("Error when freeing connections");
    }

    try
    {
        m_websocketServer->stop();
    }

    catch (...)
    {
        logger().error("Error when stopping WS server");
    }
    if (m_serverThread.joinable())
        m_serverThread.join();
}

oscquery_client*
oscquery_server_protocol::find_client(const connection_handler& hdl)
{
  lock_t lock(m_clientsMutex);

  auto it = ossia::find(m_clients, hdl);
  if (it != m_clients.end())
    return &*it;
  return nullptr;
}

using map_setter_fun = void (*)(
    const std::pair<const std::string, std::string>& str,
    ossia::net::parameter_data&);
template <typename Attr>
static auto make_setter_pair(Attr)
{
  return std::make_pair(
      detail::metadata<Attr>::key(),
      [](const std::pair<const std::string, std::string>& str,
         ossia::net::parameter_data& addr) {
        query_parser::parse(Attr{}, str.second, addr);
      });
}

using query_reader_map_type = string_view_map<map_setter_fun>;

static auto& querySetterMap()
{
  static const query_reader_map_type map{[] {
    query_reader_map_type attr_impl;
    using namespace ossia::net;

    // Remaining metadata
    ossia::for_each_tagged(detail::all_attributes{}, [&](auto attr) {
      using type = typename decltype(attr)::type;
      attr_impl.insert(make_setter_pair(type{}));
    });

    return attr_impl;
  }()};

  return map;
}

void oscquery_server_protocol::add_node(
    ossia::string_view parent_path, const string_map<std::string>& parameters)
{
  // /foo/bar/baz?ADD_NODE=tutu&VALUE="[hi]"
  net::parameter_data address;
  auto& map = querySetterMap();

  const auto& name_it = parameters.find(detail::add_node());
  if (name_it != parameters.end())
  {
    address.name = name_it.value();
  }

  for (const auto& e : parameters)
  {
    auto it = map.find(e.first);
    if (it != map.end())
    {
      it.value()(e, address);
    }
    else
    {
      // TODO add as an extended attribute.
    }
  }

  m_device->on_add_node_requested(
      std::string(parent_path), std::move(address));
}

void oscquery_server_protocol::remove_node(
    ossia::string_view path, const std::string& node)
{
  m_device->on_remove_node_requested(std::string(path), node);
}

void oscquery_server_protocol::on_OSCMessage(
    const oscpack::ReceivedMessage& m, oscpack::IpEndpointName ip) try
{
  ossia::net::handle_osc_message<true>(m, m_listening, *m_device, m_logger);

  if(m_echo)
  {
    for(auto& c : m_clients)
    {
      if(c.sender)
      {
        // TODO this is weird: udp does not really have a port...
        if(ip.port != c.remote_sender_port) // TODO check for ip too
        {
          c.sender->socket().Send(m.data(), m.size());
        }
      }
      else
      {
        m_websocketServer->send_binary_message(
              c.connection,
              std::string(m.data(), m.size()));
      }
    }
  }
}
catch (const std::exception& e)
{
  logger().error("oscquery_server_protocol::on_OSCMessage: {}", e.what());
}
catch (...)
{
  logger().error("oscquery_server_protocol::on_OSCMessage: error.");
}

void oscquery_server_protocol::on_connectionOpen(const connection_handler& hdl) try
{
  auto con = m_websocketServer->impl().get_con_from_hdl(hdl);

  asio::ip::tcp::socket& sock = con->get_raw_socket();
  auto ip = sock.remote_endpoint().address().to_string();
  if(ip.substr(0, 7) == "::ffff:")
      ip = ip.substr(7);

  {
    lock_t lock(m_clientsMutex);
    m_clients.emplace_back(hdl);
    m_clients.back().client_ip = std::move(ip);
  }

  onClientConnected(con->get_remote_endpoint());
}
catch (const std::exception& e)
{
  logger().error("oscquery_server_protocol::on_connectionOpen: {}", e.what());
}
catch (...)
{
  logger().error("oscquery_server_protocol::on_connectionOpen: error.");
}

void oscquery_server_protocol::on_connectionClosed(const connection_handler& hdl)
{
  lock_t lock(m_clientsMutex);
  auto it = ossia::find(m_clients, hdl);
  if (it != m_clients.end())
  {
    m_clients.erase(it);
  }

  auto con = m_websocketServer->impl().get_con_from_hdl(hdl);
  onClientDisconnected(con->get_remote_endpoint());
}

void oscquery_server_protocol::on_nodeCreated(const net::node_base& n) try
{
  const auto mess = json_writer::path_added(n);

  lock_t lock(m_clientsMutex);
  for (auto& client : m_clients)
  {
    m_websocketServer->send_message(client.connection, mess);
  }
}
catch (const std::exception& e)
{
  logger().error("oscquery_server_protocol::on_nodeCreated: {}", e.what());
}
catch (...)
{
  logger().error("oscquery_server_protocol::on_nodeCreated: error.");
}

void oscquery_server_protocol::on_nodeRemoved(const net::node_base& n) try
{
  const auto mess = json_writer::path_removed(n.osc_address());

  lock_t lock(m_clientsMutex);
  for (auto& client : m_clients)
  {
    m_websocketServer->send_message(client.connection, mess);
  }
}
catch (const std::exception& e)
{
  logger().error("oscquery_server_protocol::on_nodeRemoved: {}", e.what());
}
catch (...)
{
  logger().error("oscquery_server_protocol::on_nodeRemoved: error.");
}

void oscquery_server_protocol::on_attributeChanged(
    const net::node_base& n, ossia::string_view attr) try
{
  const auto mess = json_writer::attributes_changed(n, attr);
  lock_t lock(m_clientsMutex);
  for (auto& client : m_clients)
  {
    m_websocketServer->send_message(client.connection, mess);
  }
}
catch (const std::exception& e)
{
  logger().error(
      "oscquery_server_protocol::on_attributeChanged: {}", e.what());
}
catch (...)
{
  logger().error("oscquery_server_protocol::on_attributeChanged: error.");
}

void oscquery_server_protocol::on_nodeRenamed(
    const net::node_base& n, std::string oldname) try
{
  {
    // Local listening
    m_listening.rename(oldname, n.osc_address());
  }

  {
    // Remote listening
    lock_t lock(m_clientsMutex);
    for (auto& client : m_clients)
    {
      auto it = client.listening.find(oldname);
      if(it != client.listening.end())
      {
        auto v = it->second;
        client.listening.erase(it);
        client.listening.insert({n.osc_address(), v});
      }
    }
  }
  /* TODO
  const auto mess = json_writer::attributes_changed(n, attr);
  lock_t lock(m_clientsMutex);
  for (auto& client : m_clients)
  {
    m_websocketServer->send_message(client.connection, mess);
  }
  */
}
catch (const std::exception& e)
{
  logger().error(
      "oscquery_server_protocol::on_nodeRenamed: {}", e.what());
}
catch (...)
{
  logger().error("oscquery_server_protocol::on_nodeRenamed: error.");
}

void oscquery_server_protocol::update_zeroconf()
{
  try
  {
    m_zeroconfServerWS = net::make_zeroconf_server(
        get_device().get_name(), "_oscjson._tcp", "", m_wsPort, 0);
  }
  catch (const std::exception& e)
  {
    logger().error("oscquery_server_protocol::update_zeroconf: {}", e.what());
  }
  catch (...)
  {
    logger().error("oscquery_server_protocol::update_zeroconf: error.");
  }

  try
  {
    m_zeroconfServerOSC = net::make_zeroconf_server(
        get_device().get_name(), "_osc._udp", "", m_oscPort, 0);
  }
  catch (const std::exception& e)
  {
    logger().error("oscquery_server_protocol::update_zeroconf: {}", e.what());
  }
  catch (...)
  {
    logger().error("oscquery_server_protocol::update_zeroconf: error.");
  }
}

ossia::oscquery::server_reply oscquery_server_protocol::on_WSrequest(
    const connection_handler& hdl, const std::string& message)
{
  if (m_logger.inbound_logger)
    m_logger.inbound_logger->info("WS In: {}", message);

  if (message.empty())
  {
    return {};
  }
  else if (message[0] == '/')
  {
    return query_parser::parse_http_request(
        message, get_query_answerer{}(*this, hdl));
  }
  else
  {
    rapidjson::Document doc;
    doc.Parse(message); // TODO ParseInsitu

    if(!doc.HasParseError())
    {
      auto s = json_query_answerer{}(*this, hdl, doc);
      return std::string{};
    }
  }
  // Exceptions are catched in the caller.

  return {};
}

server_reply oscquery_server_protocol::on_BinaryWSrequest(
    const oscquery_server_protocol::connection_handler& hdl,
    const std::string& message)
{
  auto handler = [=](const oscpack::ReceivedMessage& m,
      const oscpack::IpEndpointName& ip) {
    this->on_OSCMessage(m, ip);
  };
  osc::listener<decltype(handler)> h{handler};
  auto clt = find_client(hdl);
  if(clt)
  {
    // TODO use proper ip / port
    h.ProcessPacket(message.data(), message.size(), {});
  }
  else
  {
    h.ProcessPacket(message.data(), message.size(), {});
  }

  return {};
}
}

oscquery_device::oscquery_device(uint16_t osc_port, uint16_t ws_port, std::string name)
  : device{std::make_unique<oscquery::oscquery_server_protocol>(osc_port, ws_port), name}
  , protocol{static_cast<oscquery::oscquery_server_protocol&>(device.get_protocol())}
{
}


oscquery_device::~oscquery_device()
{

}

}
