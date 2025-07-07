#include "coap_client_protocol.hpp"

#include <ossia/network/base/node.hpp>
#include <ossia/network/base/node_functions.hpp>
#include <ossia/network/base/parameter.hpp>
#include <ossia/network/common/complex_type.hpp>
#include <ossia/protocols/coap/link_format_parser.hpp>

#include <boost/asio/as_tuple.hpp>
#include <boost/asio/bind_executor.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/use_future.hpp>
#include <boost/unordered/concurrent_flat_map.hpp>

#include <coap3/coap.h>

namespace ossia::net
{
struct coap_destination
{
  coap_uri_t uri{};
  coap_address_t dst{};

  static std::optional<coap_destination> parse(const char* coap_uri)
  {
    coap_uri_t uri{};
    coap_address_t dst{};
    int len = coap_split_uri((const unsigned char*)coap_uri, strlen(coap_uri), &uri);
    if(len != 0)
    {
      coap_log_warn("Failed to parse uri %s\n", coap_uri);
      return {};
    }

    len = resolve_address(&uri.host, uri.port, &dst, 1 << uri.scheme);
    if(len <= 0)
    {
      coap_log_warn(
          "Failed to resolve address %*.*s\n", (int)uri.host.length,
          (int)uri.host.length, (const char*)uri.host.s);
      return {};
    }

    return coap_destination{uri, dst};
  }

  static int resolve_address(
      coap_str_const_t* host, uint16_t port, coap_address_t* dst, int scheme_hint_bits)
  {
    int ret = 0;
    coap_addr_info_t* addr_info;

    addr_info = coap_resolve_address_info(
        host, port, port, port, port, AF_UNSPEC, scheme_hint_bits,
        COAP_RESOLVE_TYPE_REMOTE);
    if(addr_info)
    {
      ret = 1;
      *dst = addr_info->addr;
    }

    coap_free_address_info(addr_info);
    return ret;
  }
};

struct coap_context
{
  coap_context() { coap_startup(); }
  ~coap_context() { coap_cleanup(); }
};

enum coap_reply_action
{
  None,
  DeleteSession
};
using coap_reply_callback
    = std::function<coap_reply_action(const coap_pdu_t* received, const coap_mid_t mid)>;
struct coap_session;
struct coap_client
{
  static const inline coap_context lib_ctx;

  coap_context_t* ctx = nullptr;

  coap_client();
  ~coap_client()
  {
    in_flight.clear();
    coap_free_context(ctx);
  }

  std::shared_ptr<coap_session> get(std::string_view req, coap_reply_callback);
  std::shared_ptr<coap_session> subscribe(std::string_view req, coap_reply_callback);
  void unsubscribe(const coap_session& dest);
  std::shared_ptr<coap_session>
  post(std::string_view req, std::string_view fmt, std::string_view bytes);

  void poll() { coap_io_process(ctx, COAP_IO_NO_WAIT); }

private:
  boost::unordered::concurrent_flat_map<
      coap_session_t*, std::shared_ptr<coap_session>, ossia::unknown_pointer_hash<800>>
      in_flight;
};

struct coap_session
{
  coap_client& client;

  coap_session_t* session = nullptr;
  coap_optlist_t* optlist = nullptr;
  coap_pdu_t* pdu = nullptr;

  coap_reply_callback on_reply
      = [](const coap_pdu_t* received, const coap_mid_t mid) -> coap_reply_action {
    return DeleteSession;
  };

  int mid = 0;

  int is_mcast{};
  unsigned char scratch[255];
  static inline int have_response = 0;

  explicit coap_session(
      coap_client& clt, const coap_destination& dest, coap_reply_callback rep = {})
      : client{clt}
  {
    if(rep)
      on_reply = std::move(rep);

    if(dest.uri.scheme == COAP_URI_SCHEME_COAP)
    {
      session = coap_new_client_session(client.ctx, NULL, &dest.dst, COAP_PROTO_UDP);
    }
    else if(dest.uri.scheme == COAP_URI_SCHEME_COAP_TCP)
    {
      session = coap_new_client_session(client.ctx, NULL, &dest.dst, COAP_PROTO_TCP);
    }
    else if(dest.uri.scheme == COAP_URI_SCHEME_COAP_WS)
    {
      session = coap_new_client_session(client.ctx, NULL, &dest.dst, COAP_PROTO_WS);
      coap_ws_set_host_request(session, coap_make_str_const("localhost"));
    }

    if(!session)
      throw std::runtime_error("Cannot create CoAP session");

    coap_session_set_app_data(session, this);

    is_mcast = coap_is_mcast(&dest.dst);
  }

