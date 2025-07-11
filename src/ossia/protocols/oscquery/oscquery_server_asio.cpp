// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "oscquery_server_asio.hpp"

#include <ossia/detail/algorithms.hpp>
#include <ossia/detail/mutex.hpp>
#include <ossia/detail/string_map.hpp>
#include <ossia/network/base/osc_address.hpp>
#include <ossia/network/common/network_logger.hpp>
#include <ossia/network/common/node_visitor.hpp>
#include <ossia/network/context.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/generic/generic_node.hpp>
#include <ossia/network/generic/generic_parameter.hpp>
#include <ossia/network/osc/detail/osc.hpp>
#include <ossia/network/osc/detail/osc_1_1_extended_policy.hpp>
#include <ossia/network/osc/detail/osc_packet_processor.hpp>
#include <ossia/network/osc/detail/osc_receive.hpp>
#include <ossia/network/osc/detail/osc_value_write_visitor.hpp>
#include <ossia/network/osc/detail/receiver.hpp>
#include <ossia/network/osc/detail/sender.hpp>
#include <ossia/network/oscquery/detail/get_query_parser.hpp>
#include <ossia/network/oscquery/detail/json_query_parser.hpp>
#include <ossia/network/oscquery/detail/json_writer.hpp>
#include <ossia/network/oscquery/detail/osc_writer.hpp>
#include <ossia/network/oscquery/detail/oscquery_protocol_common.hpp>
#include <ossia/network/oscquery/detail/outbound_visitor.hpp>
#include <ossia/network/oscquery/detail/query_parser.hpp>
#include <ossia/network/sockets/udp_socket.hpp>
#include <ossia/network/sockets/websocket_server.hpp>
#include <ossia/protocols/oscquery/oscquery_client_asio.hpp>
namespace ossia::oscquery_asio
{

struct oscquery_server_protocol::osc_receiver_impl : ossia::net::udp_receive_socket
{
  using udp_receive_socket::udp_receive_socket;
};

static uintptr_t
client_identifier(const clients& clts, const oscpack::IpEndpointName& ip)
{
  for(const auto& client_p : clts)
  {
    auto& c = *client_p;
    if(c.remote_sender_port == ip.port)
    {
      return (uintptr_t)c.connection.lock().get();
    }
  }
  return {};
}

static bool
is_same(const oscquery_client& clt, const ossia::net::message_origin_identifier& id)
{
  return (uintptr_t)clt.connection.lock().get() == id.identifier;
}

oscquery_server_protocol_base::oscquery_server_protocol_base(
    ossia::net::network_context_ptr ctx,
    const std::vector<ossia::net::osc_server_configuration>& conf, uint16_t ws_port,
    bool forceWS)
    : protocol_base{flags{SupportsMultiplex}}
    , m_context{std::move(ctx)}
    , m_websocketServer{std::make_unique<ossia::net::websocket_server>(m_context)}
    , m_oscConf{conf}
    , m_wsPort{ws_port}
    , m_forceWS{forceWS}
{
  m_clients.reserve(2);
  m_websocketServer->set_open_handler(
      [&](connection_handler hdl) { on_connectionOpen(hdl); });
  m_websocketServer->set_close_handler(
      [&](connection_handler hdl) { on_connectionClosed(hdl); });
  m_websocketServer->set_message_handler(
      [&](const connection_handler& hdl, websocketpp::frame::opcode::value op,
          const std::string& str) {
    switch(op)
    {
      case websocketpp::frame::opcode::value::TEXT: {
        auto res = on_text_ws_message(hdl, str);

        if(!res.data.empty() && res.type != ossia::net::server_reply::data_type::binary
           && m_logger.outbound_logger)
          m_logger.outbound_logger->info("OSCQuery WS Out: {}", res.data);

        return res;
      }
      case websocketpp::frame::opcode::value::BINARY: {
        return on_binary_ws_message(hdl, str);
      }
      default:
        return ossia::net::server_reply{};
    }
  });
}

oscquery_server_protocol_base::~oscquery_server_protocol_base()
{
  if(m_device)
  {
    auto& dev = *m_device;
    dev.on_node_created.disconnect<&oscquery_server_protocol_base::on_nodeCreated>(this);
    dev.on_node_removing.disconnect<&oscquery_server_protocol_base::on_nodeRemoved>(
        this);
    dev.on_parameter_created
        .disconnect<&oscquery_server_protocol_base::on_parameterChanged>(this);
    dev.on_parameter_removing
        .disconnect<&oscquery_server_protocol_base::on_parameterChanged>(this);
    dev.on_attribute_modified
        .disconnect<&oscquery_server_protocol_base::on_attributeChanged>(this);
    dev.on_node_renamed.disconnect<&oscquery_server_protocol_base::on_nodeRenamed>(this);
  }
  stop();
}

bool oscquery_server_protocol_base::pull(net::parameter_base&)
{
  //! The server cannot pull because it can have multiple clients.
  return false;
}

std::future<void> oscquery_server_protocol_base::pull_async(net::parameter_base&)
{
  // Do nothing
  return {};
}

void oscquery_server_protocol_base::request(net::parameter_base&)
{
  // Do nothing
}

struct ws_client_socket
{
  ossia::net::websocket_server& server;
  ossia::net::websocket_server::connection_handler& connection;

