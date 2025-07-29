#pragma once

#include <ossia/detail/timer.hpp>
#include <ossia/network/base/bundle.hpp>
#include <ossia/network/base/parameter.hpp>
#include <ossia/network/base/protocol.hpp>
#include <ossia/network/context.hpp>
#include <ossia/network/osc/detail/osc_packet_processor.hpp>
#include <ossia/network/osc/detail/osc_receive.hpp>
#include <ossia/network/sockets/configuration.hpp>
#include <ossia/network/sockets/null_socket.hpp>
#include <ossia/network/sockets/writers.hpp>
#include <ossia/protocols/dense/dense_protocol_configuration.hpp>

#include <oscpack/osc/OscReceivedElements.h>

#include <span>

namespace ossia::net
{
struct dense_writer
{
  unsigned char* it{};
  unsigned char* end{};
  int available() const noexcept { return end - it; }

  bool operator()() { return false; }
  bool operator()(ossia::impulse) { return true; }
  bool operator()(uint16_t v) noexcept
  {
    if(available() >= 2)
    {
      memcpy(it, &v, sizeof(v));
      it += 2;
      return true;
    }
    return false;
  }
  bool operator()(int32_t v) noexcept
  {
    if(available() >= 4)
    {
      memcpy(it, &v, sizeof(v));
      it += 4;
      return true;
    }
    return false;
  }
  bool operator()(bool vv) noexcept
  {
    int32_t v = vv ? 1 : 0;
    if(available() >= 4)
    {
      memcpy(it, &v, sizeof(v));
      it += 4;
      return true;
    }
    return false;
  }
  bool operator()(float v) noexcept
  {
    if(available() >= 4)
    {
      memcpy(it, &v, sizeof(v));
      it += 4;
      return true;
    }
    return false;
  }
  bool operator()(std::string_view v) noexcept
  {
    if(available() >= (4 + v.size()))
    {
      (*this)((int)std::ssize(v));
      memcpy(it, v.data(), v.size());
      it += v.size();
      return true;
    }
    return false;
  }
  template <std::size_t N>
  bool operator()(std::array<float, N> v) noexcept
  {
    if(available() >= 4 * N)
    {
      for(int i = 0; i < N; i++)
        (*this)(v[N]);
      return true;
    }
    return false;
  }
  bool operator()(const std::vector<ossia::value>& v) noexcept
  {
    if(available() >= 4 * v.size() + 4) // FIXME
    {
      (*this)((uint16_t)0);
      (*this)((uint16_t)v.size());
      bool ok = true;
      for(int i = 0; i < v.size(); i++)
      {
        ok &= v[i].apply(*this);
        if(!ok)
          return false;
      }
      return true;
    }
    return false;
  }
  bool operator()(const ossia::value_map_type& v) noexcept
  {
    if(available() >= 4 * v.size() + 4) // FIXME
    {
      (*this)((uint16_t)0);
      (*this)((uint16_t)v.size());
      bool ok = true;
      for(auto& [k, val] : v)
      {
        ok &= val.apply(*this);
        if(!ok)
          return false;
      }
      return true;
    }
    return false;
  }
};

template <typename SendSocket, typename RecvSocket>
class dense_generic_bidir_protocol : public ossia::net::protocol_base
{
public:
  // using socket_type = Socket;
  using writer_type = socket_writer<SendSocket>;

  dense_generic_bidir_protocol(
      network_context_ptr ctx, const dense_configuration& conf,
      const send_fd_configuration& send_conf, const receive_fd_configuration& recv_conf)
      : ossia::net::protocol_base{flags{SupportsMultiplex}}
      , m_ctx{std::move(ctx)}
      , m_id{*this}
      , from_client{recv_conf, m_ctx->context}
      , to_client{send_conf, m_ctx->context}
  {
    from_client.open();
    to_client.connect();

    init_timer(conf.interval);
  }

  dense_generic_bidir_protocol(
      network_context_ptr ctx, const dense_configuration& conf,
      const outbound_socket_configuration& send_conf,
      const inbound_socket_configuration& recv_conf)
      : ossia::net::protocol_base{flags{SupportsMultiplex}}
      , m_ctx{std::move(ctx)}
      , m_id{*this}
      , from_client{recv_conf, m_ctx->context}
      , to_client{send_conf, m_ctx->context}
  {
    from_client.open();
    to_client.connect();
    this->receive();
    init_timer(conf.interval);
  }

  dense_generic_bidir_protocol(
      network_context_ptr ctx, const dense_configuration& conf,
      const outbound_socket_configuration& send_conf)
      : ossia::net::protocol_base{flags{SupportsMultiplex}}
      , m_ctx{std::move(ctx)}
      , m_id{*this}
      , to_client{send_conf, m_ctx->context}
      , m_timer{m_ctx->context}
  {
    to_client.connect();
    init_timer(conf.interval);
  }

