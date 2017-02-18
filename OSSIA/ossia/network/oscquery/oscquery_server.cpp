#include "oscquery_server.hpp"
#include <ossia/network/generic/generic_address.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/generic/generic_node.hpp>

#include <ossia/network/osc/detail/osc.hpp>
#include <ossia/network/osc/detail/receiver.hpp>

#include <ossia/network/oscquery/detail/query_parser.hpp>
#include <ossia/network/oscquery/detail/json_writer.hpp>
#include <ossia/network/oscquery/detail/get_query_parser.hpp>
#include <ossia/network/oscquery/detail/json_query_parser.hpp>
namespace ossia
{
namespace oscquery
{

oscquery_server_protocol::oscquery_server_protocol(uint16_t osc_port, uint16_t ws_port):
  m_oscServer{std::make_unique<osc::receiver>(
                osc_port, [=](
                const oscpack::ReceivedMessage& m,
                const oscpack::IpEndpointName& ip) {
    this->on_OSCMessage(m, ip);
  })},
  m_oscPort{(uint16_t)m_oscServer->port()},
  m_wsPort{ws_port}
{
  m_websocketServer.set_open_handler([&] (connection_handler hdl) { on_connectionOpen(hdl); });
  m_websocketServer.set_close_handler([&] (connection_handler hdl) { on_connectionClosed(hdl); });
  m_websocketServer.set_message_handler([&] (connection_handler hdl, const std::string& str) {
    return on_WSrequest(hdl, str);
  });

  m_serverThread = std::thread{[&] { m_websocketServer.run(m_wsPort); }};
  m_oscServer->run();
}

oscquery_server_protocol::~oscquery_server_protocol()
{
  try { m_oscServer->stop(); } catch (...) { logger().error("Error when stopping osc server"); }
  try { m_websocketServer.stop(); } catch (...) { logger().error("Error when stopping WS server"); }
  if(m_serverThread.joinable())
    m_serverThread.join();
}

bool oscquery_server_protocol::pull(net::address_base&)
{
  //! The server cannot pull because it can have multiple clients.
  return false;
}

std::future<void> oscquery_server_protocol::pullAsync(net::address_base&)
{
  // Do nothing
  return {};
}

void oscquery_server_protocol::request(net::address_base&)
{
  // Do nothing
}

bool oscquery_server_protocol::push(const net::address_base& addr)
{
  if (addr.getAccessMode() == ossia::access_mode::GET)
    return false;

  auto val = net::filter_value(addr);
  if (val.valid())
  {
    //Push to all clients
    auto critical = net::get_critical(addr.getNode());
    if(!critical)
    {
      lock_t lock(m_clientsMutex);
      for(auto& client : m_clients)
      {
        client.sender->send(addr, val);
      }
    }
    else
    {
      lock_t lock(m_clientsMutex);
      for(auto& client : m_clients)
      {
        m_websocketServer.send_message(
              client.connection,
              json_writer::send_message(addr, val));
      }
    }

    return true;
  }
  return false;
}

bool oscquery_server_protocol::observe(net::address_base& address, bool enable)
{
  if (enable)
  {
    m_listening.insert(
          std::make_pair(net::osc_address_string(address), &address));
  }
  else
  {
    m_listening.erase(net::osc_address_string(address));
  }

  return true;
}

bool oscquery_server_protocol::observe_quietly(net::address_base& addr, bool b)
{
  return observe(addr, b);
}

bool oscquery_server_protocol::update(net::node_base&)
{
  return false;
}

void oscquery_server_protocol::setDevice(net::device_base& dev)
{
  // TODO disconnect in case there is an existing device
  m_device = &dev;

  // TODO renamed, etc
  dev.onNodeCreated.connect<oscquery_server_protocol, &oscquery_server_protocol::on_nodeCreated>(this);
  dev.onNodeRemoving.connect<oscquery_server_protocol, &oscquery_server_protocol::on_nodeRemoved>(this);
  dev.onAttributeModified.connect<oscquery_server_protocol, &oscquery_server_protocol::on_attributeChanged>(this);
}

oscquery_client*oscquery_server_protocol::findClient(
    const connection_handler& hdl)
{
  lock_t lock(m_clientsMutex);

  auto it = ossia::find(m_clients, hdl);
  if(it != m_clients.end())
    return &*it;
  return nullptr;
}

oscquery_client*oscquery_server_protocol::findBuildingClient(
    const connection_handler& hdl)
{
  lock_t lock(m_buildingClientsMutex);

  auto it = ossia::find(m_buildingClients, hdl);
  if(it != m_buildingClients.end())
    return &*it;
  return nullptr;
}

void oscquery_server_protocol::enableClient(
    const oscquery_server_protocol::connection_handler& hdl)
{
  lock_t l1(m_clientsMutex);
  lock_t l2(m_buildingClientsMutex);

  auto bld_it = ossia::find(m_buildingClients, hdl);
  assert(bld_it != m_buildingClients.end());

  m_clients.emplace_back(std::move(*bld_it));
  m_buildingClients.erase(bld_it);
}

using map_setter_fun = void(*)(const std::pair<const std::string, std::string>& str, ossia::net::address_data&);
template<typename Attr>
static auto make_setter_pair(Attr)
{
  return std::make_pair(detail::metadata<Attr>::key(),
                        [] (const std::pair<const std::string, std::string>& str,
                        ossia::net::address_data& addr) {
    query_parser::parse(Attr{}, str.second, addr);
  });
}

using query_reader_map_type = string_view_map<map_setter_fun>;

static auto& querySetterMap()
{
  static const query_reader_map_type map{
    [] {
      query_reader_map_type attr_impl;
      using namespace ossia::net;

      // Remaining metadata
      brigand::for_each< detail::all_attributes >([&] (auto attr) {
        using type = typename decltype(attr)::type;
        attr_impl.insert(make_setter_pair(type{}));
      });

      return attr_impl;
    }()};

  return map;
}

void oscquery_server_protocol::add_node(
    ossia::string_view parent_path,
    const string_map<std::string>& parameters)
{
  // /foo/bar/baz?ADD_NODE=tutu&VALUE="[hi]"
  net::address_data address;
  auto& map = querySetterMap();

  const auto& name_it = parameters.find(detail::add_node());
  if(name_it != parameters.end())
  {
    address.node_name = name_it.value();
  }

  for(const auto& e : parameters)
  {
    auto it = map.find(e.first);
    if(it != map.end())
    {
      it.value()(e, address);
    }
    else
    {
      // TODO add as an extended attribute.
    }
  }

  m_device->onAddNodeRequested(parent_path.to_string(), std::move(address));
}

void oscquery_server_protocol::remove_node(ossia::string_view path, const std::string& node)
{
  m_device->onRemoveNodeRequested(path.to_string(), node);
}

void oscquery_server_protocol::on_OSCMessage(
    const oscpack::ReceivedMessage& m, const oscpack::IpEndpointName& ip)
try {
  auto addr_txt = m.AddressPattern();
  auto addr = m_listening.find(addr_txt);
  if (addr && *addr)
  {
    net::update_value(**addr, m);
  }
  else
  {
    // We still want to save the value even if it is not listened to.
    if(auto n = net::find_node(m_device->getRootNode(), addr_txt))
    {
      if(auto base_addr = n->getAddress())
      {
        net::update_value_quiet(*base_addr, m);
      }
    }
  }

  if(mLogger.inbound_logger)
    mLogger.inbound_logger->info("In: {}", m);
} catch(const std::exception& e) {
  logger().error("oscquery_server_protocol::on_OSCMessage: {}", e.what());
} catch(...) {
  logger().error("oscquery_server_protocol::on_OSCMessage: error.");
}

void oscquery_server_protocol::on_connectionOpen(
    connection_handler hdl)
try {
  {
    auto con = m_websocketServer.impl().get_con_from_hdl(hdl);
    lock_t lock(m_buildingClientsMutex);
    m_buildingClients.emplace_back(hdl);
    m_buildingClients.back().client_ip = con->get_host();
  }
  // Send the client a message with the OSC port
  m_websocketServer.send_message(hdl, json_writer::device_info(m_oscPort));
} catch(const std::exception& e) {
  logger().error("oscquery_server_protocol::on_connectionOpen: {}", e.what());
} catch(...) {
  logger().error("oscquery_server_protocol::on_connectionOpen: error.");
}

void oscquery_server_protocol::on_connectionClosed(
    connection_handler hdl)
{
  lock_t lock(m_clientsMutex);
  auto it = ossia::find(m_clients, hdl);
  if(it != m_clients.end())
  {
    m_clients.erase(it);
  }
}

void oscquery_server_protocol::on_nodeCreated(
    const net::node_base& n)
try {
  const auto mess = json_writer::path_added(n);

  lock_t lock(m_clientsMutex);
  for(auto& client : m_clients)
  {
    m_websocketServer.send_message(
          client.connection,
          mess);
  }
} catch(const std::exception& e) {
  logger().error("oscquery_server_protocol::on_nodeCreated: {}", e.what());
} catch(...) {
  logger().error("oscquery_server_protocol::on_nodeCreated: error.");
}

void oscquery_server_protocol::on_nodeRemoved(
    const net::node_base& n)
try {
  const auto mess = json_writer::path_removed(net::osc_address_string(n));

  logger().info("on_nodeRemoved: {}", mess.GetString());
  lock_t lock(m_clientsMutex);
  for(auto& client : m_clients)
  {
    m_websocketServer.send_message(
          client.connection,
          mess);
  }
} catch(const std::exception& e) {
  logger().error("oscquery_server_protocol::on_nodeRemoved: {}", e.what());
} catch(...) {
  logger().error("oscquery_server_protocol::on_nodeRemoved: error.");
}

void oscquery_server_protocol::on_attributeChanged(
    const net::node_base& n,
    ossia::string_view attr)
try {
  const auto mess = json_writer::attributes_changed(n, attr);
  lock_t lock(m_clientsMutex);
  for(auto& client : m_clients)
  {
    m_websocketServer.send_message(
          client.connection,
          mess);
  }
} catch(const std::exception& e) {
  logger().error("oscquery_server_protocol::on_attributeChanged: {}", e.what());
} catch(...) {
  logger().error("oscquery_server_protocol::on_attributeChanged: error.");
}

rapidjson::StringBuffer oscquery_server_protocol::on_WSrequest(
    connection_handler hdl,
    const std::string& message)
{
  if(message.empty())
    return {};
  else if(message[0] == '/')
  {
    return query_parser::parse_http_request(
          message,
          get_query_answerer{}(*this, hdl));
  }
  else if(message[0] == '{')
  {
    rapidjson::Document doc;
    doc.Parse(message); // TODO ParseInsitu

    return json_query_answerer{}(*this, hdl, doc);
  }
  else
  {
    return {};
  }
  // Exceptions are catched in the caller.
}
}
}
