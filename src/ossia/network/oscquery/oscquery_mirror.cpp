// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "oscquery_mirror.hpp"

#include <ossia/detail/hash_map.hpp>
#include <ossia/detail/thread.hpp>
#include <ossia/network/base/device.hpp>
#include <ossia/network/common/node_visitor.hpp>
#include <ossia/network/exceptions.hpp>
#include <ossia/network/http/http_client.hpp>
#include <ossia/network/osc/detail/osc.hpp>
#include <ossia/network/osc/detail/osc_receive.hpp>
#include <ossia/network/osc/detail/receiver.hpp>
#include <ossia/network/osc/detail/sender.hpp>
#include <ossia/network/oscquery/detail/json_parser.hpp>
#include <ossia/network/oscquery/detail/json_writer.hpp>
#include <ossia/network/oscquery/detail/osc_writer.hpp>
#include <ossia/network/oscquery/detail/outbound_visitor.hpp>
#include <ossia/network/oscquery/detail/value_to_json.hpp>
#include <ossia/network/sockets/websocket_client.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/erase.hpp>
#include <boost/asio/steady_timer.hpp>

#include <atomic>
#include <mutex>
#include <stdexcept>
#include <vector>
namespace ossia::oscquery
{
struct http_answer
{
  static constexpr auto reserve_expect = 65536 * 8;
  oscquery_mirror_protocol& self;

  template <typename T, typename S>
  void operator()(T& req, const S& str)
  {
    self.on_WSMessage({}, str);
  }
};

struct http_error
{
  template <typename T>
  void operator()(T& req)
  {
    req.close();
  }
};

using http_request = ossia::net::http_get_request<http_answer, http_error>;

// set_read_after_listen: the reads that follow a LISTEN. Every member under
// the mutex, but `tracked`.
struct listen_reads
{
  using clock = std::chrono::steady_clock;
  std::mutex mutex;
  bool alive{true};
  oscquery_mirror_protocol::listen_read_mode mode{
      oscquery_mirror_protocol::listen_read_mode::unknown};
  bool sent_any{};
  // Sent on the WebSocket, not answered yet: by address, when, oldest first
  ossia::hash_map<std::string, std::vector<clock::time_point>> waiting;
  std::size_t waiting_count{};
  // Sent over HTTP, not answered yet: by address, how many
  ossia::hash_map<std::string, int> applying;
  // By address, when a value last came while one of its reads was in one of
  // the two above
  ossia::hash_map<std::string, clock::time_point> arrived;
  // Addresses in waiting or applying: the path of every value checks it
  // before taking the mutex
  std::atomic<std::size_t> tracked{0};
  std::unique_ptr<boost::asio::steady_timer> timer;
  bool timer_armed{};

  void update_tracked() { tracked = waiting.size() + applying.size(); }

  // Whether a value of address came after a read of it sent at `sent`; what
  // came is forgotten once no read of address is left
  bool overtaken(const std::string& address, clock::time_point sent)
  {
    bool res = false;
    if(auto it = arrived.find(address); it != arrived.end())
      res = it->second >= sent;
    if(waiting.find(address) == waiting.end()
       && applying.find(address) == applying.end())
      arrived.erase(address);
    update_tracked();
    return res;
  }
};

// A value read: its answer goes to the address it was asked for, whatever
// the order the answers come back in, and a failed read fails its future
struct value_read
{
  std::string address;
  std::promise<void> promise;
  std::atomic_flag done = ATOMIC_FLAG_INIT;
  //! The answer is applied only if it changes the value
  bool only_if_differs{};
  //! For a read of our own, when it was asked: a value this client writes
  //! after is newer than the answer
  std::chrono::steady_clock::time_point asked{};
  //! The read that follows a LISTEN, over HTTP
  std::weak_ptr<listen_reads> listen;
  bool listen_done{};

  value_read() = default;
  value_read(const value_read&) = delete;
  value_read& operator=(const value_read&) = delete;
  ~value_read() { finish_listen(); }

  //! For the read that follows a LISTEN: whether a value of the address
  //! came after it was asked, which makes its answer older
  bool finish_listen()
  {
    if(listen_done)
      return false;
    listen_done = true;
    auto st = listen.lock();
    if(!st)
      return false;
    std::lock_guard lock{st->mutex};
    if(auto it = st->applying.find(address);
       it != st->applying.end() && --it->second <= 0)
      st->applying.erase(it);
    return st->overtaken(address, asked);
  }

  void succeed()
  {
    if(!done.test_and_set())
      promise.set_value();
  }
  void fail()
  {
    if(!done.test_and_set())
      promise.set_exception(
          std::make_exception_ptr(std::runtime_error("value read failed: " + address)));
  }
};

struct http_value_answer
{
  static constexpr auto reserve_expect = 65536 * 8;
  oscquery_mirror_protocol& self;
  std::shared_ptr<value_read> read;

