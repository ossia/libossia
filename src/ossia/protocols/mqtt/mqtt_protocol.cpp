#include "mqtt_protocol.hpp"

#include <ossia/network/base/device.hpp>

#include <boost/asio/as_tuple.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/use_awaitable.hpp>

#include <async_mqtt5.hpp>

#include <coroutine>

namespace ossia::net
{
static constexpr auto use_nothrow_awaitable
    = boost::asio::as_tuple(boost::asio::use_awaitable);
struct mqtt5_client
{
  async_mqtt5::mqtt_client<boost::asio::ip::tcp::socket> client;
  explicit mqtt5_client(boost::asio::io_context& ioc)
      : client{ioc}
  {
  }

  boost::asio::awaitable<bool> publish(ossia::net::parameter_base& param)
  {
    auto&& [ec, rc, props]
        = co_await client.async_publish<async_mqtt5::qos_e::at_least_once>(
            param.get_node().osc_address(), ossia::convert<std::string>(param.value()),
            async_mqtt5::retain_e::no, async_mqtt5::publish_props{},
            use_nothrow_awaitable);

    co_return ec || !rc;
  }

  boost::asio::awaitable<bool> subscribe(const std::string& topic)
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

  boost::asio::awaitable<void> subscribe_and_receive(ossia::net::parameter_base& param)
  {
    // subscribe
    if(!(co_await subscribe(param.get_node().osc_address())))
      co_return;

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

      param.push_value(ossia::convert(payload, param.get_value_type()));
    }

    co_return;
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

mqtt5_protocol::mqtt5_protocol(network_context_ptr ctx, const mqtt5_configuration& conf)
    : m_context{ctx}
    , m_conf{conf}
    , m_client{std::make_unique<mqtt5_client>(m_context->context)}
{
}

mqtt5_protocol::~mqtt5_protocol() { }

bool mqtt5_protocol::pull(ossia::net::parameter_base&)
{
  return false;
}

bool mqtt5_protocol::push(const ossia::net::parameter_base& p, const ossia::value& v)
{
  co_spawn(
      m_context->context, m_client->publish(const_cast<ossia::net::parameter_base&>(p)),
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
  boost::asio::post([&param, this] {
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
  });
}

void mqtt5_protocol::on_removed_param(const ossia::net::parameter_base& p)
{
  on_unsubscribe(p);
}

void mqtt5_protocol::on_subscribe(const ossia::net::parameter_base& p)
{
  m_subscriptions.visit(&p, [this, &p](auto& res) {
    subscribe_state& sub = res.second;

    if(std::exchange(sub.state, sub.operating) == sub.operating)
      return;

    co_spawn(
        m_context->context,
        m_client->subscribe_and_receive(const_cast<ossia::net::parameter_base&>(p)),
        boost::asio::bind_cancellation_slot(
            sub.cancellation.slot(), boost::asio::detached));
  });
}

void mqtt5_protocol::on_unsubscribe(const ossia::net::parameter_base& p)
{
  m_subscriptions.visit(&p, [this, &p](auto& res) {
    subscribe_state& sub = res.second;
    if(std::exchange(sub.state, sub.cancelled) == sub.operating)
      sub.cancellation.emit(boost::asio::cancellation_type::all);
  });
}

bool mqtt5_protocol::update(ossia::net::node_base& node_base)
{
  return false;
}

void mqtt5_protocol::set_device(device_base& dev)
{
  dev.on_parameter_created.connect<&mqtt5_protocol::on_new_param>(*this);
  dev.on_parameter_removing.connect<&mqtt5_protocol::on_removed_param>(*this);

  // Connect to the broker
  m_client->client.brokers(m_conf.host, m_conf.port).async_run(boost::asio::detached);

  // Publish all existing nodes
  ossia::net::iterate_all_children(
      &dev.get_root_node(),
      [this](ossia::net::parameter_base& param) { on_new_param(param); });
}
}
