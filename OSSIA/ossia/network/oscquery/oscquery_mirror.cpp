#include "oscquery_mirror.hpp"

#include <ossia/network/osc/detail/osc.hpp>
#include <ossia/network/osc/detail/receiver.hpp>
#include <ossia/network/osc/detail/sender.hpp>
#include <ossia/network/oscquery/detail/json_reader.hpp>
#include <boost/algorithm/string/erase.hpp>
#include <ossia/network/exceptions.hpp>
namespace ossia
{
namespace oscquery
{

/*
auto wait_for(std::future<void>& fut, std::chrono::milliseconds dur)
{
  using clk = std::chrono::high_resolution_clock;
  std::future_status status = std::future_status::deferred;
  while(dur.count() > 0 || !(status == std::future_status::ready))
  {
    auto start = clk::now();
    auto next = start;
    for(int n = 0; n < 1000; n++)
    {
      next = std::chrono::system_clock::now();
    }
    dur -= std::chrono::duration_cast<std::chrono::milliseconds>(next - start);
    status = fut.wait_for(std::chrono::milliseconds(dur / 8));
    start = std::chrono::system_clock::now();
    dur -= std::chrono::duration_cast<std::chrono::milliseconds>(start - next);
  }
  return status;
}
*/

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
  int n = 0;
  while(!m_websocketClient.connected())
  {
    n++;
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    if(n > 500)
    {
      cleanup_connections();
      throw ossia::connection_error{"oscquery_mirror_protocol::oscquery_mirror_protocol: "
                                    "Could not connect to " + m_websocketHost};
    }
  }
}

void oscquery_mirror_protocol::cleanup_connections()
{
  try { m_oscServer->stop(); } catch (...) { logger().error("Error when stopping osc server"); }
  if(m_websocketClient.connected())
    try { m_websocketClient.stop(); } catch (...) { logger().error("Error when stopping WS server"); }
  if(m_wsThread.joinable())
    m_wsThread.join();
}

oscquery_mirror_protocol::~oscquery_mirror_protocol()
{
  cleanup_connections();
}

bool oscquery_mirror_protocol::pull(net::address_base& address)
{
#if defined(OSSIA_BENCHMARK)
  auto t1 = std::chrono::high_resolution_clock::now();
#endif
  auto fut = pullAsync(address);
  auto status = fut.wait_for(std::chrono::milliseconds(3000));

#if defined(OSSIA_BENCHMARK)
  auto t2 = std::chrono::high_resolution_clock::now();
  ossia::logger().info("Time taken: {}", std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count());
#endif
  return status == std::future_status::ready;
}

std::future<void> oscquery_mirror_protocol::pullAsync(net::address_base& address)
{
  std::promise<void> promise;
  auto fut = promise.get_future();
  auto text = net::osc_address_string(address);
  m_getWSPromises.enqueue({std::move(promise), text});
  /*
  m_getOSCPromises.insert(
        std::make_pair(
          ossia::net::osc_address_string(address),
          get_promise{std::move(promise), &address}));
  */
  m_websocketClient.send_message(text + "?VALUE");
  return fut;
}

void oscquery_mirror_protocol::request(net::address_base& address)
{
  m_websocketClient.send_message(net::osc_address_string(address) + "?VALUE");
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
#if defined(OSSIA_BENCHMARK)
  auto t1 = std::chrono::high_resolution_clock::now();
#endif
  auto addr_txt = m.AddressPattern();
  auto addr = m_listening.find(addr_txt);
  if (addr && *addr)
  {
    net::update_value(**addr, m);
  }
  else
  {
    // Maybe waiting for a get()
    auto get_opt = m_getOSCPromises.find_and_take(addr_txt);
    if(get_opt)
    {
      net::update_value(*get_opt->address, m);
      get_opt->promise.set_value();
    }
    else
    {
      // We still want to save the value even if it is not listened to.
      auto node = find_node(m_device->getRootNode(), addr_txt);
      if(node)
      {
        auto base_addr = node->getAddress();
        if(base_addr)
        {
          net::update_value_quiet(*base_addr, m);
        }
      }
    }
  }

  if(mLogger.inbound_logger)
    mLogger.inbound_logger->info("In: {0}", m);

#if defined(OSSIA_BENCHMARK)
  auto t2 = std::chrono::high_resolution_clock::now();
  ossia::logger().info("on_OSCMessage : Time taken: {}", std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count());
#endif
}

std::string to_ip(std::string uri)
{
  uri = boost::algorithm::ierase_first_copy(uri, "http://");
  uri = boost::algorithm::ierase_first_copy(uri, "https://");
  uri = boost::algorithm::ierase_first_copy(uri, "ws://");
  uri = boost::algorithm::ierase_first_copy(uri, "wss://");

  auto pos = uri.find_last_of(':');
  if(pos != std::string::npos)
    uri.erase(pos, uri.size());

  return uri;
}

void oscquery_mirror_protocol::on_WSMessage(
    oscquery_mirror_protocol::connection_handler hdl,
    const std::string& message)
{
#if defined(OSSIA_BENCHMARK)
  auto t1 = std::chrono::high_resolution_clock::now();
#endif
  try
  {
    auto data = json_parser::parse(message);
    if(data.IsNull()) {
      if(mLogger.inbound_logger)
        mLogger.inbound_logger->warn("Invalid WS message received: {}", message);
      return;
    }

    auto mt = json_parser::messageType(data);
    if(mt == message_type::Device)
    {
      // The ip of the OSC server on the server
      m_oscSender = std::make_unique<osc::sender>(mLogger, to_ip(m_websocketHost), json_parser::getPort(data));

      // Send to the server the local receiver port
      m_websocketClient.send_message("?set_port=" + std::to_string(m_oscServer->port()));
    }
    else
    {
      switch(mt)
      {
        case message_type::Namespace:
        {
          json_parser::parse_namespace(m_device->getRootNode(), data);
          m_namespacePromise.set_value();
          break;
        }
        case message_type::Value:
        {
          // TODO instead just put full path in reply ?
          auto p = m_getWSPromises.peek();
          if(p)
          {
            auto node = ossia::net::find_node(m_device->getRootNode(), p->address);
            if(node)
            {
              auto addr = node->getAddress();
              if(addr)
              {
                json_parser::parse_value(*addr, data);
              }
              break;
            }

            p->promise.set_value();
            m_getWSPromises.pop();
          }

          break;
        }
        case message_type::PathAdded:
        {
          json_parser::parse_path_added(m_device->getRootNode(), data);
          break;
        }
        case message_type::PathRemoved:
        {
          json_parser::parse_path_removed(m_device->getRootNode(), data);
          break;

        }
        case message_type::AttributesChanged:
        {
          json_parser::parse_attributes_changed(m_device->getRootNode(), data);
          break;

        }

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

#if defined(OSSIA_BENCHMARK)
  auto t2 = std::chrono::high_resolution_clock::now();
  ossia::logger().info("on_WSMessage : Time taken: {}", std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count());
#endif
}

}
}
