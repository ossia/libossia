#include "oscquery_mirror.hpp"

#include <ossia/network/osc/detail/osc.hpp>
#include <ossia/network/osc/detail/receiver.hpp>
#include <ossia/network/osc/detail/sender.hpp>
#include <ossia/network/oscquery/detail/json_parser.hpp>
#include <ossia/network/oscquery/detail/json_writer.hpp>
#include <boost/algorithm/string/erase.hpp>
#include <ossia/network/exceptions.hpp>
#include <ossia/network/oscquery/detail/http_client.hpp>
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
                    std::string& msg) { this->on_WSMessage(hdl, msg); } }
, m_websocketHost{std::move(host)}
{
  auto port_idx = m_websocketHost.find_last_of(':');
  if(port_idx != std::string::npos)
  {
    m_websocketPort = m_websocketHost.substr(port_idx);
  }
  else
  {
    m_websocketPort = "80";
  }
  m_wsThread = std::thread([=]{
    try {
      m_websocketClient.connect(m_websocketHost);
    } catch(...) {
      // Websocket does not connect, so let's try http requests
      m_useHTTP = true;
    }
  });
  int n = 0;
  while(!query_connected())
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

  m_oscServer->run();
}

void oscquery_mirror_protocol::cleanup_connections()
{
  try { m_oscServer->stop(); } catch (...) { logger().error("Error when stopping osc server"); }
  if(query_connected())
    try { query_stop(); } catch (...) { logger().error("Error when stopping WS server"); }
  if(m_wsThread.joinable())
    try { m_wsThread.join(); }
  catch(std::exception& e) { logger().error("Error when stopping WS thread: {}", e.what()); }
  catch(...) { logger().error("Error when stopping WS thread"); }
}

void oscquery_mirror_protocol::query_send_message(const std::string& str)
{
  if(!m_useHTTP)
    m_websocketClient.send_message(str);

  new http_get_request([=] (auto req, const auto& str) { delete req; },
  m_httpContext, m_websocketHost, m_websocketPort, str);
}

void oscquery_mirror_protocol::query_send_message(const rapidjson::StringBuffer& str)
{
  if(!m_useHTTP)
    m_websocketClient.send_message(str);

  new http_get_request([=] (auto req, const auto& str) { delete req; },
  m_httpContext, m_websocketHost, m_websocketPort, str.GetString());
}

bool oscquery_mirror_protocol::query_connected()
{
  if(!m_useHTTP)
    return m_websocketClient.connected();
  return true;
}

void oscquery_mirror_protocol::query_stop()
{
  if(!m_useHTTP)
    m_websocketClient.stop();
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
  m_getWSPromises.enqueue(get_ws_promise{std::move(promise), text});
  /*
  m_getOSCPromises.insert(
        std::make_pair(
          ossia::net::osc_address_string(address),
          get_promise{std::move(promise), &address}));
  */
  query_send_message(text + detail::query_value().to_string());
  return fut;
}

void oscquery_mirror_protocol::request(net::address_base& address)
{
  query_send_message(net::osc_address_string(address) + detail::query_value().to_string());
}

bool oscquery_mirror_protocol::push(const net::address_base& addr)
{
  if (addr.getAccessMode() == ossia::access_mode::GET)
    return false;

  auto val = net::filter_value(addr);
  if (val.valid())
  {
    // Push to server
    auto critical = net::get_critical(addr.getNode());
    if(!critical)
    {
      m_oscSender->send(addr, val);
    }
    else
    {
      query_send_message(json_writer::send_message(addr, val));
    }
    return true;
  }
  return false;
}