  bool init_message(
      const coap_destination& dest, coap_pdu_type_t confirmable, coap_pdu_code_t code)
  {
    mid = coap_new_message_id(session);
    pdu = coap_pdu_init(confirmable, code, mid, coap_session_max_pdu_size(session));
    if(!pdu)
    {
      return false;
    }

    int len = coap_uri_into_options(
        &dest.uri, &dest.dst, &optlist, 1, scratch, sizeof(scratch));
    if(len)
    {
      return false;
    }
    return true;
  }

  void get(const coap_destination& dest)
  {
    if(!init_message(
           dest, is_mcast ? COAP_MESSAGE_NON : COAP_MESSAGE_CON, COAP_REQUEST_CODE_GET))
      return;

    send();
  }

  void subscribe(const coap_destination& dest)
  {
    if(!init_message(
           dest, is_mcast ? COAP_MESSAGE_NON : COAP_MESSAGE_CON, COAP_REQUEST_CODE_GET))
      return;

    if(!add_option(COAP_OPTION_OBSERVE, COAP_OBSERVE_ESTABLISH, nullptr))
      return;
    send();
  }

  bool add_option(uint16_t number, size_t length, const uint8_t* data)
  {
    if(!coap_insert_optlist(&optlist, coap_new_optlist(number, length, data)))
    {
      return false;
    }
    return true;
  }

  void post(const coap_destination& dest, std::string_view fmt, std::string_view bytes)
  {
    if(!init_message(
           dest, is_mcast ? COAP_MESSAGE_NON : COAP_MESSAGE_CON, COAP_REQUEST_CODE_POST))
      return;

    add_option(COAP_OPTION_CONTENT_TYPE, fmt.length(), (const uint8_t*)fmt.data());
    coap_add_data(pdu, bytes.size(), (const uint8_t*)bytes.data());
    send();
  }

  void send()
  {
    if(optlist)
    {
      int res = coap_add_optlist_pdu(pdu, &optlist);
      if(res != 1)
      {
        coap_log_warn("Failed to add options to PDU\n");
        return;
      }
    }

    coap_send(session, pdu);
  }

