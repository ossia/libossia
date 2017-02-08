#include "oscquery_mirror.hpp"

#include <ossia/network/osc/detail/osc.hpp>
#include <ossia/network/osc/detail/receiver.hpp>
#include <ossia/network/osc/detail/sender.hpp>
#include <ossia/network/oscquery/detail/json_reader_detail.hpp>
#include <boost/algorithm/string/erase.hpp>

namespace ossia
{
namespace oscquery
{

oscquery_mirror_protocol::oscquery_mirror_protocol(std::string host, uint16_t local_osc_port):
  m_oscServer{std::make_unique<osc::receiver>(
                local_osc_port, [=](
                const oscpack::ReceivedMessage& m,
                const oscpack::IpEndpointName& ip) { this->on_OSCMessage(m, ip); })},
  m_websocketClient{[=] (
                    connection_handler hdl,
                    const std::string& msg) { this->on_WSMessage(hdl, msg); } }
, m_websocketHost{std::move(host)}
{
  m_wsThread = std::thread([=]{ m_websocketClient.connect(m_websocketHost); });
  while(!m_websocketClient.connected())
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
}

oscquery_mirror_protocol::~oscquery_mirror_protocol()
{
  m_oscServer->stop();
  if(m_websocketClient.connected())
    m_websocketClient.stop();
  if(m_wsThread.joinable())
    m_wsThread.join();
}

bool oscquery_mirror_protocol::pull(net::address_base&)
{
  //! \todo
  return false;
}

std::future<void> oscquery_mirror_protocol::pullAsync(net::address_base&)
{
  //! \todo
  return {};
}

void oscquery_mirror_protocol::request(net::address_base&)
{
  //! \todo
}

bool oscquery_mirror_protocol::push(const net::address_base& addr)
{
  if (addr.getAccessMode() == ossia::access_mode::GET)
    return false;

  auto val = net::filter_value(addr);
  if (val.valid())
  {
    // Push to server
    if(!ossia::net::get_critical(addr.getNode()))
    {
      m_oscSender->send(addr, val);
    }
    else
    {
      // TODO send via WS
    }


    return true;
  }
  return false;
}

bool oscquery_mirror_protocol::observe(net::address_base& address, bool enable)
{
  //! \todo Send an OSCQuery Listen request
  if (enable)
  {
    auto str = net::osc_address_string(address);
    m_websocketClient.send_message(str + "?listen=true");
    m_listening.insert(
          std::make_pair(str, &address));
  }
  else
  {
    auto str = net::osc_address_string(address);
    // TODO for minuit when disconnecting, disable listening for everything.
    m_websocketClient.send_message(str + "?listen=false");
    m_listening.erase(str);
  }
  return true;
}

bool oscquery_mirror_protocol::observe_quietly(net::address_base& address, bool enable)
{
  if (enable)
    m_listening.insert(
          std::make_pair(net::osc_address_string(address), &address));
  else
    m_listening.erase(net::osc_address_string(address));

  return true;
}

bool oscquery_mirror_protocol::update(net::node_base& b)
{
  m_namespacePromise = std::promise<void>{};
  auto fut = m_namespacePromise.get_future();
  m_websocketClient.send_message(ossia::net::osc_address_string(b));

  auto status = fut.wait_for(std::chrono::seconds(3));
  return status == std::future_status::ready;
}

void oscquery_mirror_protocol::setDevice(net::device_base& dev)
{
  m_device = &dev;
}

void oscquery_mirror_protocol::on_OSCMessage(
    const oscpack::ReceivedMessage& m, const oscpack::IpEndpointName& ip)
{
  auto addr_txt = m.AddressPattern();
  auto addr = m_listening.find(addr_txt);
  if (addr)
  {
    net::update_value(*addr, m);
  }
  else
  {
    // We still want to save the value even if it is not listened to.
    if(auto n = find_node(m_device->getRootNode(), addr_txt))
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

std::string to_ip(std::string uri)
{
  boost::algorithm::erase_first_copy(uri, "http://");
  boost::algorithm::erase_first_copy(uri, "https://");
  boost::algorithm::erase_first_copy(uri, "ws://");
  boost::algorithm::erase_first_copy(uri, "wss://");

  auto pos = uri.find_last_of(':');
  if(pos != std::string::npos)
    uri.erase(pos, uri.size());

  return uri;
}

void oscquery_mirror_protocol::on_WSMessage(
    oscquery_mirror_protocol::connection_handler hdl,
    const std::string& message)
{
  try
  {
    auto data = json_parser::parse(message);
    if(data.IsNull()) {
      if(mLogger.inbound_logger)
        mLogger.inbound_logger->warn("Invalid WS message received: {}", message);
      return;
    }

    auto mt = json_parser::messageType(data);
    if(mt == MessageType::Device)
    {
      // The ip of the OSC server on the server
      m_oscSender = std::make_unique<osc::sender>(mLogger, to_ip(m_websocketHost), json_parser::getPort(data));
    }
    else
    {
      // TODO "Value" message
      if(mt == MessageType::Namespace)
      {
        json_parser::parseNamespace(m_device->getRootNode(), data);
        m_namespacePromise.set_value();
        return;
      }

      /*
      switch(mt)
      {
        case MessageType::PathAdded:
          Parser::template path_added<BaseMapType>(map().get_data_map(), data);
          break;

        case MessageType::PathChanged:
          Parser::path_changed(map().get_data_map(), data);
          break;

        case MessageType::PathRemoved:
          Parser::path_removed(map().get_data_map(), data);
          break;

        case MessageType::AttributesChanged:
          Parser::attributes_changed(map().get_data_map(), data);
          break;


        case MessageType::PathsAdded:
          Parser::template paths_added<BaseMapType>(map().get_data_map(), data);
          break;

        case MessageType::PathsChanged:
          Parser::paths_changed(map().get_data_map(), data);
          break;

        case MessageType::PathsRemoved:
          Parser::paths_removed(map().get_data_map(), data);
          break;

        case MessageType::AttributesChangedArray:
          Parser::attributes_changed_array(map().get_data_map(), data);
          break;

        case MessageType::Device:
        default:
          break;
      }

      if(onUpdate) onUpdate();

      */
    }
  }
  catch(std::exception& e)
  {
    if(mLogger.inbound_logger)
      mLogger.inbound_logger->warn("Error while parsing: {} ==> {}", e.what(), message);
  }
}

}
}