  template <typename T, typename S>
  void operator()(T& req, int status, const S& body)
  {
    if(status >= 200 && status < 300)
    {
      // An answer older than a write of this client would bring back the
      // value the write replaced
      const bool stale = read->asked != std::chrono::steady_clock::time_point{}
                         && self.written_since(read->address, read->asked);
      // The read that follows a LISTEN: a value that came since is newer
      const bool overtaken = read->finish_listen();
      if(!stale && !overtaken)
        self.on_value_read(read->address, body, read->only_if_differs);
      read->succeed();
    }
    else
    {
      ossia::logger().error("HTTP Error: {}: status code {}", read->address, status);
      read->fail();
    }
  }
};

struct http_value_error
{
  std::shared_ptr<value_read> read;

  template <typename T>
  void operator()(T& req, std::string_view)
  {
    read->fail();
    req.close();
  }
};

struct http_client_context
{
  ossia::thread thread;
  boost::asio::io_context context;
  std::shared_ptr<boost::asio::executor_work_guard<boost::asio::io_context::executor_type>> worker;
};

struct oscquery_mirror_protocol::crossing_state
{
  std::mutex mutex;
  bool alive{true};
  struct sent_value
  {
    std::chrono::steady_clock::time_point time;
    ossia::value value;
  };
  // By address, the last value this client wrote
  ossia::hash_map<std::string, sent_value> sent;
  // By address, the reading to come
  ossia::hash_map<std::string, std::unique_ptr<boost::asio::steady_timer>> rereads;
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

oscquery_mirror_protocol::oscquery_mirror_protocol(
    std::string host, uint16_t local_osc_port)
    : protocol_base{flags{SupportsMultiplex}}
    , m_queryHost{std::move(host)}
    , m_httpHost{m_queryHost}
    , m_osc_port{local_osc_port}
    , m_http{std::make_unique<http_client_context>()}
    , m_id{*this, {}}
{
  m_crossing = std::make_shared<crossing_state>();
  m_listenReads = std::make_shared<listen_reads>();
  auto port_idx = m_queryHost.find_last_of(':');
  if(port_idx != std::string::npos)
    m_queryPort = m_queryHost.substr(port_idx + 1);
  else
    m_queryPort = "80";

  // for http, host should be only the name, e.g. example.com instead of
  // http://example.com:1234
  if(port_idx != std::string::npos)
    m_httpHost.erase(m_httpHost.begin() + port_idx, m_httpHost.end());
  if(boost::starts_with(m_httpHost, "http://"))
    m_httpHost.erase(m_httpHost.begin(), m_httpHost.begin() + 7);
  else if(boost::starts_with(m_httpHost, "ws://"))
    m_httpHost.erase(m_httpHost.begin(), m_httpHost.begin() + 5);
}

void oscquery_mirror_protocol::stop()
{
  cleanup_connections();
}

void oscquery_mirror_protocol::cleanup_connections()
{
  try
  {
    m_oscServer->stop();
  }
  catch(...)
  {
    logger().error("Error when stopping osc server");
  }

  {
    // No reading after the connections are gone
    std::lock_guard lock{m_crossing->mutex};
    m_crossing->alive = false;
    for(auto& [address, timer] : m_crossing->rereads)
      timer->cancel();
  }
  {
    // No read after a LISTEN once the connections are gone
    std::lock_guard lock{m_listenReads->mutex};
    auto& st = *m_listenReads;
    st.alive = false;
    if(st.timer)
      st.timer->cancel();
    st.waiting.clear();
    st.waiting_count = 0;
    st.update_tracked();
  }

  if(query_connected())
  {
    try
    {
      query_stop();
    }
    catch(...)
    {
      logger().error("Error when stopping WS server");
    }
  }

  if(m_wsThread.joinable())
  {
    try
    {
      query_stop();
      m_wsThread.join();
    }
    catch(std::exception& e)
    {
      logger().error("Error when stopping WS thread: {}", e.what());
    }
    catch(...)
    {
      logger().error("Error when stopping WS thread");
    }
  }

  try
  {
    m_http->context.stop();
    m_http->worker.reset();
    if(m_http->thread.joinable())
      m_http->thread.join();
  }
  catch(...)
  {
    logger().error("Error when stopping HTTP thread");
  }

  {
    // The timers go with the context they wait on
    std::lock_guard lock{m_crossing->mutex};
    m_crossing->rereads.clear();
  }
  {
    std::lock_guard lock{m_listenReads->mutex};
    m_listenReads->timer.reset();
    m_listenReads->timer_armed = false;
  }
}

void oscquery_mirror_protocol::http_send_message(const std::string& str)
{
  auto hrq = std::make_shared<http_request>(
      http_answer{*this}, http_error{}, m_http->context, m_httpHost, str);
  hrq->resolve(m_httpHost, m_queryPort);
}

void oscquery_mirror_protocol::http_send_message(const rapidjson::StringBuffer& str)
{
  auto hrq = std::make_shared<http_request>(
      http_answer{*this}, http_error{}, m_http->context, m_httpHost, str.GetString());
  hrq->resolve(m_httpHost, m_queryPort);
}

void oscquery_mirror_protocol::ws_send_message(const std::string& str)
{
  if(m_hasWS)
    m_websocketClient->send_message(str);
}

void oscquery_mirror_protocol::ws_send_message(const rapidjson::StringBuffer& str)
{
  if(m_hasWS)
    m_websocketClient->send_message(str);
}

void oscquery_mirror_protocol::ws_send_binary_message(const std::string& str)
{
  if(m_hasWS)
    m_websocketClient->send_binary_message(str);
}

bool oscquery_mirror_protocol::query_connected()
{
  if(m_hasWS)
    return m_websocketClient->connected();
  return true;
}

void oscquery_mirror_protocol::query_stop()
{
  if(m_hasWS)
  {
    m_hasWS = false;
    m_websocketClient->stop();
  }
}

oscquery_mirror_protocol::~oscquery_mirror_protocol()
{
  stop();
}

bool oscquery_mirror_protocol::pull(net::parameter_base& address)
{
#if defined(OSSIA_BENCHMARK)
  auto t1 = std::chrono::high_resolution_clock::now();
#endif
  auto fut = pull_async(address);
  auto status = fut.wait_for(std::chrono::milliseconds(3000));
  bool ok = false;
  if(status == std::future_status::ready)
  {
    try
    {
      fut.get();
      ok = true;
    }
    catch(...)
    {
    }
  }

#if defined(OSSIA_BENCHMARK)
  auto t2 = std::chrono::high_resolution_clock::now();
  ossia::logger().info(
      "Time taken: {}",
      std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count());
#endif
  return ok;
}

std::future<void> oscquery_mirror_protocol::pull_async(net::parameter_base& address)
{
  auto read = std::make_shared<value_read>();
  read->address = address.get_node().osc_address();
  auto fut = read->promise.get_future();
  read_value(std::move(read));
  return fut;
}

void oscquery_mirror_protocol::request(net::parameter_base& address)
{
  auto read = std::make_shared<value_read>();
  read->address = address.get_node().osc_address();
  read_value(std::move(read));
}

void oscquery_mirror_protocol::read_value(std::shared_ptr<value_read> read)
{
  using request = ossia::net::http_client_request<http_value_answer, http_value_error>;
  auto path = read->address;
  path += detail::query_value();
  auto hrq = std::make_shared<request>(
      http_value_answer{*this, read}, http_value_error{read}, m_http->context, "GET",
      m_httpHost, path);
  hrq->resolve(m_httpHost, m_queryPort);
}

void oscquery_mirror_protocol::on_value_read(
    const std::string& address, const std::string& message, bool only_if_differs)
{
  std::shared_ptr<rapidjson::Document> data = json_parser::parse(message);
  if(data->IsNull())
  {
    if(m_logger.inbound_logger)
      m_logger.inbound_logger->warn("Invalid value received: {}", message);
    return;
  }

  const rapidjson::Value* obj_value = data.get();
  if(obj_value->IsObject())
  {
    if(auto it = obj_value->FindMember("VALUE"); it != obj_value->MemberEnd())
    {
      obj_value = &it->value;
    }
  }
  apply_value(address, *obj_value, only_if_differs);
}

void oscquery_mirror_protocol::apply_value(
    const std::string& address, const rapidjson::Value& obj, bool only_if_differs)
{
  const rapidjson::Value* obj_value = &obj;
  auto node = ossia::net::find_node(m_device->get_root_node(), address);
  if(node)
  {
    if(auto addr = node->get_parameter())
    {
      if(only_if_differs)
      {
        // A read of our own: what follows the parameter gets the answer only
        // if it is news
        auto val = addr->value();
        val.apply(detail::json_to_value_unchecked{*obj_value});
        if(val != addr->value())
        {
          addr->set_value(std::move(val));
          m_device->on_message(*addr);
        }
      }
      else
      {
        json_parser::parse_value(*addr, *obj_value);
        m_device->on_message(*addr);
      }
      return;
    }
  }
  m_device->on_unhandled_message(address, detail::ReadValue(*obj_value));
}

void oscquery_mirror_protocol::set_feedback(bool b)
{
  m_feedback = b;
}

bool oscquery_mirror_protocol::push(
    const net::parameter_base& addr, const ossia::value& v)
{
  if(!m_feedback)
    return false;

  if(addr.get_access() == ossia::access_mode::GET)
    return false;

  auto val = bound_value(addr, v);
  if(val.valid())
  {
    // Push to server
    auto critical = addr.get_critical();
    const auto& osc_addr = addr.get_node().osc_address();
    // Before it leaves: the server's value may come back before this returns
    record_sent(osc_addr, val);
    if((!critical || !m_hasWS) && m_oscSender)
    {
      if(m_logger.outbound_logger)
      {
        m_logger.outbound_logger->info("Out: {} {}", osc_addr, val);
      }
      ossia::oscquery::osc_writer::send_message(addr, val, m_oscSender->socket());
    }
    else if(m_hasWS)
    {
      if(m_logger.outbound_logger)
      {
        m_logger.outbound_logger->info("Out: {} {}", osc_addr, val);
      }
      ws_send_binary_message(osc_writer::to_message(addr, val));
    }

    if(m_logger.outbound_listened_logger)
      m_logger.outbound_listened_logger->info("Out: {0}", osc_addr, val);
    return true;
  }
  return false;
}

bool oscquery_mirror_protocol::echo_incoming_message(
    const ossia::net::message_origin_identifier& id,
    const ossia::net::parameter_base& addr, const value& val)
{
  if(!m_feedback)
    return false;

  if(&id.protocol == this && id.identifier == (uintptr_t)this->m_websocketClient.get())
    return true;

  auto critical = addr.get_critical();
  if((!critical || !m_hasWS) && m_oscSender)
  {
    if(m_logger.outbound_logger)
    {
      m_logger.outbound_logger->info("Out: {} {}", addr.get_node().osc_address(), val);
    }
    ossia::oscquery::osc_writer::send_message(addr, val, m_oscSender->socket());
    return true;
  }
  else if(m_hasWS)
  {
    if(m_logger.outbound_logger)
    {
      m_logger.outbound_logger->info("Out: {} {}", addr.get_node().osc_address(), val);
    }
    ws_send_binary_message(osc_writer::to_message(addr, val));
    return true;
  }
  return false;
}

bool oscquery_mirror_protocol::push_raw(const net::full_parameter_data& addr)
{
  if(!m_feedback)
    return false;

  if(addr.get_access() == ossia::access_mode::GET)
    return false;

  auto val = bound_value(addr, addr.value());
  if(val.valid())
  {
    // Push to server
    auto critical = addr.get_critical();
    record_sent(addr.address, val);
    if((!critical || !m_hasWS) && m_oscSender)
    {
      if(m_logger.outbound_logger)
      {
        m_logger.outbound_logger->info("Out: {} {}", addr.address, val);
      }
      ossia::oscquery::osc_writer::send_message(addr, val, m_oscSender->socket());
    }
    else if(m_hasWS)
    {
      if(m_logger.outbound_logger)
      {
        m_logger.outbound_logger->info("Out: {} {}", addr.address, val);
      }
      ws_send_binary_message(osc_writer::to_message(addr, val));
    }
    return true;
  }
  return false;
}

bool oscquery_mirror_protocol::push_bundle(
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

bool oscquery_mirror_protocol::push_raw_bundle(
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

bool oscquery_mirror_protocol::observe(net::parameter_base& address, bool enable)
{
  if(enable)
  {
    auto str = address.get_node().osc_address();

    if(m_hasWS)
      ws_send_message(json_writer::listen(str));

    // A value the server changes before it applies the LISTEN is not sent:
    // set_read_after_listen reads it again behind the LISTEN
    if(m_hasWS && m_readAfterListen && address.get_access() != ossia::access_mode::SET)
      read_after_listen(str);

    m_listening.insert(std::make_pair(std::move(str), &address));
  }
  else
  {
    const auto& str = address.get_node().osc_address();

    if(m_hasWS)
      ws_send_message(json_writer::ignore(str));

    m_listening.erase(str);
  }
  return true;
}

bool oscquery_mirror_protocol::observe_quietly(net::parameter_base& address, bool enable)
{
  if(enable)
    m_listening.insert(std::make_pair(address.get_node().osc_address(), &address));
  else
    m_listening.erase(address.get_node().osc_address());

  return true;
}

bool oscquery_mirror_protocol::update(net::node_base& b)
{
  auto fut = update_async(b);
  auto status = fut.wait_for(std::chrono::seconds(3));
  return status == std::future_status::ready;
}

std::future<void> oscquery_mirror_protocol::update_async(net::node_base& b)
{
  m_namespacePromise = std::promise<void>{};
  auto fut = m_namespacePromise.get_future();
  http_send_message(b.osc_address());
  return fut;
}

void oscquery_mirror_protocol::on_nodeRenamed(
    const net::node_base& n, std::string oldname)
try
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
catch(const std::exception& e)
{
  logger().error("oscquery_mirror_protocol::on_nodeRenamed: {}", e.what());
}
catch(...)
{
  logger().error("oscquery_mirror_protocol::on_nodeRenamed: error.");
}

void oscquery_mirror_protocol::set_device(net::device_base& dev)
{
  if(m_device)
  {
    auto& old = *m_device;
    old.on_node_renamed.disconnect<&oscquery_mirror_protocol::on_nodeRenamed>(this);

    ossia::net::visit_parameters(
        old.get_root_node(),
        [&](ossia::net::node_base& n, ossia::net::parameter_base& p) {
      if(p.callback_count() > 0)
        observe(p, false);
        });
  }

  m_device = &dev;

  init();

  m_device->on_node_renamed.connect<&oscquery_mirror_protocol::on_nodeRenamed>(this);
  ossia::net::visit_parameters(
      dev.get_root_node(), [&](ossia::net::node_base& n, ossia::net::parameter_base& p) {
        if(p.callback_count() > 0)
          observe(p, true);
      });
}

void oscquery_mirror_protocol::run_commands()
{
  bool ok = false;
  std::function<void()> cmd;
  do
  {
    ok = m_functionQueue.try_dequeue(cmd);
    if(ok && cmd)
      cmd();
  } while(ok);
}

void oscquery_mirror_protocol::set_command_callback(std::function<void()> cb)
{
  m_commandCallback = std::move(cb);
}

void oscquery_mirror_protocol::request_remove_node(net::node_base& self)
{
  if(auto parent = self.get_parent())
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

void oscquery_mirror_protocol::request_rename_node(
    net::node_base& node, const std::string& new_name)
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

host_info oscquery_mirror_protocol::get_host_info() const noexcept
{
  std::lock_guard lock{m_host_info_mutex};
  return m_host_info;
}

void oscquery_mirror_protocol::connect()
{
  init();
}

void oscquery_mirror_protocol::init()
{
  if(!m_crossing->alive)
    m_crossing = std::make_shared<crossing_state>();
  if(!m_listenReads->alive)
    m_listenReads = std::make_shared<listen_reads>();
  m_oscServer = std::make_unique<osc::receiver>(
      m_osc_port,
      [this](const oscpack::ReceivedMessage& m, const oscpack::IpEndpointName& ip) {
    this->on_OSCMessage(m, ip);
      });

  m_websocketClient = std::make_unique<ossia::net::websocket_client>(
      [this](
          connection_handler hdl, websocketpp::frame::opcode::value op,
          std::string& msg) {
    switch(op)
    {
      case websocketpp::frame::opcode::value::TEXT:
        this->on_WSMessage(hdl, msg);
        break;
      case websocketpp::frame::opcode::value::BINARY:
        this->on_BinaryWSMessage(hdl, msg);
        break;
      default:
        break;
    }
  });
  m_id.identifier = (uintptr_t)m_websocketClient.get();

  m_websocketClient->on_close.connect<&oscquery_mirror_protocol::on_ws_disconnected>(
      *this);
  m_websocketClient->on_fail.connect<&oscquery_mirror_protocol::on_ws_disconnected>(
      *this);

  m_websocketClient->on_open.connect(this->on_connection_open);
  m_websocketClient->on_close.connect(this->on_connection_closed);
  m_websocketClient->on_fail.connect(this->on_connection_failure);

  start_http();

  m_hasWS = true;
  std::atomic_bool started{false};
  m_wsThread = ossia::thread([this, &started] {
    ossia::set_thread_name("ossia oscq ws");
    try
    {
      started = true;
      m_websocketClient->connect_and_run(m_queryHost);
    }
    catch(...)
    {
      // Websocket does not connect, so let's try http requests
      // m_websocketClient.reset(); // TODO unsafe non-atomic access
    }
    m_hasWS = false;

    // m_websocketClient.reset(); TODO
  });

  while(!started)
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  int n = 0;
  while(!query_connected())
  {
    n++;
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    if(n > 1500)
    {
      cleanup_connections();
      throw ossia::connection_error{
          "oscquery_mirror_protocol::oscquery_mirror_protocol: "
          "Could not connect to "
          + m_queryHost};
    }
  }

  http_send_message("/?HOST_INFO=1");
  m_oscServer->run();
}

void oscquery_mirror_protocol::start_http()
{
  m_http->worker = std::make_shared<boost::asio::executor_work_guard<boost::asio::io_context::executor_type>>(m_http->context.get_executor());
  m_http->thread = ossia::thread([this] {
    ossia::set_thread_name("ossia oscq http");
    m_http->context.run();
  });
}

void oscquery_mirror_protocol::request_add_node(
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

void oscquery_mirror_protocol::on_OSCMessage(
    const oscpack::ReceivedMessage& m, const oscpack::IpEndpointName& ip)
{
#if defined(OSSIA_BENCHMARK)
  auto t1 = std::chrono::high_resolution_clock::now();
#endif

  // Before it is applied: a read after LISTEN answered meanwhile is older
  listen_value_arrived(m.AddressPattern());
  ossia::net::on_input_message<true>(
      m.AddressPattern(), ossia::net::osc_message_applier{m_id, m}, m_listening,
      *m_device, m_logger);
  check_crossing(m.AddressPattern());

#if defined(OSSIA_BENCHMARK)
  auto t2 = std::chrono::high_resolution_clock::now();
  ossia::logger().info(
      "on_OSCMessage : Time taken: {}",
      std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count());
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

bool oscquery_mirror_protocol::on_WSMessage(
    oscquery_mirror_protocol::connection_handler, const std::string& message)
{
#if defined(OSSIA_BENCHMARK)
  auto t1 = std::chrono::high_resolution_clock::now();
#endif
  try
  {
    std::shared_ptr<rapidjson::Document> data = json_parser::parse(message);
    if(data->IsNull())
    {
      if(m_logger.inbound_logger)
        m_logger.inbound_logger->warn("Invalid WS message received: {}", message);
      return false;
    }
    if(data->IsArray())
    {
      // TODO
    }
    else if(on_listen_read_answer(*data))
    {
      // The answer to a read that follows a LISTEN
    }
    else
    {

      switch(json_parser::message_type(*data))
      {
        case message_type::HostInfo: {
          // TODO oscquery_mirror should actually take a host_info
          // as argument - or we should provide a factory function.
          // The ip of the OSC server on the server
          auto info = json_parser::parse_host_info(*data);
          {
            std::lock_guard lock{m_host_info_mutex};
            m_host_info = std::move(info);
            if(!m_host_info.osc_ip)
              m_host_info.osc_ip = m_queryHost;
            if(!m_host_info.osc_port)
              m_host_info.osc_port = std::stoi(m_queryPort);
          }
          if(m_host_info.osc_transport == host_info::UDP)
          {
            m_oscSender = std::make_unique<osc::sender<oscquery::osc_outbound_visitor>>(
                m_logger, to_ip(*m_host_info.osc_ip), *m_host_info.osc_port);

            // Send to the server the local receiver port
            if(m_host_info.extensions["OSC_STREAMING"])
            {
              ws_send_message(json_writer::start_osc_streaming(
                  m_oscServer->port(), m_oscSender->localPort()));
            }
          }
          else
          {
            // TODO
          }

          break;
        }
        case message_type::Namespace: {
          json_parser::parse_namespace(m_device->get_root_node(), *data);
          m_namespacePromise.set_value();
          break;
        }
        case message_type::Value: {
          // Values pushed by the server (critical parameters): the answers
          // to value reads go to on_value_read
          {
            for(auto it = data->MemberBegin(), end = data->MemberEnd(); it != end; ++it)
            {
              auto path = get_string_view(it->name);
              if(!path.empty() && path[0] == '/')
              {
                listen_value_arrived(path);
                auto node = ossia::net::find_node(m_device->get_root_node(), path);
                if(node)
                {
                  auto addr = node->get_parameter();
                  if(addr)
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

        case message_type::PathAdded: {
          auto dat_it = data->FindMember(detail::data());
          if(dat_it != data->MemberEnd())
          {
            auto& dat = dat_it->value;
            if(dat.IsString())
            {
              std::string full_path{dat.GetString(), dat.GetStringLength()};

              m_functionQueue.enqueue(
                  [this, f = std::move(full_path), doc = std::move(data)] {
                json_parser::parse_path_added(m_device->get_root_node(), f, *doc);
              });
            }
          }
          if(m_commandCallback)
            m_commandCallback();
          break;
        }

        case message_type::PathRenamed: {
          m_functionQueue.enqueue([this, doc = std::move(data)] {
            json_parser::parse_path_renamed(m_device->get_root_node(), *doc);
          });
          if(m_commandCallback)
            m_commandCallback();
          break;
        }

        case message_type::PathChanged: {
          m_functionQueue.enqueue([this, doc = std::move(data)] {
            json_parser::parse_path_changed(m_device->get_root_node(), *doc);
          });
          if(m_commandCallback)
            m_commandCallback();
          break;
        }

        case message_type::PathRemoved: {
          m_functionQueue.enqueue([this, doc = std::move(data)] {
            json_parser::parse_path_removed(
                m_device->get_root_node(), *doc, m_zombie_on_remove);
          });
          if(m_commandCallback)
            m_commandCallback();
          break;
        }

        case message_type::AttributesChanged: {
          m_functionQueue.enqueue([this, doc = std::move(data), message] {
            ossia::net::parameter_base* request_value = nullptr;
            json_parser::parse_attributes_changed(
                m_device->get_root_node(), *doc, request_value);
            // TODO investigate why those are called twice in
            // OSCQueryValueCallbackTest
            if(request_value)
            {
              pull_async(*request_value);
            }
          });
          if(m_commandCallback)
            m_commandCallback();
          break;
        }
        default:
          break;
      }
    }
  }
  catch(std::exception& e)
  {
    if(m_logger.inbound_logger)
      m_logger.inbound_logger->warn("Error while parsing: {} ==> {}", e.what(), message);
    return false;
  }

  if(m_logger.inbound_logger)
    m_logger.inbound_logger->info("WS In: {}", message);
#if defined(OSSIA_BENCHMARK)
  auto t2 = std::chrono::high_resolution_clock::now();
  ossia::logger().info(
      "on_WSMessage : Time taken: {}",
      std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count());
#endif
  return true;
}

bool oscquery_mirror_protocol::on_BinaryWSMessage(
    oscquery_mirror_protocol::connection_handler hdl, const std::string& message)
{
  auto handler
      = [this](const oscpack::ReceivedMessage& m, const oscpack::IpEndpointName& ip) {
    this->on_OSCMessage(m, ip);
  };
  osc::listener<decltype(handler)> h{handler};
  // TODO use proper ip / port
  h.ProcessPacket(message.data(), message.size(), {});

  return {};
}

void oscquery_mirror_protocol::set_reread_crossings(bool b) noexcept
{
  m_rereadCrossings = b;
}

void oscquery_mirror_protocol::record_sent(
    const std::string& address, const ossia::value& v)
{
  // The reads of our own need the writes of this client
  if(!m_rereadCrossings && !m_readAfterListen)
    return;
  std::lock_guard lock{m_crossing->mutex};
  m_crossing->sent.insert_or_assign(
      address, crossing_state::sent_value{std::chrono::steady_clock::now(), v});
}

bool oscquery_mirror_protocol::written_since(
    const std::string& address, std::chrono::steady_clock::time_point t)
{
  std::lock_guard lock{m_crossing->mutex};
  auto it = m_crossing->sent.find(address);
  return it != m_crossing->sent.end() && it->second.time > t;
}

void oscquery_mirror_protocol::check_crossing(std::string_view address)
{
  if(!m_rereadCrossings)
    return;
  const auto now = std::chrono::steady_clock::now();
  std::lock_guard lock{m_crossing->mutex};
  if(!m_crossing->alive || !m_device)
    return;
  auto it = m_crossing->sent.find(std::string(address));
  if(it == m_crossing->sent.end() || now - it->second.time > crossing_window)
    return;
  auto node = ossia::net::find_node(m_device->get_root_node(), address);
  auto param = node ? node->get_parameter() : nullptr;
  if(!param || param->value() == it->second.value)
    return;

  // The server's value crossed ours: what it holds now is read again once
  // the values that cross have arrived
  auto& timer = m_crossing->rereads[std::string(address)];
  if(!timer)
    timer = std::make_unique<boost::asio::steady_timer>(m_http->context);
  timer->expires_after(settle_delay);
  timer->async_wait([this, weak = std::weak_ptr<crossing_state>{m_crossing},
                     path = std::string(address),
                     asked = now](const boost::system::error_code& ec) {
    if(ec)
      return;
    auto st = weak.lock();
    if(!st)
      return;
    std::lock_guard lock{st->mutex};
    if(!st->alive || !m_device)
      return;
    // A write of ours since: the server takes it after
    auto sent = st->sent.find(path);
    if(sent != st->sent.end() && sent->second.time > asked)
      return;
    // The server may have sent that value already: the same value read
    // again is not a new one
    auto read = std::make_shared<value_read>();
    read->address = path;
    read->only_if_differs = true;
    read->asked = std::chrono::steady_clock::now();
    read_value(std::move(read));
  });
}

oscquery_mirror_protocol::listen_read_mode
oscquery_mirror_protocol::get_listen_read_mode() const
{
  std::lock_guard lock{m_listenReads->mutex};
  return m_listenReads->mode;
}

void oscquery_mirror_protocol::read_after_listen(const std::string& address)
{
  auto st = m_listenReads;
  bool over_ws{};
  {
    std::lock_guard lock{st->mutex};
    if(!st->alive)
      return;
    // Unless the server does not answer on the WebSocket
    over_ws = st->mode != listen_read_mode::http;
    if(over_ws)
    {
      st->waiting[address].push_back(listen_reads::clock::now());
      st->waiting_count++;
      st->sent_any = true;
      st->update_tracked();
    }
  }
  if(!over_ws)
  {
    read_over_http({address});
    return;
  }
  // Behind the LISTEN, on the same connection: the server answers it once
  // the LISTEN is applied. FULL_PATH says which read it answers.
  ws_send_message(
      address + "?" + detail::attribute_value() + "&" + detail::attribute_full_path());
  arm_listen_read_timer();
}

bool oscquery_mirror_protocol::on_listen_read_answer(const rapidjson::Document& d)
{
  if(!d.IsObject())
    return false;
  auto full_path = d.FindMember(detail::attribute_full_path());
  if(full_path == d.MemberEnd() || !full_path->value.IsString())
    return false;
  // What was asked and nothing else: the structure of a node has more
  for(auto it = d.MemberBegin(); it != d.MemberEnd(); ++it)
  {
    auto key = get_string_view(it->name);
    if(key != detail::attribute_full_path() && key != detail::attribute_value())
      return false;
  }

  auto st = m_listenReads;
  std::string address = get_string(full_path->value);
  listen_reads::clock::time_point sent;
  {
    std::lock_guard lock{st->mutex};
    // Not a mirror that reads after LISTEN
    if(!st->sent_any)
      return false;
    // The server answers on the WebSocket, if slowly
    st->mode = listen_read_mode::websocket;
    auto it = st->waiting.find(address);
    if(it == st->waiting.end())
      // Answered after it was sent again over HTTP: nothing to do
      return true;
    sent = it->second.front();
    it->second.erase(it->second.begin());
    st->waiting_count--;
    if(it->second.empty())
      st->waiting.erase(it);
    // A value of the parameter came since: the answer is older
    if(st->overtaken(address, sent))
      return true;
  }

  // A write-only parameter has no VALUE
  auto value = d.FindMember(detail::attribute_value());
  if(value == d.MemberEnd())
    return true;
  // A write of this client since the read was sent is newer
  if(written_since(address, sent))
    return true;
  apply_value(address, value->value, true);
  return true;
}

void oscquery_mirror_protocol::listen_value_arrived(std::string_view address)
{
  auto& st = *m_listenReads;
  if(st.tracked.load(std::memory_order_acquire) == 0)
    return;
  std::string a{address};
  std::lock_guard lock{st.mutex};
  if(st.waiting.find(a) != st.waiting.end() || st.applying.find(a) != st.applying.end())
    st.arrived[std::move(a)] = listen_reads::clock::now();
}

void oscquery_mirror_protocol::arm_listen_read_timer()
{
  auto st = m_listenReads;
  std::lock_guard lock{st->mutex};
  if(!st->alive || st->timer_armed || !m_http)
    return;
  if(!st->timer)
    st->timer = std::make_unique<boost::asio::steady_timer>(m_http->context);
  st->timer_armed = true;
  st->timer->expires_after(std::chrono::milliseconds(100));
  st->timer->async_wait([this, weak = std::weak_ptr<listen_reads>{st}](
                            const boost::system::error_code& ec) {
    if(ec)
      return;
    if(auto s = weak.lock())
      check_listen_reads();
  });
}

void oscquery_mirror_protocol::check_listen_reads()
{
  // On the HTTP thread, from the timer
  auto st = m_listenReads;
  const auto now = listen_reads::clock::now();
  std::vector<std::string> resend;
  bool again{};
  {
    std::lock_guard lock{st->mutex};
    st->timer_armed = false;
    if(!st->alive)
      return;
    if(st->mode == listen_read_mode::unknown)
    {
      // No answer at all yet: a server that does not answer on the WebSocket
      bool silent = false;
      for(const auto& [address, sent] : st->waiting)
        if(!sent.empty() && now - sent.front() >= listen_read_probe)
          silent = true;
      if(silent)
      {
        st->mode = listen_read_mode::http;
        for(auto& [address, sent] : st->waiting)
          resend.push_back(address);
        st->waiting.clear();
        st->waiting_count = 0;
      }
    }
    else
    {
      // A read the server left unanswered (an address it does not have, a
      // query it refused)
      for(auto it = st->waiting.begin(); it != st->waiting.end();)
      {
        auto& sent = it->second;
        bool late = false;
        while(!sent.empty() && now - sent.front() >= listen_read_timeout)
        {
          sent.erase(sent.begin());
          st->waiting_count--;
          late = true;
        }
        if(late)
          resend.push_back(it->first);
        if(sent.empty())
          it = st->waiting.erase(it);
        else
          ++it;
      }
    }
    for(const auto& address : resend)
      if(st->applying.find(address) == st->applying.end())
        st->arrived.erase(address);
    st->update_tracked();
    again = st->waiting_count > 0;
  }
  if(!resend.empty())
    read_over_http(resend);
  if(again)
    arm_listen_read_timer();
}

void oscquery_mirror_protocol::read_over_http(const std::vector<std::string>& addresses)
{
  auto st = m_listenReads;
  {
    std::lock_guard lock{st->mutex};
    if(!st->alive)
      return;
    for(const auto& a : addresses)
      st->applying[a]++;
    st->update_tracked();
  }
  const auto now = listen_reads::clock::now();
  for(const auto& a : addresses)
  {
    auto read = std::make_shared<value_read>();
    read->address = a;
    read->only_if_differs = true;
    read->asked = now;
    read->listen = st;
    read_value(std::move(read));
  }
}

void load_oscquery_device(net::device_base& dev, std::string json)
{
  rapidjson::Document doc;
  doc.Parse(json.c_str());
  if(!doc.HasParseError())
    json_parser::parse_namespace(dev.get_root_node(), doc);
}
}
