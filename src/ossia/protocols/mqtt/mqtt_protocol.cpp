#include "mqtt_protocol.hpp"

#include <ossia/network/base/device.hpp>

#include <boost/asio/as_tuple.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/use_future.hpp>
#include <boost/beast.hpp>

// clang-format off
#include <async_mqtt5.hpp>
#include <async_mqtt5/websocket.hpp>
// clang-format on

#include <coroutine>

namespace ossia::net
{
using topics_type
    = boost::unordered::concurrent_flat_map<std::string, ossia::net::parameter_base*>;
using strand_type
    = decltype(boost::asio::make_strand(std::declval<boost::asio::io_context&>()));

static constexpr auto use_nothrow_awaitable
    = boost::asio::as_tuple(boost::asio::use_awaitable);
struct mqtt5_client_base
{
  virtual ~mqtt5_client_base() = default;
  virtual void connect(const ossia::net::mqtt5_configuration& m_conf) = 0;
  virtual boost::asio::awaitable<bool> publish(ossia::net::parameter_base& param) = 0;
  virtual boost::asio::awaitable<bool> subscribe(std::string topic) = 0;
  virtual boost::asio::awaitable<bool> unsubscribe(std::string topic) = 0;
  virtual boost::asio::awaitable<void> receive(mqtt5_protocol& self) = 0;
  virtual boost::asio::awaitable<bool> learn(mqtt5_protocol& self, bool) = 0;
};

using mqtt_tcp_socket = boost::asio::ip::tcp::socket;
using mqtt_ws_socket = boost::beast::websocket::stream<boost::asio::ip::tcp::socket>;
template <typename Socket = boost::asio::ip::tcp::socket>
struct mqtt5_client : mqtt5_client_base
{
  async_mqtt5::mqtt_client<Socket> client;
  explicit mqtt5_client(strand_type& strand)
      : client{strand}
  {
  }

  void connect(const ossia::net::mqtt5_configuration& m_conf) override
  {
    if constexpr(std::is_same_v<Socket, mqtt_tcp_socket>)
    {
      auto& conf = ossia::get<tcp_client_configuration>(m_conf.transport);
      client.brokers(conf.host, conf.port).async_run(boost::asio::detached);
    }
    else if constexpr(std::is_same_v<Socket, mqtt_ws_socket>)
    {
      auto& conf = ossia::get<ws_client_configuration>(m_conf.transport);
      std::string host = conf.url;
      uint16_t default_port = 1883;
      client.brokers(host, default_port).async_run(boost::asio::detached);
    }
    else
    {
      static_assert(std::is_same_v<Socket*, void>);
    }
  }

  boost::asio::awaitable<bool> publish(ossia::net::parameter_base& param) override
  {
    auto&& [ec, rc, props]
        = co_await client.template async_publish<async_mqtt5::qos_e::at_least_once>(
            param.get_node().osc_address(), ossia::convert<std::string>(param.value()),
            async_mqtt5::retain_e::no, async_mqtt5::publish_props{},
            use_nothrow_awaitable);

    co_return ec || !rc;
  }

  boost::asio::awaitable<bool> subscribe(std::string topic) override
  {
    // Configure the request to subscribe to a Topic.
    async_mqtt5::subscribe_topic sub_topic = async_mqtt5::subscribe_topic{
        topic, async_mqtt5::subscribe_options{
                   async_mqtt5::qos_e::exactly_once, async_mqtt5::no_local_e::no,
                   async_mqtt5::retain_as_published_e::retain,
                   async_mqtt5::retain_handling_e::send}};

    auto&& [ec, sub_codes, sub_props] = co_await client.async_subscribe(
        sub_topic, async_mqtt5::subscribe_props{}, use_nothrow_awaitable);

    co_return !ec && !sub_codes[0];
  }
  boost::asio::awaitable<bool> unsubscribe(std::string topic) override
  {
    auto&& [ec, sub_codes, sub_props] = co_await client.async_unsubscribe(
        topic, async_mqtt5::unsubscribe_props{}, use_nothrow_awaitable);

    co_return !ec && !sub_codes[0];
  }

  static void dump_props(const auto& publish_props)
  {
    publish_props.visit([]<typename Key>(const Key& x, const auto& v) -> bool {
      if constexpr(requires { v.has_value(); })
      {
        if(v.has_value())
          std::cerr << "prop " << (int)Key::value << " : " << *v << "\n";
        else
          std::cerr << "prop " << (int)Key::value << " (unset) \n";
      }
      else if constexpr(requires { v.size(); })
      {
        std::cerr << "arr prop (" << v.size() << "): [ ";
        for(size_t i = 0; i < v.size(); i++)
        {
          if constexpr(requires {
                         v[i].first;
                         v[i].second;
                       })
            std::cerr << "(" << v[i].first << ", " << v[i].second << ")";
          else
            std::cerr << v[i];
          if(i + 1 < v.size())
            std::cerr << ", ";
        }
        std::cerr << "]\n";
      }
      return true;
    });
  }

  boost::asio::awaitable<void> receive(mqtt5_protocol& self) override
  {
    // receive
    for(;;)
    {
      auto&& [ec, topic, payload, publish_props]
          = co_await client.async_receive(use_nothrow_awaitable);

      if(ec == async_mqtt5::client::error::session_expired)
      {
        if(co_await subscribe(topic))
          continue;
        else
          break;
      }
      else if(ec)
        break;

      self.on_message(topic, payload);
    }

    co_return;
  }