  dense_generic_bidir_protocol(
      network_context_ptr ctx, const dense_configuration& conf,
      const inbound_socket_configuration& recv_conf)
      : ossia::net::protocol_base{flags{SupportsMultiplex}}
      , m_ctx{std::move(ctx)}
      , m_id{*this}
      , from_client{recv_conf, m_ctx->context}
  {
    from_client.open();
    this->receive();
    init_timer(conf.interval);
  }

  dense_generic_bidir_protocol(
      network_context_ptr ctx, const dense_configuration& conf,
      const send_fd_configuration& send_conf)
      : ossia::net::protocol_base{flags{SupportsMultiplex}}
      , m_ctx{std::move(ctx)}
      , m_id{*this}
      , to_client{send_conf, m_ctx->context}
  {
    to_client.connect();
    init_timer(conf.interval);
  }

  dense_generic_bidir_protocol(
      network_context_ptr ctx, const dense_configuration& conf,
      const receive_fd_configuration& recv_conf)
      : ossia::net::protocol_base{flags{SupportsMultiplex}}
      , m_ctx{std::move(ctx)}
      , m_id{*this}
      , from_client{recv_conf, m_ctx->context}
  {
    from_client.open();
    this->receive();
    init_timer(conf.interval);
  }

  dense_generic_bidir_protocol(const dense_generic_bidir_protocol&) = delete;
  dense_generic_bidir_protocol(dense_generic_bidir_protocol&&) = delete;
  dense_generic_bidir_protocol& operator=(const dense_generic_bidir_protocol&) = delete;
  dense_generic_bidir_protocol& operator=(dense_generic_bidir_protocol&&) = delete;

  ~dense_generic_bidir_protocol() override = default;

  void init_timer(std::chrono::microseconds us) { m_timer.set_delay(us); }

  void receive()
  {
    from_client.receive([this](const char* data, std::size_t sz) {
      if(!m_device)
        return;
      auto on_message = [this](auto&& msg) {
        ossia::net::on_input_message<false>(
            msg.AddressPattern(), ossia::net::osc_message_applier{m_id, msg}, false,
            *m_device, m_logger);
      };
      osc_packet_processor<decltype(on_message)>{on_message}(data, sz);
    });
  }

  bool update(ossia::net::node_base& node_base) override { return false; }

  bool pull(ossia::net::parameter_base& parameter_base) override { return false; }

  bool observe(ossia::net::parameter_base& parameter_base, bool enable) override
  {
    return false;
  }

  bool echo_incoming_message(
      const message_origin_identifier& id, const parameter_base& addr,
      const value& val) override
  {
    // FIXME
    return false;
  }

  bool push(const ossia::net::parameter_base& addr, const ossia::value& v) override
  {
    // Store locally
    return false;
  }

  bool push(const ossia::net::parameter_base& addr, ossia::value&& v) override
  {
    // Store locally
    return false;
  }

  bool push_raw(const ossia::net::full_parameter_data& addr) override
  {
    // Store locally
    return false;
  }

  bool push_bundle(const std::vector<const parameter_base*>& addresses) override
  {
    // Store locally
    return false;
  }

  bool push_bundle(std::span<ossia::bundle_element> addresses) override
  {
    // Store locally
    return false;
  }

  bool push_bundle_bounded(std::span<ossia::bundle_element> addresses) override
  {
    // Store locally
    return false;
  }

  bool
  push_raw_bundle(const std::vector<ossia::net::full_parameter_data>& addresses) override
  {
    // Store locally
    return false;
  }

  void set_device(ossia::net::device_base& dev) override
  {
    m_device = &dev;
    m_timer.start([this] { this->update_function(); });
  }

  auto writer() noexcept { return writer_type{to_client}; }

  void update_function()
  {
    recompute_nodes();

    try
    {
      dense_packet pkt;
      pkt.header.protocol_code = 0xCAFECAFE;

      int n = write_packet(pkt.data);

      to_client.write(reinterpret_cast<const char*>(&pkt), sizeof(pkt.header) + n);
    }
    catch(std::exception& e)
    {
      ossia::logger().error("write failure: {}", e.what());
    }
    catch(...)
    {
      ossia::logger().error("write failure");
    }
  }

  void recompute_nodes()
  {
    m_params.clear();
    m_params.reserve(200);

    if(!this->m_device)
      return;
    auto& dev = this->m_device->get_root_node();
    ossia::net::iterate_all_children(
        &dev, [this](ossia::net::parameter_base& p) { m_params.insert(&p); });

    // FIXME hash: name + type
    // for(auto& p : m_params)
    // {
    // }
  }

  int write_packet(std::span<unsigned char> data)
  {
    dense_writer wr{data.data(), data.data() + data.size()};
    for(auto& param : m_params)
    {
      param->value().apply(wr);
    }
    return wr.it - data.data();
  }

  using ossia::net::protocol_base::m_logger;
  ossia::net::network_context_ptr m_ctx;
  message_origin_identifier m_id;

  ossia::net::device_base* m_device{};

  RecvSocket from_client;
  SendSocket to_client;

  ossia::timer m_timer;

  ossia::flat_set<ossia::net::parameter_base*, parameter_alphabetical_sort> m_params;
};
}
