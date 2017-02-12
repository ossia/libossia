#include "oscquery_server.hpp"
#include <ossia/network/generic/generic_address.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/generic/generic_node.hpp>

#include <ossia/network/osc/detail/osc.hpp>
#include <ossia/network/osc/detail/receiver.hpp>

#include <ossia/network/oscquery/detail/query_parser.hpp>
#include <ossia/network/oscquery/detail/json_writer.hpp>
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
  m_oscPort{osc_port},
  m_wsPort{ws_port}
{
  m_websocketServer.set_open_handler([&] (connection_handler hdl) { on_connectionOpen(hdl); });
  m_websocketServer.set_close_handler([&] (connection_handler hdl) { on_connectionClosed(hdl); });
  m_websocketServer.set_message_handler([&] (connection_handler hdl, const std::string& str) {
    return on_WSrequest(hdl, str);
  });

  m_serverThread = std::thread{[&] { m_websocketServer.run(m_wsPort); }};
}

oscquery_server_protocol::~oscquery_server_protocol()
{
  m_oscServer->stop();
  m_websocketServer.stop();
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
    std::lock_guard<std::mutex> lock(m_clientsMutex);
    for(auto& client : m_clients)
    {
      client.sender->send(addr, val);
    }
    return true;
  }
  return false;
}

bool oscquery_server_protocol::observe(net::address_base& address, bool enable)
{
  if (enable)
    m_listening.insert(
          std::make_pair(net::osc_address_string(address), &address));
  else
    m_listening.erase(net::osc_address_string(address));

  return true;
}

bool oscquery_server_protocol::observe_quietly(net::address_base& addr, bool b)
{
  return observe(addr, b);
}

bool oscquery_server_protocol::update(net::node_base& b)
{
  return false;
}

void oscquery_server_protocol::setDevice(net::device_base& dev)
{
  // TODO disconnect in case there is an existing device
  m_device = &dev;

  dev.onNodeCreated.connect<oscquery_server_protocol, &oscquery_server_protocol::on_nodeCreated>(this);
  dev.onNodeRemoving.connect<oscquery_server_protocol, &oscquery_server_protocol::on_nodeRemoved>(this);
  dev.onAttributeModified.connect<oscquery_server_protocol, &oscquery_server_protocol::on_attributeChanged>(this);
}

oscquery_client*oscquery_server_protocol::findClient(const connection_handler& hdl)
{
  std::lock_guard<std::mutex> lock(m_clientsMutex);

  auto it = ossia::find(m_clients, hdl);
  if(it != m_clients.end())
    return &*it;
  return nullptr;
}

void oscquery_server_protocol::on_OSCMessage(
    const oscpack::ReceivedMessage& m, const oscpack::IpEndpointName& ip)
{
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
    mLogger.inbound_logger->info("In: {0}", m);
}

void oscquery_server_protocol::on_connectionOpen(
    connection_handler hdl)
{
  {
    std::lock_guard<std::mutex> lock(m_clientsMutex);
    m_clients.emplace_back(hdl);
    auto con = m_websocketServer.impl().get_con_from_hdl(hdl);
    m_clients.back().client_ip = con->get_host();
  }
  // Send the client a message with the OSC port
  m_websocketServer.send_message(hdl, json_writer::device_info(m_oscPort));
}

void oscquery_server_protocol::on_connectionClosed(
    connection_handler hdl)
{
  std::lock_guard<std::mutex> lock(m_clientsMutex);
  auto it = ossia::find(m_clients, hdl);
  if(it != m_clients.end())
  {
    m_clients.erase(it);
  }
}

void oscquery_server_protocol::on_nodeCreated(const net::node_base& n)
{
  m_clientsMutex.lock();
  for(auto& client : m_clients)
  {
    m_websocketServer.send_message(
          client.connection,
          json_writer::path_added(n));
  }
}

void oscquery_server_protocol::on_nodeRemoved(const net::node_base& n)
{
  m_clientsMutex.lock();
  for(auto& client : m_clients)
  {
    m_websocketServer.send_message(
          client.connection,
          json_writer::path_removed(net::osc_address_string(n)));
  }

}

void oscquery_server_protocol::on_attributeChanged(const net::node_base& n, ossia::string_view attr)
{
  m_clientsMutex.lock();
  for(auto& client : m_clients)
  {
    m_websocketServer.send_message(
          client.connection,
          json_writer::attributes_changed(n, attr));
  }
}

rapidjson::StringBuffer oscquery_server_protocol::on_WSrequest(
    connection_handler hdl,
    const std::string& message)
{
  return query_parser::parse(
        message,
        query_answerer::answer(*this, hdl));
}

}
}