  boost::asio::awaitable<bool> learn(mqtt5_protocol& self, bool v) override
  {
    return v ? subscribe("#") : unsubscribe("#");
  }
};

struct mqtt5_protocol::subscribe_state
{
  boost::asio::cancellation_signal cancellation;
  enum
  {
    created,
    operating,
    cancelled
  } state{created};
};

static auto make_mqtt_client(const mqtt5_configuration& conf, strand_type& strand)
{
  struct
  {
    strand_type& strand;
    std::unique_ptr<mqtt5_client_base> operator()(const tcp_client_configuration& tcp)
    {
      return std::make_unique<mqtt5_client<mqtt_tcp_socket>>(strand);
    }
    std::unique_ptr<mqtt5_client_base> operator()(const ws_client_configuration& tcp)
    {
      return std::make_unique<mqtt5_client<mqtt_ws_socket>>(strand);
    }
  } vis{strand};

  return visit(vis, conf.transport);
}

mqtt5_protocol::mqtt5_protocol(network_context_ptr ctx, const mqtt5_configuration& conf)
    : m_context{ctx}
    , m_conf{conf}
    , m_strand{boost::asio::make_strand(m_context->context)}
{
  m_client = make_mqtt_client(conf, m_strand);
}

mqtt5_protocol::~mqtt5_protocol() { }

bool mqtt5_protocol::pull(ossia::net::parameter_base&)
{
  return false;
}

bool mqtt5_protocol::push(const ossia::net::parameter_base& p, const ossia::value& v)
{
  co_spawn(
      m_strand, m_client->publish(const_cast<ossia::net::parameter_base&>(p)),
      boost::asio::detached);
  return false;
}

bool mqtt5_protocol::push_raw(const ossia::net::full_parameter_data&)
{
  return false;
}

bool mqtt5_protocol::observe(ossia::net::parameter_base&, bool)
{
  return false;
}

void mqtt5_protocol::on_new_param(const ossia::net::parameter_base& param)
{
  // FIXME we should create a "safe" data object copied to the thread there
  boost::asio::post(boost::asio::bind_executor(
      m_strand, [&param = const_cast<ossia::net::parameter_base&>(param), this]() {
    if(!m_client)
      return;
    switch(param.get_access())
    {
      case ossia::access_mode::GET:
        on_subscribe(param);
        break;
      case ossia::access_mode::SET:
        // Nothing to do. publishing is just "push".
        break;
      case ossia::access_mode::BI:
        // does not make sense with MQTT...
        // let's try to subscribe anyways as most likely we want to read some sensor
        on_subscribe(param);
        break;
    }
  }));
}

void mqtt5_protocol::on_removed_param(const ossia::net::parameter_base& p)
{
  if(!m_client)
    return;
  on_unsubscribe(p);
}

void mqtt5_protocol::on_subscribe(ossia::net::parameter_base& p)
{
  m_topics.try_emplace(p.get_node().osc_address(), &p);
  co_spawn(
      m_strand, m_client->subscribe(p.get_node().osc_address()), boost::asio::detached);
}

void mqtt5_protocol::on_unsubscribe(const ossia::net::parameter_base& p)
{
  m_topics.erase(p.get_node().osc_address());
}

bool mqtt5_protocol::update(ossia::net::node_base& node_base)
{
  return false;
}

void mqtt5_protocol::set_device(device_base& dev)
{
  m_device = &dev;
  dev.on_parameter_created.connect<&mqtt5_protocol::on_new_param>(*this);
  dev.on_parameter_removing.connect<&mqtt5_protocol::on_removed_param>(*this);

  // Connect to the broker
  m_client->connect(m_conf);

  // Publish all existing nodes
  ossia::net::iterate_all_children(
      &dev.get_root_node(),
      [this](ossia::net::parameter_base& param) { on_new_param(param); });

  m_root = std::make_shared<subscribe_state>();
  co_spawn(
      m_strand, m_client->receive(*this),
      boost::asio::bind_cancellation_slot(
          m_root->cancellation.slot(), boost::asio::detached));
}

void mqtt5_protocol::set_learning(bool v)
{
  can_learn::set_learning(v);
  co_spawn(m_strand, m_client->learn(*this, v), boost::asio::detached);
}

void mqtt5_protocol::on_message(const std::string& topic, const std::string& payload)
{
  if(learning())
  {
    [[unlikely]];
    on_learn(topic, payload);
    return;
  }

  m_topics.visit(topic, [&payload = payload](auto& v) {
    auto& param = *v.second;
    param.set_value(ossia::convert(payload, param.get_value_type()));
  });
}

void mqtt5_protocol::on_learn(const std::string& topic, const std::string& payload)
{
  assert(this->m_device);
  auto* n = &this->m_device->get_root_node();
  std::vector<std::string> v = address_parts(topic);
  bool is_new = false;
  for(const auto& part : v)
  {
    auto cld = n->find_child(part);
    if(cld)
    {
      n = cld;
    }
    else
    {
      // Start adding
      n = n->create_child(part);
      is_new = true;
    }
  }

  if(!is_new)
  {
    return;
  }
  auto addr = n->create_parameter(ossia::val_type::STRING);
  addr->set_value(payload);
}

void mqtt5_protocol::stop()
{
  assert(m_device);
  m_device->on_parameter_created.disconnect<&mqtt5_protocol::on_new_param>(*this);
  m_device->on_parameter_removing.disconnect<&mqtt5_protocol::on_removed_param>(*this);

  std::future<void> wait = boost::asio::dispatch(
      boost::asio::bind_executor(m_strand, std::packaged_task<void()>([this] {
    m_client.reset();

    m_root->cancellation.emit(boost::asio::cancellation_type::all);
  })));
  wait.get();

  std::future<void> wait2 = boost::asio::dispatch(m_strand, boost::asio::use_future);
  wait2.get();
}
}
