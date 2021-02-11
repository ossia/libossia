// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "oscquery_mirror_asio.hpp"

#include <ossia/network/base/device.hpp>
#include <ossia/network/common/node_visitor.hpp>
#include <ossia/network/exceptions.hpp>
#include <ossia/network/osc/detail/osc.hpp>
#include <ossia/network/osc/detail/osc_receive.hpp>
#include <ossia/network/osc/detail/receiver.hpp>
#include <ossia/network/osc/detail/sender.hpp>
#include <ossia/network/oscquery/detail/client.hpp>
#include <ossia/network/oscquery/detail/http_client.hpp>
#include <ossia/network/oscquery/detail/json_parser.hpp>
#include <ossia/network/oscquery/detail/json_writer.hpp>
#include <ossia/network/oscquery/detail/osc_writer.hpp>
#include <ossia/network/oscquery/detail/outbound_visitor.hpp>
#include <ossia/network/oscquery/detail/value_to_json.hpp>
#include <ossia/network/osc/detail/bidir.hpp>
#include <ossia/network/osc/detail/osc_1_1_extended_policy.hpp>
#include <ossia/network/osc/detail/osc_protocol_common.hpp>
#include <ossia/network/context.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/erase.hpp>
namespace ossia
{
namespace oscquery
{
struct oscquery_mirror_asio_protocol::osc_sender_impl
{
  ossia::net::udp_socket socket;
};
struct oscquery_mirror_asio_protocol::osc_receiver_impl
{
  ossia::net::udp_socket socket;
};

struct http_async_answer
{
  oscquery_mirror_asio_protocol& self;

