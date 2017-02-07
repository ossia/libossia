#include "oscquery.hpp"
#include <ossia/network/generic/generic_address.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/generic/generic_node.hpp>

#include <ossia/network/osc/detail/osc.hpp>
#include <ossia/network/osc/detail/sender.hpp>
#include <ossia/network/osc/detail/receiver.hpp>

#include <ossia/network/oscquery/detail/query_parser.hpp>
#include <ossia/network/oscquery/detail/json_writer.hpp>
namespace ossia
{
namespace oscquery
{

oscquery_server_protocol::oscquery_server_protocol(int16_t osc_port, int16_t ws_port):
  m_oscPort{osc_port},
  m_wsPort{ws_port},
  m_oscServer{std::make_unique<osc::receiver>(
                m_oscPort, [=](
                const oscpack::ReceivedMessage& m,
                const oscpack::IpEndpointName& ip) {
    this->on_OSCMessage(m, ip);
  })}
{
  m_websocketServer.set_open_handler([&] (connection_handler hdl) { on_connectionOpen(hdl); });
  m_websocketServer.set_close_handler([&] (connection_handler hdl) { on_connectionClosed(hdl); });
  m_websocketServer.set_message_handler([&] (connection_handler hdl, const std::string& str) {
    return on_WSrequest(hdl, str);
  });

  auto t = new std::thread{[&] { m_websocketServer.run(m_wsPort); }};
}

oscquery_server_protocol::~oscquery_server_protocol()
{

}

bool oscquery_server_protocol::pull(net::address_base&)
{
  return false;
}

std::future<void> oscquery_server_protocol::pullAsync(net::address_base&)
{
  return {};
}

void oscquery_server_protocol::request(net::address_base&)
{
}

bool oscquery_server_protocol::push(const net::address_base&)
{
  return false;
}

bool oscquery_server_protocol::observe(net::address_base&, bool)
{
  return false;
}

bool oscquery_server_protocol::observe_quietly(net::address_base&, bool)
{
  return false;
}

bool oscquery_server_protocol::update(net::node_base& b)
{
  return false;
}

void oscquery_server_protocol::setDevice(net::device_base& dev)
{
  m_device = &dev;
}

void oscquery_server_protocol::on_OSCMessage(
    const oscpack::ReceivedMessage& m, const oscpack::IpEndpointName& ip)
{
  auto addr_txt = m.AddressPattern();
  std::lock_guard<std::mutex> lock(m_listeningMutex);
  auto it = m_listening.find(addr_txt);
  if (it != m_listening.end())
  {
    ossia::net::address_base& addr = *it->second;
    update_value(addr, m);
  }
  else
  {
    // We still want to save the value even if it is not listened to.
    if(auto n = find_node(m_device->getRootNode(), addr_txt))
    {
      if(auto base_addr = n->getAddress())
      {
        update_value_quiet(*base_addr, m);
      }
    }
  }

  if(mLogger.inbound_logger)
    mLogger.inbound_logger->info("In: {0}", m);
}

void oscquery_server_protocol::on_connectionOpen(
    connection_handler hdl)
{
  /*
  // TODO This should be locked by inside? Use a thread-safe vector ?
  m_clients.emplace_back(hdl);

  // Send the client a message with the OSC port
  m_websocketServer.send_message(hdl, Serializer::device_info(m_data_server.port()));
  */
}

void oscquery_server_protocol::on_connectionClosed(
    connection_handler hdl)
{
  /*
  auto it = std::find(begin(m_clients), end(m_clients), hdl);
  if(it != end(m_clients))
  {
    m_clients.erase(it);
  }
  */
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