  void send_binary_message(std::string_view s)
  {
    server.send_binary_message(connection, s);
  }
};

bool oscquery_server_protocol_base::write_impl(std::string_view data, bool critical)
{
  // Push to all clients
  if(!critical)
  {
    lock_t lock(m_clientsMutex);
    for(auto& client_p : m_clients)
    {
      auto& client = *client_p;
      if(client.osc_socket)
      {
        client.osc_socket->write(data.data(), data.size());
      }
      else
      {
        m_websocketServer->send_binary_message(client.connection, data);
      }
    }
  }
  else
  {
    lock_t lock(m_clientsMutex);
    for(auto& client_p : m_clients)
    {
      auto& client = *client_p;
      m_websocketServer->send_binary_message(client.connection, data);
    }
  }

  return true;
}

template <typename T>
bool oscquery_server_protocol_base::push_impl(const T& addr, const ossia::value& v)
{
  using namespace ossia::net;
  using namespace ossia::oscquery;
  using proto = oscquery_protocol_common<osc_extended_policy>;

  // TODO if we have multiple clients, likely better to generate the message
  // once....
  auto val = bound_value(addr, v);
  if(val.valid())
  {
    // Push to all clients
    auto critical = m_forceWS || addr.get_critical();
    if(!critical)
    {
      lock_t lock(m_clientsMutex);
      for(auto& client_p : m_clients)
      {
        auto& client = *client_p;
        if(client.osc_socket)
        {
          if(m_logger.outbound_logger)
          {
            m_logger.outbound_logger->info(
                "Out: {} {}", ossia::net::osc_address(addr), val);
          }

          proto::osc_send_message(*this, *client.osc_socket, addr, val);
        }
        else
        {
          if(m_logger.outbound_logger)
          {
            m_logger.outbound_logger->info(
                "Out: {} {}", ossia::net::osc_address(addr), val);
          }

          proto::ws_send_binary_message(
              *this, ws_client_socket{*m_websocketServer, client.connection}, addr, val);
        }
      }
    }
    else
    {
      lock_t lock(m_clientsMutex);
      for(auto& client_p : m_clients)
      {
        auto& client = *client_p;
        if(m_logger.outbound_logger)
        {
          m_logger.outbound_logger->info(
              "Out: {} {}", ossia::net::osc_address(addr), val);
        }
        proto::ws_send_binary_message(
            *this, ws_client_socket{*m_websocketServer, client.connection}, addr, val);
      }
    }

    return true;
  }
  return false;
}

bool oscquery_server_protocol_base::push(
    const net::parameter_base& addr, const ossia::value& v)
{
  return push_impl(addr, v);
}

bool oscquery_server_protocol_base::push_raw(const net::full_parameter_data& addr)
{
  return push_impl(addr, addr.value());
}

bool oscquery_server_protocol_base::push_bundle(
    const std::vector<const ossia::net::parameter_base*>& addresses)
{
  using namespace ossia::net;
  using OscVersion = osc_extended_policy;

  if(auto bundle = ossia::net::make_bundle(
         ossia::net::bundle_client_policy<OscVersion>{}, addresses))
  {
    return write_impl(
        std::string_view{bundle->data.data(), bundle->data.size()}, bundle->critical);
  }
  return false;
}

bool oscquery_server_protocol_base::push_raw_bundle(
    const std::vector<ossia::net::full_parameter_data>& addresses)
{
  using namespace ossia::net;
  using OscVersion = osc_extended_policy;

  if(auto bundle = ossia::net::make_bundle(
         ossia::net::bundle_client_policy<OscVersion>{}, addresses))
  {
    return write_impl(
        std::string_view{bundle->data.data(), bundle->data.size()}, bundle->critical);
  }
  return false;
}

bool oscquery_server_protocol_base::echo_incoming_message(
    const net::message_origin_identifier& id, const net::parameter_base& addr,
    const ossia::value& val)
{
  using namespace ossia::net;

  using send_visitor = osc_value_send_visitor<
      net::parameter_base, osc_extended_policy, socket_writer<udp_send_socket>>;

  bool not_this_protocol = &id.protocol != this;
  // we know that the value is valid
  // Push to all clients except ours
  auto critical = addr.get_critical();
  if(!critical)
  {
    lock_t lock(m_clientsMutex);
    // No need to echo if we just have one client, the most common case
    for(auto& client_p : m_clients)
    {
      auto& client = *client_p;
      if(not_this_protocol || !is_same(client, id))
      {
        if(client.osc_socket)
        {
          if(m_logger.outbound_logger)
          {
            m_logger.outbound_logger->info(
                "Out: {} {}", addr.get_node().osc_address(), val);
          }
          send_visitor vis{addr, ossia::net::osc_address(addr), {*client.osc_socket}};
          val.apply(vis);
        }
        else
        {
          if(m_logger.outbound_logger)
          {
            m_logger.outbound_logger->info(
                "Out: {} {}", addr.get_node().osc_address(), val);
          }
          m_websocketServer->send_binary_message(
              client.connection, ossia::oscquery::osc_writer::to_message(addr, val));
        }
      }
    }
  }
  else
  {
    lock_t lock(m_clientsMutex);

    for(auto& client_p : m_clients)
    {
      auto& client = *client_p;
      if(not_this_protocol || !is_same(client, id))
      {

        if(m_logger.outbound_logger)
        {
          m_logger.outbound_logger->info(
              "Out: {} {}", addr.get_node().osc_address(), val);
        }

        m_websocketServer->send_binary_message(
            client.connection, ossia::oscquery::osc_writer::to_message(addr, val));
      }
    }
  }

  return true;
}

bool oscquery_server_protocol_base::observe(net::parameter_base& address, bool enable)
{
  if(enable)
  {
    m_listening.insert(std::make_pair(address.get_node().osc_address(), &address));
  }
  else
  {
    m_listening.erase(address.get_node().osc_address());
  }

  return true;
}

bool oscquery_server_protocol_base::observe_quietly(net::parameter_base& addr, bool b)
{
  return observe(addr, b);
}

bool oscquery_server_protocol_base::update(net::node_base&)
{
  return false;
}

void oscquery_server_protocol_base::set_device(net::device_base& dev)
{
  if(m_device)
  {
    auto& old = *m_device;
    old.on_node_created.disconnect<&oscquery_server_protocol_base::on_nodeCreated>(this);
    old.on_node_removing.disconnect<&oscquery_server_protocol_base::on_nodeRemoved>(
        this);
    dev.on_parameter_created
        .disconnect<&oscquery_server_protocol_base::on_parameterChanged>(this);
    dev.on_parameter_removing
        .disconnect<&oscquery_server_protocol_base::on_parameterChanged>(this);
    old.on_attribute_modified
        .disconnect<&oscquery_server_protocol_base::on_attributeChanged>(this);
    old.on_node_renamed.disconnect<&oscquery_server_protocol_base::on_nodeRenamed>(this);

    ossia::net::visit_parameters(
        old.get_root_node(),
        [&](ossia::net::node_base& n, ossia::net::parameter_base& p) {
      if(p.callback_count() > 0)
        observe(p, false);
        });
  }
  m_device = &dev;

  dev.on_node_created.connect<&oscquery_server_protocol_base::on_nodeCreated>(this);
  dev.on_node_removing.connect<&oscquery_server_protocol_base::on_nodeRemoved>(this);
  dev.on_parameter_created.connect<&oscquery_server_protocol_base::on_parameterChanged>(
      this);
  dev.on_parameter_removing.connect<&oscquery_server_protocol_base::on_parameterChanged>(
      this);
  dev.on_attribute_modified.connect<&oscquery_server_protocol_base::on_attributeChanged>(
      this);
  dev.on_node_renamed.connect<&oscquery_server_protocol_base::on_nodeRenamed>(this);

  update_zeroconf();

  ossia::net::visit_parameters(
      dev.get_root_node(), [&](ossia::net::node_base& n, ossia::net::parameter_base& p) {
        if(p.callback_count() > 0)
          observe(p, true);
      });

  m_websocketServer->listen(m_wsPort);
}

void oscquery_server_protocol_base::stop()
{
  if(m_websocketServer)
  {
    try
    {
      lock_t lock(m_clientsMutex);
      // close client-connections before stopping
      auto it = m_clients.begin();
      while(it != m_clients.end())
      {
        auto con = m_websocketServer->impl().get_con_from_hdl((*it)->connection);
        con->close(websocketpp::close::status::going_away, "Server shutdown");
        it = m_clients.erase(it);
      }
      m_clientCount = 0;
    }
    catch(...)
    {
      logger().error("Error when freeing connections");
    }

    try
    {
      m_websocketServer->stop();
      m_websocketServer.reset();
    }

    catch(...)
    {
      logger().error("Error when stopping WS server");
    }
  }
}

void oscquery_server_protocol_base::set_force_ws(bool forceWS) noexcept
{
  m_forceWS.store(forceWS, std::memory_order_relaxed);
}

oscquery_client*
oscquery_server_protocol_base::find_client(const connection_handler& hdl)
{
  lock_t lock(m_clientsMutex);

  auto it = ossia::find_if(m_clients, [&](auto& clt) { return *clt == hdl; });
  if(it != m_clients.end())
    return it->get();
  return nullptr;
}

using map_setter_fun = void (*)(
    const std::pair<const std::string, std::string>& str, ossia::net::parameter_data&);
template <typename Attr>
static auto make_setter_pair(Attr)
{
  return std::make_pair(
      ossia::oscquery::detail::metadata<Attr>::key(),
      [](const std::pair<const std::string, std::string>& str,
         ossia::net::parameter_data& addr) {
    ossia::oscquery::query_parser::parse(Attr{}, str.second, addr);
      });
}

using query_reader_map_type = string_view_map<map_setter_fun>;

static auto& querySetterMap()
{
  static const query_reader_map_type map{[] {
    query_reader_map_type attr_impl;
    using namespace ossia::net;

    // Remaining metadata
    ossia::for_each_tagged(ossia::oscquery::detail::all_attributes{}, [&](auto attr) {
      using type = typename decltype(attr)::type;
      attr_impl.insert(make_setter_pair(type{}));
    });

    return attr_impl;
  }()};

  return map;
}

void oscquery_server_protocol_base::add_node(
    std::string_view parent_path, const string_map<std::string>& parameters)
{
  // /foo/bar/baz?ADD_NODE=tutu&VALUE="[hi]"
  net::parameter_data address;
  auto& map = querySetterMap();

  const auto& name_it = parameters.find(ossia::oscquery::detail::add_node());
  if(name_it != parameters.end())
  {
    address.name = name_it->second;
  }

  for(const auto& e : parameters)
  {
    auto it = map.find(e.first);
    if(it != map.end())
    {
      it->second(e, address);
    }
    else
    {
      // TODO add as an extended attribute.
    }
  }

  m_device->on_add_node_requested(std::string(parent_path), address);
}

void oscquery_server_protocol_base::remove_node(
    std::string_view path, const std::string& node)
{
  m_device->on_remove_node_requested(std::string(path), node);
}

void oscquery_server_protocol_base::rename_node(
    std::string_view path, const std::string& new_name)
{
  m_device->on_rename_node_requested(std::string(path), new_name);
}

void oscquery_server_protocol_base::process_raw_osc_data(
    const char* data, std::size_t sz)
{
  auto on_message = [this](auto&& msg) { this->on_osc_message(msg); };
  ossia::net::osc_packet_processor<decltype(on_message)>{on_message}(data, sz);
}

void oscquery_server_protocol_base::on_osc_message(const oscpack::ReceivedMessage& m)
try
{
  // TODO
  auto id
      = ossia::net::message_origin_identifier{*this, client_identifier(m_clients, {})};
  ossia::net::on_input_message<true>(
      m.AddressPattern(), ossia::net::osc_message_applier{id, m}, m_listening, *m_device,
      m_logger);
}
catch(const std::exception& e)
{
  logger().error("oscquery_server_protocol::on_OSCMessage: {}", e.what());
}
catch(...)
{
  logger().error("oscquery_server_protocol::on_OSCMessage: error.");
}

void oscquery_server_protocol_base::on_connectionOpen(const connection_handler& hdl)
try
{
  auto con = m_websocketServer->impl().get_con_from_hdl(hdl);

  boost::asio::ip::tcp::socket& sock = con->get_raw_socket();
  auto ip = sock.remote_endpoint().address().to_string();
  if(ip.substr(0, 7) == "::ffff:")
    ip = ip.substr(7);

  {
    lock_t lock(m_clientsMutex);

    m_clients.emplace_back(std::make_unique<oscquery_client>(hdl));
    m_clients.back()->client_ip = std::move(ip);
    m_clientCount++;
  }

  onClientConnected(con->get_remote_endpoint());
}
catch(const std::exception& e)
{
  logger().error("oscquery_server_protocol::on_connectionOpen: {}", e.what());
}
catch(...)
{
  logger().error("oscquery_server_protocol::on_connectionOpen: error.");
}

void oscquery_server_protocol_base::on_connectionClosed(const connection_handler& hdl)
{
  {
    lock_t lock(m_clientsMutex);
    auto it = ossia::find_if(m_clients, [&](auto& clt) { return *clt == hdl; });
    if(it != m_clients.end())
    {
      --m_clientCount;
      m_clients.erase(it);
    }
  }

  auto con = m_websocketServer->impl().get_con_from_hdl(hdl);
  onClientDisconnected(con->get_remote_endpoint());
}

void oscquery_server_protocol_base::on_nodeCreated(const net::node_base& n)
try
{
  const auto mess = ossia::oscquery::json_writer::path_added(n);

  lock_t lock(m_clientsMutex);
  for(auto& client : m_clients)
  {
    m_websocketServer->send_message(client->connection, mess);
  }
}
catch(const std::exception& e)
{
  logger().error("oscquery_server_protocol::on_nodeCreated: {}", e.what());
}
catch(...)
{
  logger().error("oscquery_server_protocol::on_nodeCreated: error.");
}

void oscquery_server_protocol_base::on_nodeRemoved(const net::node_base& n)
try
{
  const auto mess = ossia::oscquery::json_writer::path_removed(n.osc_address());

  lock_t lock(m_clientsMutex);
  for(auto& client : m_clients)
  {
    m_websocketServer->send_message(client->connection, mess);
  }
}
catch(const std::exception& e)
{
  logger().error("oscquery_server_protocol::on_nodeRemoved: {}", e.what());
}
catch(...)
{
  logger().error("oscquery_server_protocol::on_nodeRemoved: error.");
}

void oscquery_server_protocol_base::on_parameterChanged(
    const ossia::net::parameter_base& p)
{
  on_attributeChanged(p.get_node(), ossia::net::text_value_type());
}

void oscquery_server_protocol_base::on_attributeChanged(
    const net::node_base& n, std::string_view attr)
try
{
  const auto mess = ossia::oscquery::json_writer::attributes_changed(n, attr);
  lock_t lock(m_clientsMutex);
  for(auto& client : m_clients)
  {
    m_websocketServer->send_message(client->connection, mess);
  }
}
catch(const std::exception& e)
{
  logger().error("oscquery_server_protocol::on_attributeChanged: {}", e.what());
}
catch(...)
{
  logger().error("oscquery_server_protocol::on_attributeChanged: error.");
}

void oscquery_server_protocol_base::on_nodeRenamed(
    const net::node_base& n, std::string oldname)
try
{
  auto old_addr = n.osc_address();
  auto it = old_addr.find_last_of('/');
  old_addr.resize(it + 1);
  old_addr += oldname;
  {
    // Local listening
    m_listening.rename(old_addr, n.osc_address());
  }

  {
    // Remote listening
    lock_t lock(m_clientsMutex);
    for(auto& client_p : m_clients)
    {
      auto& client = *client_p;
      std::lock_guard lck{client.listeningMutex};
      auto it = client.listening.find(old_addr);
      if(it != client.listening.end())
      {
        auto v = it->second;
        client.listening.erase(it);
        client.listening.insert({n.osc_address(), v});
      }
    }
  }
  const auto mess
      = ossia::oscquery::json_writer::path_renamed(old_addr, n.osc_address());
  lock_t lock(m_clientsMutex);
  for(auto& client : m_clients)
  {
    m_websocketServer->send_message(client->connection, mess);
  }
}
catch(const std::exception& e)
{
  logger().error("oscquery_server_protocol::on_nodeRenamed: {}", e.what());
}
catch(...)
{
  logger().error("oscquery_server_protocol::on_nodeRenamed: error.");
}

void oscquery_server_protocol_base::update_zeroconf()
{
  try
  {
    m_zeroconfServerWS = net::make_zeroconf_server(
        get_device().get_name(), "_oscjson._tcp", "", m_wsPort, 0);
  }
  catch(const std::exception& e)
  {
    logger().error("oscquery_server_protocol::update_zeroconf: {}", e.what());
  }
  catch(...)
  {
    logger().error("oscquery_server_protocol::update_zeroconf: error.");
  }
}

ossia::net::server_reply oscquery_server_protocol_base::on_text_ws_message(
    const connection_handler& hdl, const std::string& message)
{
  if(m_logger.inbound_logger)
    m_logger.inbound_logger->info("WS In: {}", message);

  if(message.empty())
  {
    return {};
  }
  else if(message[0] == '/')
  {
    return ossia::oscquery::query_parser::parse_http_request(
        message, ossia::oscquery::get_query_answerer{}(*this, hdl));
  }
  else
  {
    rapidjson::Document doc;
    doc.Parse(message); // TODO ParseInsitu

    if(!doc.HasParseError())
    {
      auto s = ossia::oscquery::json_query_answerer{}(*this, hdl, doc);
      return std::string{};
    }
  }
  // Exceptions are catched in the caller.

  return {};
}

ossia::net::server_reply oscquery_server_protocol_base::on_binary_ws_message(
    const oscquery_server_protocol_base::connection_handler& hdl,
    const std::string& message)
{
  // TODO here we know where the message comes from, can be used for id
  process_raw_osc_data(message.data(), message.size());
  return {};
}

oscquery_server_protocol::oscquery_server_protocol(
    ossia::net::network_context_ptr ctx, uint16_t osc_port, uint16_t ws_port,
    bool forceWS)
    : oscquery_server_protocol_base{ctx, {ossia::net::udp_server_configuration{{{"0.0.0.0"}, osc_port}}}, ws_port, forceWS}
    , m_oscPort{osc_port}
    , m_oscServer{std::make_unique<osc_receiver_impl>(
          ossia::net::inbound_socket_configuration{"0.0.0.0", osc_port},
          m_context->context)}

{
}

void oscquery_server_protocol::set_device(ossia::net::device_base& dev)
{
  oscquery_server_protocol_base::set_device(dev);

  m_oscServer->open();
  m_oscServer->receive(
      [this](const char* data, std::size_t sz) { process_raw_osc_data(data, sz); });
}

void oscquery_server_protocol::update_zeroconf()
{
  try
  {
    int port = ossia::get<ossia::net::udp_server_configuration>(m_oscConf[0]).port;
    m_zeroconfServerOSC
        = net::make_zeroconf_server(get_device().get_name(), "_osc._udp", "", port, 0);
  }
  catch(const std::exception& e)
  {
    logger().error("oscquery_server_protocol::update_zeroconf: {}", e.what());
  }
  catch(...)
  {
    logger().error("oscquery_server_protocol::update_zeroconf: error.");
  }
}

void oscquery_server_protocol::stop()
{
  try
  {
    m_oscServer->close();
  }
  catch(...)
  {
    logger().error("Error when stopping osc server");
  }
  oscquery_server_protocol_base::stop();
}

oscquery_server_protocol::~oscquery_server_protocol()
{
  if(m_device)
  {
    auto& dev = *m_device;
    dev.on_node_created.disconnect<&oscquery_server_protocol::on_nodeCreated>(this);
    dev.on_node_removing.disconnect<&oscquery_server_protocol::on_nodeRemoved>(this);
    dev.on_parameter_created.disconnect<&oscquery_server_protocol::on_parameterChanged>(
        this);
    dev.on_parameter_removing.disconnect<&oscquery_server_protocol::on_parameterChanged>(
        this);
    dev.on_attribute_modified.disconnect<&oscquery_server_protocol::on_attributeChanged>(
        this);
    dev.on_node_renamed.disconnect<&oscquery_server_protocol::on_nodeRenamed>(this);
  }
  stop();
}
}