  template <typename T, typename S>
  void operator()(T& req, const S& str)
  {
    if (self.on_text_ws_message({}, str))
    {
      req.close();
    }
  }
};

struct http_async_error
{
  template <typename T>
  void operator()(T& req)
  {
    req.close();
  }
};

using http_async_request = http_get_request<http_async_answer, http_async_error>;

struct http_async_client_context
{
  std::shared_ptr<asio::io_service::work> worker;
};
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

oscquery_mirror_asio_protocol::oscquery_mirror_asio_protocol(
    ossia::net::network_context_ptr ctx,
    std::string host,
    uint16_t local_osc_port)
    : protocol_base{flags{SupportsMultiplex}}
    , m_ctx{ctx}
    , m_queryHost{std::move(host)}
    , m_httpHost{m_queryHost}
    , m_osc_port{local_osc_port}
    , m_http{std::make_unique<http_async_client_context>()}
    , m_id{*this, {}}
{
  auto port_idx = m_queryHost.find_last_of(':');
  if (port_idx != std::string::npos)
    m_queryPort = m_queryHost.substr(port_idx + 1);
  else
    m_queryPort = "80";

  // for http, host should be only the name, e.g. example.com instead of
  // http://example.com:1234
  if (port_idx != std::string::npos)
    m_httpHost.erase(m_httpHost.begin() + port_idx, m_httpHost.end());
  if (boost::starts_with(m_httpHost, "http://"))
    m_httpHost.erase(m_httpHost.begin(), m_httpHost.begin() + 7);
  else if (boost::starts_with(m_httpHost, "ws://"))
    m_httpHost.erase(m_httpHost.begin(), m_httpHost.begin() + 5);
}


void oscquery_mirror_asio_protocol::stop()
{
  cleanup_connections();
}

void oscquery_mirror_asio_protocol::cleanup_connections()
{
  try
  {
    m_oscServer->socket.close([]{});
  }
  catch (...)
  {
    logger().error("Error when stopping osc server");
  }

  if (query_connected())
  {
    try
    {
      query_stop();
    }
    catch (...)
    {
      logger().error("Error when stopping WS server");
    }
  }

  try
  {
    m_http->worker.reset();
  }
  catch (...)
  {
    logger().error("Error when stopping HTTP thread");
  }
}

void oscquery_mirror_asio_protocol::http_send_message(const std::string& str)
{
  auto hrq = std::make_shared<http_async_request>(
      http_async_answer{*this}, http_async_error{}, m_ctx->context, m_httpHost, str);
  hrq->resolve(m_httpHost, m_queryPort);
}

void oscquery_mirror_asio_protocol::http_send_message(
    const rapidjson::StringBuffer& str)
{
  auto hrq = std::make_shared<http_async_request>(
      http_async_answer{*this}, http_async_error{}, m_ctx->context, m_httpHost,
      str.GetString());
  hrq->resolve(m_httpHost, m_queryPort);
}

void oscquery_mirror_asio_protocol::ws_send_message(const std::string& str)
{
  if (m_hasWS)
    m_websocketClient->send_message(str);
}

void oscquery_mirror_asio_protocol::ws_send_message(
    const rapidjson::StringBuffer& str)
{
  if (m_hasWS)
    m_websocketClient->send_message(str);
}

void oscquery_mirror_asio_protocol::ws_send_binary_message(const std::string& str)
{
  if (m_hasWS)
    m_websocketClient->send_binary_message(str);
}

bool oscquery_mirror_asio_protocol::query_connected()
{
  if (m_hasWS)
    return m_websocketClient->connected();
  return true;
}

void oscquery_mirror_asio_protocol::query_stop()
{
  if (m_hasWS)
  {
    m_hasWS = false;
    m_websocketClient->stop();
  }
}

oscquery_mirror_asio_protocol::~oscquery_mirror_asio_protocol()
{
  stop();
}

bool oscquery_mirror_asio_protocol::pull(net::parameter_base& address)
{
#if defined(OSSIA_BENCHMARK)
  auto t1 = std::chrono::high_resolution_clock::now();
#endif
  auto fut = pull_async(address);
  auto status = fut.wait_for(std::chrono::milliseconds(3000));

#if defined(OSSIA_BENCHMARK)
  auto t2 = std::chrono::high_resolution_clock::now();
  ossia::logger().info(
      "Time taken: {}",
      std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count());
#endif
  return status == std::future_status::ready;
}

std::future<void>
oscquery_mirror_asio_protocol::pull_async(net::parameter_base& address)
{
  std::promise<void> promise;
  auto fut = promise.get_future();
  auto text = address.get_node().osc_address();
  m_getWSPromises.enqueue(get_ws_promise{std::move(promise), text});
  /*
  m_getOSCPromises.insert(
        std::make_pair(
          address.get_node().osc_address(),
          get_promise{std::move(promise), &address}));
  */
  text += detail::query_value();
  http_send_message(text);
  return fut;
}

void oscquery_mirror_asio_protocol::request(net::parameter_base& address)
{
  auto text = address.get_node().osc_address();
  text += detail::query_value();
  http_send_message(text);
}

void oscquery_mirror_asio_protocol::osc_send_message(const net::parameter_base& addr, const ossia::value& val)
{
  using namespace ossia::net;
  using send_visitor = osc_value_send_visitor<ossia::net::parameter_base, osc_extended_policy, udp_socket>;
  send_visitor vis{addr, addr.get_node().osc_address(), this->m_oscSender->socket};
  val.apply(vis);
}

void oscquery_mirror_asio_protocol::osc_send_message(const net::full_parameter_data& addr, const ossia::value& val)
{
  using namespace ossia::net;
  using send_visitor = osc_value_send_visitor<ossia::net::full_parameter_data, osc_extended_policy, udp_socket>;
  send_visitor vis{addr, addr.address, this->m_oscSender->socket};
  val.apply(vis);
}

bool oscquery_mirror_asio_protocol::push(const net::parameter_base& addr, const ossia::value& v)
{
  if (addr.get_access() == ossia::access_mode::GET)
    return false;

  auto val = net::filter_value(addr, v);
  if (val.valid())
  {
    // Push to server
    auto critical = addr.get_critical();
    if ((!critical || !m_hasWS) && m_oscSender)
    {
      if (m_logger.outbound_logger)
      {
        m_logger.outbound_logger->info("Out: {} {}", addr.get_node().osc_address(), val);
      }
      osc_send_message(addr, val);
    }
    else if (m_hasWS)
    {
      if (m_logger.outbound_logger)
      {
        m_logger.outbound_logger->info("Out: {} {}", addr.get_node().osc_address(), val);
      }
      ws_send_binary_message(osc_writer::to_message(addr, val));
    }

    if (m_logger.outbound_listened_logger)
      m_logger.outbound_listened_logger->info("Out: {0}", addr, val);
    return true;
  }

  return false;
}

bool oscquery_mirror_asio_protocol::echo_incoming_message(
    const ossia::net::message_origin_identifier& id, const ossia::net::parameter_base& addr, const value& val)
{
  if(&id.protocol == this && id.identifier == (uintptr_t)this->m_websocketClient.get())
    return true;

  auto critical = addr.get_critical();
  if ((!critical || !m_hasWS) && m_oscSender)
  {
    if (m_logger.outbound_logger)
    {
      m_logger.outbound_logger->info("Out: {} {}", addr.get_node().osc_address(), val);
    }
    osc_send_message(addr, val);
    return true;
  }
  else if (m_hasWS)
  {
    if (m_logger.outbound_logger)
    {
      m_logger.outbound_logger->info("Out: {} {}", addr.get_node().osc_address(), val);
    }
    ws_send_binary_message(osc_writer::to_message(addr, val));
    return true;
  }
  return false;
}


bool oscquery_mirror_asio_protocol::push_raw(const net::full_parameter_data& addr)
{
  if (addr.get_access() == ossia::access_mode::GET)
    return false;

  auto val = net::filter_value(addr, addr.value());
  if (val.valid())
  {
    // Push to server
    auto critical = addr.get_critical();
    if ((!critical || !m_hasWS) && m_oscSender)
    {
      if (m_logger.outbound_logger)
      {
        m_logger.outbound_logger->info("Out: {} {}", addr.address, val);
      }
      osc_send_message(addr, val);
    }
    else if (m_hasWS)
    {
      if (m_logger.outbound_logger)
      {
        m_logger.outbound_logger->info("Out: {} {}", addr.address, val);
      }
      ws_send_binary_message(osc_writer::to_message(addr, val));
    }
    return true;
  }
  return false;
}

bool oscquery_mirror_asio_protocol::push_bundle(
    const std::vector<const ossia::net::parameter_base*>& addresses)
{
  /* TODO
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

  m_websocketClient->send_binary_message(b.finish());
  */
  return false;
}

bool oscquery_mirror_asio_protocol::push_raw_bundle(
    const std::vector<ossia::net::full_parameter_data>& addresses)
{
  /* TODO
  json_bundle_builder b;
  for(const auto& addr : addresses)
  {
    ossia::value val = net::filter_value(addr);
    if (val.valid())
    {
      b.add_message(addr, val);
    }
  }

  m_websocketClient->send_message(b.finish());
  */
  return false;
}

bool oscquery_mirror_asio_protocol::observe(
    net::parameter_base& address, bool enable)
{
  if (enable)
  {
    auto str = address.get_node().osc_address();

    if (m_hasWS)
      ws_send_message(json_writer::listen(str));

    m_listening.insert(std::make_pair(std::move(str), &address));
  }
  else
  {
    const auto& str = address.get_node().osc_address();

    if (m_hasWS)
      ws_send_message(json_writer::ignore(str));

    m_listening.erase(str);
  }
  return true;
}

bool oscquery_mirror_asio_protocol::observe_quietly(
    net::parameter_base& address, bool enable)
{
  if (enable)
    m_listening.insert(
        std::make_pair(address.get_node().osc_address(), &address));
  else
    m_listening.erase(address.get_node().osc_address());

  return true;
}

bool oscquery_mirror_asio_protocol::update(net::node_base& b)
{
  auto fut = update_async(b);
  auto status = fut.wait_for(std::chrono::seconds(3));
  return status == std::future_status::ready;
}

std::future<void> oscquery_mirror_asio_protocol::update_async(net::node_base& b)
{
  m_namespacePromise = std::promise<void>{};
  auto fut = m_namespacePromise.get_future();
  http_send_message(b.osc_address());
  return fut;
}

void oscquery_mirror_asio_protocol::on_nodeRenamed(
    const net::node_base& n, std::string oldname) try
{
  // Normally, the only reason this function could be called
  // is because of a path_renamed oscquery message
  {
    // Local listening
    auto old_addr = n.osc_address();
    auto it = old_addr.find_last_of('/');
    old_addr.resize(it + 1);
    old_addr += oldname;
    m_listening.rename(old_addr, n.osc_address());
  }
}
catch (const std::exception& e)
{
  logger().error("oscquery_mirror_asio_protocol::on_nodeRenamed: {}", e.what());
}
catch (...)
{
  logger().error("oscquery_mirror_asio_protocol::on_nodeRenamed: error.");
}

void oscquery_mirror_asio_protocol::set_device(net::device_base& dev)
{
  if (m_device)
  {
    auto& old = *m_device;
    old.on_node_renamed.disconnect<&oscquery_mirror_asio_protocol::on_nodeRenamed>(
        this);

    ossia::net::visit_parameters(
        old.get_root_node(),
        [&](ossia::net::node_base& n, ossia::net::parameter_base& p) {
          if (p.callback_count() > 0)
            observe(p, false);
        });
  }

  m_device = &dev;

  init();

  m_device->on_node_renamed.connect<&oscquery_mirror_asio_protocol::on_nodeRenamed>(
      this);
  ossia::net::visit_parameters(
      dev.get_root_node(),
      [&](ossia::net::node_base& n, ossia::net::parameter_base& p) {
        if (p.callback_count() > 0)
          observe(p, true);
      });
}

void oscquery_mirror_asio_protocol::run_commands()
{
  bool ok = false;
  std::function<void()> cmd;
  do
  {
    ok = m_functionQueue.try_dequeue(cmd);
    if (ok && cmd)
      cmd();
  } while (ok);
}

void oscquery_mirror_asio_protocol::set_command_callback(std::function<void()> cb)
{
  m_commandCallback = std::move(cb);
}

void oscquery_mirror_asio_protocol::request_remove_node(net::node_base& self)
{
  if (auto parent = self.get_parent())
  {
    std::string req;
    req.reserve(64);
    req = parent->osc_address();
    req += '?';
    req += detail::remove_node();
    req += '=';
    req += self.get_name();

    http_send_message(std::move(req));
  }
}

void oscquery_mirror_asio_protocol::request_rename_node(net::node_base& node, const std::string& new_name)
{
  std::string req;
  req.reserve(64);
  req = node.osc_address();
  req += '?';
  req += detail::rename_node();
  req += '=';
  req += new_name;

  http_send_message(std::move(req));
}

void oscquery_mirror_asio_protocol::set_disconnect_callback(std::function<void()> f)
{
  if(m_hasWS)
  {
    if(f)
    {
      m_websocketClient->onClose = [fun=std::move(f),&ws=m_hasWS] { ws = false; fun(); };
    }
    else
    {
      m_websocketClient->onClose = [&ws=m_hasWS] { ws = false; };
    }
  }
}

void oscquery_mirror_asio_protocol::set_fail_callback(std::function<void()> f)
{
  if(m_hasWS)
  {
    if(f)
    {
      m_websocketClient->onFail = [fun=std::move(f),&ws=m_hasWS] { ws = false; fun(); };
    }
    else
    {
      m_websocketClient->onFail = [&ws=m_hasWS] { ws = false; };
    }
  }
}

host_info oscquery_mirror_asio_protocol::get_host_info() const noexcept
{
  return m_host_info;
}

void oscquery_mirror_asio_protocol::reconnect()
{
    // TODO ws: disconnect / close / finish_connection
  init();
}

void oscquery_mirror_asio_protocol::process_raw_osc_data(const char* data, std::size_t sz)
{
  auto on_message = [this] (auto&& msg) { this->on_osc_message(msg); };
  ossia::net::osc_packet_processor<decltype(on_message)>{on_message}({data, sz});

}
void oscquery_mirror_asio_protocol::init()
{
  m_oscServer = std::make_unique<osc_receiver_impl>(ossia::net::udp_socket{"127.0.0.1", (uint16_t)m_osc_port, this->m_ctx->context});
  m_oscServer->socket.open();
  m_oscServer->socket.receive([this] (const char* data, std::size_t sz) { process_raw_osc_data(data, sz); });

  m_websocketClient = std::make_unique<websocket_client>(
      [this](connection_handler hdl, websocketpp::frame::opcode::value op,
          std::string& msg) {
        switch (op)
        {
          case websocketpp::frame::opcode::value::TEXT:
            this->on_text_ws_message(hdl, msg);
            break;
          case websocketpp::frame::opcode::value::BINARY:
            this->on_binary_ws_message(hdl, msg);
            break;
          default:
            break;
        }
      });
  m_id.identifier = (uintptr_t) m_websocketClient.get();

  m_websocketClient->onClose = [&ws=m_hasWS] { ws = false; };
  m_websocketClient->onFail = [&ws=m_hasWS] { ws = false; };

  start_http();

  m_hasWS = true;
  std::atomic_bool started{false};

  try
  {
    started = true;
    m_websocketClient->connect(m_queryHost);
  }
  catch (...)
  {
    // Websocket does not connect, so let's try http requests
    // m_websocketClient.reset(); // TODO unsafe non-atomic access
  }
  m_hasWS = false;

  //m_websocketClient.reset(); TODO
  if(!query_connected())
  {
    {
      cleanup_connections();
      throw ossia::connection_error{
          "oscquery_mirror_asio_protocol::oscquery_mirror_asio_protocol: "
          "Could not connect to "
          + m_queryHost};
    }
  }

  http_send_message("/?HOST_INFO");
  /*
  m_oscServer->run();
  */
}

void oscquery_mirror_asio_protocol::start_http()
{
  m_http->worker = std::make_shared<asio::io_service::work>(m_ctx->context);
}

void oscquery_mirror_asio_protocol::request_add_node(
    net::node_base& parent, const net::parameter_data& dat)
{
  std::string req;
  req.reserve(64);
  req += parent.osc_address();
  req += '?';
  req += detail::add_node();
  req += '=';
  req += dat.name;

  // TODO the other attributes
  http_send_message(std::move(req));
}

void oscquery_mirror_asio_protocol::on_osc_message(
    const oscpack::ReceivedMessage& m)
{
#if defined(OSSIA_BENCHMARK)
  auto t1 = std::chrono::high_resolution_clock::now();
#endif

  ossia::net::on_input_message<true>(
        m.AddressPattern(),
        ossia::net::osc_message_applier{m_id, m},
        m_listening, *m_device, m_logger);

#if defined(OSSIA_BENCHMARK)
  auto t2 = std::chrono::high_resolution_clock::now();
  ossia::logger().info(
      "on_OSCMessage : Time taken: {}",
      std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count());
#endif
}

std::string asio_to_ip(std::string uri)
{
  uri = boost::algorithm::ierase_first_copy(uri, "http://");
  uri = boost::algorithm::ierase_first_copy(uri, "https://");
  uri = boost::algorithm::ierase_first_copy(uri, "ws://");
  uri = boost::algorithm::ierase_first_copy(uri, "wss://");

  auto pos = uri.find_last_of(':');
  if (pos != std::string::npos)
    uri.erase(pos, uri.size());

  return uri;
}

bool oscquery_mirror_asio_protocol::on_text_ws_message(
    oscquery_mirror_asio_protocol::connection_handler, const std::string& message)
{
#if defined(OSSIA_BENCHMARK)
  auto t1 = std::chrono::high_resolution_clock::now();
#endif
  try
  {
    std::shared_ptr<rapidjson::Document> data = json_parser::parse(message);
    if (data->IsNull())
    {
      if (m_logger.inbound_logger)
        m_logger.inbound_logger->warn(
            "Invalid WS message received: {}", message);
      return false;
    }
    if (data->IsArray())
    {
      // TODO
    }
    else
    {

      switch (json_parser::message_type(*data))
      {
        case message_type::HostInfo:
        {
          // TODO oscquery_mirror should actually take a host_info
          // as argument - or we should provide a factory function.
          // The ip of the OSC server on the server
          m_host_info = json_parser::parse_host_info(*data);
          if (!m_host_info.osc_ip)
            m_host_info.osc_ip = m_queryHost;
          if (!m_host_info.osc_port)
            m_host_info.osc_port = boost::lexical_cast<int>(m_queryPort);
          if (m_host_info.osc_transport == host_info::UDP)
          {
            const auto& server_host = asio_to_ip(*m_host_info.osc_ip);
            uint16_t server_port = uint16_t(*m_host_info.osc_port);
            m_oscSender = std::make_unique<osc_sender_impl>(ossia::net::udp_socket{server_host, server_port, this->m_ctx->context});
            m_oscSender->socket.connect();
            uint16_t local_server_port = m_oscServer->socket.m_socket.local_endpoint().port();
            uint16_t local_sender_port = m_oscSender->socket.m_socket.local_endpoint().port();

            // Send to the server the local receiver port
            if (m_host_info.extensions["OSC_STREAMING"])
            {
              ws_send_message(json_writer::start_osc_streaming(local_server_port, local_sender_port));
            }
          }
          else
          {
            // TODO
          }

          break;
        }
        case message_type::Namespace:
        {
          json_parser::parse_namespace(m_device->get_root_node(), *data);
          m_namespacePromise.set_value();
          break;
        }
        case message_type::Value:
        {
          // TODO instead just put full path in reply ?
          get_ws_promise p;
          if (m_getWSPromises.try_dequeue(p))
          {
            auto node
                = ossia::net::find_node(m_device->get_root_node(), p.address);

            const rapidjson::Value* obj_value = data.get();
            if(obj_value->IsObject())
            {
                if(auto it = obj_value->FindMember("VALUE");
                        it != obj_value->MemberEnd())
                {
                    obj_value = &it->value;
                }
            }

            if (node)
            {
              auto addr = node->get_parameter();
              if (addr)
              {
                json_parser::parse_value(*addr, *obj_value);
                m_device->on_message(*addr);
              }
              else
              {
                m_device->on_unhandled_message(
                    p.address, detail::ReadValue(*obj_value));
              }
            }
            else
            {
              m_device->on_unhandled_message(
                  p.address, oscquery::detail::ReadValue(*obj_value));
            }

            p.promise.set_value();
          }

          else // if update from critical param
          {
            for (auto it = data->MemberBegin(), end = data->MemberEnd();
                 it != end; ++it)
            {
              auto path = get_string_view(it->name);
              if (!path.empty() && path[0] == '/')
              {
                auto node
                    = ossia::net::find_node(m_device->get_root_node(), path);
                if (node)
                {
                  auto addr = node->get_parameter();
                  if (addr)
                  {
                    auto val = addr->value();
                    json_parser::parse_value(*addr, it->value);
                    // val.apply(detail::json_to_value_unchecked{it->value});
                    m_device->on_message(*addr);
                    continue;
                  }
                }
              }

              m_device->on_unhandled_message(
                  std::string(path), detail::ReadValue(it->value));
            }
          }

          break;
        }

        case message_type::PathAdded:
        {
          auto dat_it = data->FindMember(detail::data());
          if (dat_it != data->MemberEnd())
          {
            auto& dat = dat_it->value;
            if (dat.IsString())
            {
              std::string full_path{dat.GetString(), dat.GetStringLength()};

              m_functionQueue.enqueue([this, f = std::move(full_path), doc = std::move(data)] {
                json_parser::parse_path_added(
                    m_device->get_root_node(), f, *doc);
              });
            }
          }
          if (m_commandCallback)
            m_commandCallback();
          break;
        }

        case message_type::PathRenamed:
        {
          m_functionQueue.enqueue([this, doc = std::move(data)] {
            json_parser::parse_path_renamed(m_device->get_root_node(), *doc);
          });
          if (m_commandCallback)
            m_commandCallback();
          break;
        }

        case message_type::PathChanged:
        {
          m_functionQueue.enqueue([this, doc = std::move(data)] {
            json_parser::parse_path_changed(m_device->get_root_node(), *doc);
          });
          if (m_commandCallback)
            m_commandCallback();
          break;
        }

        case message_type::PathRemoved:
        {
          m_functionQueue.enqueue([this, doc = std::move(data)] {
            json_parser::parse_path_removed(m_device->get_root_node(), *doc, m_zombie_on_remove);
          });
          if (m_commandCallback)
            m_commandCallback();
          break;
        }

        case message_type::AttributesChanged:
        {
          m_functionQueue.enqueue([this, doc = std::move(data),message] {
            ossia::net::parameter_base* request_value = nullptr;
            json_parser::parse_attributes_changed(
                m_device->get_root_node(), *doc, request_value);
            // TODO investigate why those are called twice in OSCQueryValueCallbackTest
            if(request_value)
            {
              pull_async(*request_value);
            }
          });
          if (m_commandCallback)
            m_commandCallback();
          break;
        }
        default:
          break;
      }
    }
  }
  catch (std::exception& e)
  {
    if (m_logger.inbound_logger)
      m_logger.inbound_logger->warn(
          "Error while parsing: {} ==> {}", e.what(), message);
    return false;
  }

  if (m_logger.inbound_logger)
    m_logger.inbound_logger->info("WS In: {}", message);
#if defined(OSSIA_BENCHMARK)
  auto t2 = std::chrono::high_resolution_clock::now();
  ossia::logger().info(
      "on_WSMessage : Time taken: {}",
      std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count());
#endif
  return true;
}

bool oscquery_mirror_asio_protocol::on_binary_ws_message(
    oscquery_mirror_asio_protocol::connection_handler hdl,
    const std::string& message)
{
  process_raw_osc_data(message.data(), message.size());

  return {};
}
}
}