bool oscquery_mirror_protocol::observe(net::address_base& address, bool enable)
{
  if (enable)
  {
    auto str = net::osc_address_string(address);
    query_send_message(str + detail::query_listen_true().to_string());
    m_listening.insert(
          std::make_pair(str, &address));
  }
  else
  {
    auto str = net::osc_address_string(address);
    // TODO for minuit when disconnecting, disable listening for everything.
    query_send_message(str + detail::query_listen_false().to_string());
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
  query_send_message(ossia::net::osc_address_string(b));

  auto status = fut.wait_for(std::chrono::seconds(3));
  return status == std::future_status::ready;
}

void oscquery_mirror_protocol::setDevice(net::device_base& dev)
{
  m_device = &dev;
}

void oscquery_mirror_protocol::runCommands()
{
  bool ok = false;
  std::function<void()> cmd;
  do {
    ok = m_functionQueue.try_dequeue(cmd);
    if(ok && cmd)
      cmd();
  } while(ok);
}

void oscquery_mirror_protocol::setCommandCallback(
    std::function<void()> cb)
{
  m_commandCallback = std::move(cb);
}

void oscquery_mirror_protocol::requestRemoveNode(
    net::node_base& self)
{
  if(auto parent = self.getParent())
  {
    std::string req; req.reserve(64);
    req += net::osc_address_string(*parent);
    req +='?';
    req += detail::remove_node().to_string();
    req += '=';
    req += self.getName();

    query_send_message(std::move(req));
  }
}

void oscquery_mirror_protocol::setDisconnectCallback(std::function<void ()> f)
{
  m_websocketClient.onClose = std::move(f);
}

void oscquery_mirror_protocol::setFailCallback(std::function<void ()> f)
{
  m_websocketClient.onFail = std::move(f);
}

void oscquery_mirror_protocol::requestAddNode(
    net::node_base& parent,
    net::address_data dat)
{
  std::string req; req.reserve(64);
  req += net::osc_address_string(parent);
  req +='?';
  req += detail::add_node().to_string();
  req += '=';
  req += dat.node_name;

  // TODO the other attributes
  query_send_message(std::move(req));
}

void oscquery_mirror_protocol::on_OSCMessage(
    const oscpack::ReceivedMessage& m,
    const oscpack::IpEndpointName& ip)
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
    std::string& message)
{
#if defined(OSSIA_BENCHMARK)
  auto t1 = std::chrono::high_resolution_clock::now();
#endif
  try
  {
    auto data = json_parser::parse(message);
    if(data->IsNull()) {
      if(mLogger.inbound_logger)
        mLogger.inbound_logger->warn("Invalid WS message received: {}", message);
      return;
    }
    if(data->IsArray())
    {
      // TODO
    }
    else
    {
      switch(json_parser::message_type(*data))
      {
        case message_type::Device:
        {
          // The ip of the OSC server on the server
          m_oscSender = std::make_unique<osc::sender>(
                mLogger,
                to_ip(m_websocketHost),
                json_parser::get_port(*data));

          // Send to the server the local receiver port
          query_send_message(fmt::format("/?{}={}", detail::set_port(), m_oscServer->port()));
          break;
        }
        case message_type::Namespace:
        {
          json_parser::parse_namespace(m_device->getRootNode(), *data);
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
                json_parser::parse_value(*addr, *data);
              }
            }
            p->promise.set_value();
            m_getWSPromises.pop();
          }

          break;
        }
        case message_type::PathAdded:
        {
          m_functionQueue.enqueue([this,doc=std::move(data)] {
            json_parser::parse_path_added(m_device->getRootNode(), *doc);
          });
          if(m_commandCallback) m_commandCallback();
          break;
        }
        case message_type::PathChanged:
        {
          m_functionQueue.enqueue([this,doc=std::move(data)] {
            json_parser::parse_path_changed(m_device->getRootNode(), *doc);
          });
          if(m_commandCallback) m_commandCallback();
          break;
        }
        case message_type::PathRemoved:
        {
          m_functionQueue.enqueue([this,doc=std::move(data)] {
            json_parser::parse_path_removed(m_device->getRootNode(), *doc);
          });
          if(m_commandCallback) m_commandCallback();
          break;
        }
        case message_type::AttributesChanged:
        {
          m_functionQueue.enqueue([this,doc=std::move(data)] {
            json_parser::parse_attributes_changed(m_device->getRootNode(), *doc);
          });
          if(m_commandCallback) m_commandCallback();
          break;
        }
        default:
          break;
      }
    }
  }
  catch(std::exception& e)
  {
    if(mLogger.inbound_logger)
      mLogger.inbound_logger->warn("Error while parsing: {} ==> {}", e.what(), message);
  }

  if(mLogger.inbound_logger)
    mLogger.inbound_logger->info("WS In: {}", message);
#if defined(OSSIA_BENCHMARK)
  auto t2 = std::chrono::high_resolution_clock::now();
  ossia::logger().info("on_WSMessage : Time taken: {}", std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count());
#endif
}

}
}