  ~coap_session()
  {
    coap_delete_optlist(optlist);
    coap_session_release(session);
  }
};

coap_client::coap_client()
{
  coap_set_log_level(COAP_LOG_EMERG);

  if(!(ctx = coap_new_context(nullptr)))
    throw std::runtime_error("Cannot create libcoap context");

  coap_context_set_block_mode(ctx, COAP_BLOCK_USE_LIBCOAP | COAP_BLOCK_SINGLE_BODY);

  coap_register_response_handler(
      ctx, [](coap_session_t* session, const coap_pdu_t* sent,
              const coap_pdu_t* received, const coap_mid_t mid) {
    try
    {
      auto sesh = (coap_session*)coap_session_get_app_data(session);
      const auto action = sesh->on_reply(received, mid);
      if(action == DeleteSession)
      {
        sesh->client.in_flight.erase(sesh->session);
      }
      return COAP_RESPONSE_OK;
    }
    catch(...)
    {
      return COAP_RESPONSE_FAIL;
    }
  });
}

std::shared_ptr<coap_session>
coap_client::get(std::string_view req, coap_reply_callback rep)
{
  auto dest = coap_destination::parse(req.data());
  if(!dest)
    return {};
  auto s = std::make_shared<coap_session>(*this, *dest, rep);
  s->get(*dest);
  this->in_flight.emplace(s->session, s);
  return s;
}

std::shared_ptr<coap_session>
coap_client::subscribe(std::string_view req, coap_reply_callback rep)
{
  auto dest = coap_destination::parse(req.data());
  if(!dest)
    return {};
  auto s = std::make_shared<coap_session>(*this, *dest, rep);
  s->subscribe(*dest);
  this->in_flight.emplace(s->session, s);
  return s;
}

void coap_client::unsubscribe(const coap_session& dest)
{
  in_flight.erase(dest.session);
}

std::shared_ptr<coap_session>
coap_client::post(std::string_view req, std::string_view fmt, std::string_view bytes)
{
  auto dest = coap_destination::parse(req.data());
  if(!dest)
    return {};

  auto s = std::make_shared<coap_session>(*this, *dest);
  s->post(*dest, fmt, bytes);
  this->in_flight.emplace(s->session, s);

  return {};
}

coap_client_protocol::coap_client_protocol(
    network_context_ptr ctx, const coap_client_configuration& conf)
    : m_context{ctx}
    , m_conf{conf}
    , m_strand{boost::asio::make_strand(m_context->context)}
    , m_timer{m_strand, m_context->context}
{
  m_client = std::make_unique<coap_client>();
  struct
  {
    coap_client_protocol& self;

    void operator()(const ossia::net::udp_configuration& conf)
    {
      self.m_host += "coap://";
      self.m_host += conf.remote->host;
      if(conf.remote->port != 0)
      {
        self.m_host += ':';
        self.m_host += std::to_string(conf.remote->port);
      }
    }

    void operator()(const ossia::net::tcp_client_configuration& conf)
    {
      self.m_host += "coap+tcp://";
      self.m_host += conf.host;
      if(conf.port != 0)
      {
        self.m_host += ':';
        self.m_host += std::to_string(conf.port);
      }
    }

    void operator()(const ossia::net::ws_client_configuration& conf)
    {
      if(conf.url.starts_with("ws://"))
        self.m_host = "coap+" + conf.url;
      else
        self.m_host = "coap+ws://" + conf.url;
    }
  } setup{*this};
  ossia::visit(setup, conf.transport);
}

coap_client_protocol::~coap_client_protocol() { }

bool coap_client_protocol::pull(ossia::net::parameter_base& param)
{
  auto req = m_client->get(
      param.get_node().osc_address(),
      [&param](const coap_pdu_t* received, coap_mid_t mid) -> coap_reply_action {
    size_t len;
    const uint8_t* databuf;
    size_t offset;
    size_t total;

    if(coap_get_data_large(received, &len, &databuf, &offset, &total))
    {
      param.set_value(std::string((const char*)databuf, len));
    }

    return coap_reply_action::DeleteSession;
  });

  return false;
}

bool coap_client_protocol::push(
    const ossia::net::parameter_base& p, const ossia::value& v)
{
  auto req = m_client->post(
      m_host + p.get_node().osc_address(), "text/plain", ossia::convert<std::string>(v));
  return false;
}

bool coap_client_protocol::push_raw(const ossia::net::full_parameter_data&)
{
  return false;
}

bool coap_client_protocol::observe(ossia::net::parameter_base& param, bool obs)
{
  const auto& addr = param.get_node().osc_address();
  if(obs)
  {
    if(m_topics.contains(addr))
      return false;

    auto req = m_client->subscribe(
        m_host + param.get_node().osc_address(),
        [&param](const coap_pdu_t* received, coap_mid_t mid) -> coap_reply_action {
      size_t len;
      const uint8_t* databuf;
      size_t offset;
      size_t total;

      if(coap_get_data_large(received, &len, &databuf, &offset, &total))
      {
        param.set_value(std::string((const char*)databuf, len));
      }

      return coap_reply_action::None;
    });

    m_topics.emplace(param.get_node().osc_address(), req);
  }
  else
  {
    if(!m_topics.contains(addr))
      return false;
    m_topics.visit(addr, [this](const auto& sesh) {
      if(sesh.second)
        m_client->unsubscribe(*sesh.second);
    });
  }
  return true;
}

void coap_client_protocol::parse_namespace(
    ossia::net::node_base& parent, std::string_view data)
{
  if(auto res = ossia::coap::parse_link_format(data))
  {
    for(const auto& [name, opts] : res->resources)
    {
      // If answer is in link format, make a new request
      if(auto it = opts.find("ct"); it != opts.end())
      {
        if(auto val = get_if<int64_t>(&it->second); val && *val == 40)
        {
          // Note : CoAP always gives absolute paths, e.g. given:
          // GET coap://coap.me/location1
          // =>
          // </location1/location2>;...

          ossia::net::create_node(parent, name);
          request_namespace(parent, m_host + name);
          continue;
        }
      }

      ossia::create_parameter(parent, name, "string");
    }
  }
}

void coap_client_protocol::request_namespace(
    ossia::net::node_base& node_base, std::string_view r)
{
  auto req = m_client->get(
      r,
      [this,
       &node_base](const coap_pdu_t* received, coap_mid_t mid) -> coap_reply_action {
    size_t len;
    const uint8_t* databuf;
    size_t offset;
    size_t total;

    if(coap_get_data_large(received, &len, &databuf, &offset, &total))
    {
      parse_namespace(node_base, std::string_view((const char*)databuf, len));
    }

    return coap_reply_action::DeleteSession;
  });
}

bool coap_client_protocol::update(ossia::net::node_base& node_base)
{
  request_namespace(node_base, m_host + "/.well-known/core");
  return true;
}

void coap_client_protocol::set_device(device_base& dev)
{
  using namespace std::chrono;
  m_device = &dev;

  m_timer.set_delay(5ms);
  m_timer.start([this] { m_client->poll(); });
}

void coap_client_protocol::stop()
{
  m_timer.stop();
  std::future<void> wait = boost::asio::dispatch(
      boost::asio::bind_executor(m_strand, std::packaged_task<void()>([this] {
    m_topics.clear();
    m_client.reset();
  })));
  wait.get();
}
}
