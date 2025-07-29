#pragma once
#include <ossia/network/base/parameter.hpp>
#include <ossia/network/osc/detail/bundle.hpp>
#include <ossia/network/osc/detail/message_generator.hpp>
#include <ossia/network/osc/detail/osc_1_0_policy.hpp>
#include <ossia/network/osc/detail/osc_packet_processor.hpp>
#include <ossia/network/osc/detail/osc_receive.hpp>
#include <ossia/network/osc/detail/osc_value_write_visitor.hpp>
#include <ossia/network/value/format_value.hpp>

namespace ossia::net
{

template <typename OscVersion>
struct osc_protocol_common
{
  using osc_configuration = OscVersion;
  template <typename T, typename Value_T>
  static bool push(T& self, const ossia::net::parameter_base& addr, Value_T&& v)
  {
    auto val = bound_value(addr, std::forward<Value_T>(v));
    if(val.valid())
    {
      using send_visitor = osc_value_send_visitor<
          ossia::net::parameter_base, OscVersion, typename T::writer_type>;

      send_visitor vis{addr, addr.get_node().osc_address(), self.writer()};
      val.apply(vis);

      if(const auto& logger = self.m_logger.outbound_logger)
      {
        logger->info("[push] {} {}", addr.get_node().osc_address(), val);
      }
      return true;
    }
    return false;
  }

  template <typename T>
  static bool push_raw(T& self, const ossia::net::full_parameter_data& addr)
  {
    auto val = bound_value(addr, addr.value());
    if(val.valid())
    {
      using send_visitor = osc_value_send_visitor<
          ossia::net::full_parameter_data, OscVersion, typename T::writer_type>;
      val.apply(send_visitor{addr, addr.address, self.writer()});

      if(const auto& logger = self.m_logger.outbound_logger)
      {
        logger->info("[push_raw] {} {}", addr.address, val);
      }
      return true;
    }
    return false;
  }

  template <typename T>
  static bool echo_incoming_message(
      T& self, const message_origin_identifier& id, const parameter_base& addr,
      const value& val)
  {
    if(&id.protocol == &self)
      return true;

    using send_visitor = osc_value_send_visitor<
        ossia::net::parameter_base, OscVersion, typename T::writer_type>;
    val.apply(send_visitor{addr, addr.get_node().osc_address(), self.writer()});

    if(const auto& logger = self.m_logger.outbound_logger)
    {
      logger->info("[echo] {} {}", addr.get_node().osc_address(), val);
    }
    return true;
  }

  template <typename T>
  static bool observe(T& self, ossia::net::parameter_base& address, bool enable)
  {
    if(enable)
      self.m_listening.insert(
          std::make_pair(address.get_node().osc_address(), &address));
    else
      self.m_listening.erase(address.get_node().osc_address());

    return true;
  }

  template <typename T>
  static void on_received_message(T& self, const oscpack::ReceivedMessage& m)
  {
    [[unlikely]] if(self.learning())
    {
      auto already_learned = ossia::net::osc_learn(&self.m_device->get_root_node(), m);
      if(!already_learned)
        return;
    }

    ossia::net::on_input_message<false>(
        m.AddressPattern(), ossia::net::osc_message_applier{self.m_id, m},
        self.m_listening, *self.m_device, self.m_logger);
  }
};

// Client can't push to GET addresses
template <typename OscVersion>
struct osc_protocol_client : osc_protocol_common<OscVersion>
{
  template <typename T, typename Val_T>
  static bool push(T& self, const ossia::net::parameter_base& addr, Val_T&& v)
  {
    if(addr.get_access() == ossia::access_mode::GET)
      return false;

    return osc_protocol_common<OscVersion>::push(self, addr, std::forward<Val_T>(v));
  }

  template <typename T>
  static bool push_raw(T& self, const ossia::net::full_parameter_data& addr)
  {
    if(addr.get_access() == ossia::access_mode::GET)
      return false;

    return osc_protocol_common<OscVersion>::push_raw(self, addr);
  }

  template <typename T, typename Writer>
  static bool push_bundle(
      T& self, Writer writer, const ossia::net::parameter_base& addr, ossia::value v)
  {
    if(auto bundle = make_bundle(bundle_client_policy<OscVersion>{}, addr, v))
    {
      writer(bundle->data.data(), bundle->data.size());
      ossia::buffer_pool::instance().release(std::move(bundle->data));
      return true;
    }
    return false;
  }

  template <typename T, typename Writer>
  static bool
  push_bundle(T& self, Writer writer, const ossia::net::full_parameter_data& addr)
  {
    if(auto bundle = make_bundle(bundle_client_policy<OscVersion>{}, addr))
    {
      writer(bundle->data.data(), bundle->data.size());
      ossia::buffer_pool::instance().release(std::move(bundle->data));
      return true;
    }
    return false;
  }

  template <typename T, typename Writer, typename Addresses>
  static bool push_bundle(T& self, Writer writer, const Addresses& addresses)
  {
    if(auto bundle = make_bundle(bundle_client_policy<OscVersion>{}, addresses))
    {
      writer(bundle->data.data(), bundle->data.size());
      ossia::buffer_pool::instance().release(std::move(bundle->data));
      return true;
    }
    return false;
  }

  template <typename T, typename Writer, typename Addresses>
  static bool push_bundle_bounded(T& self, Writer writer, const Addresses& addresses)
  {
    return make_bundle_bounded(
        bundle_bounded_client_policy<OscVersion>{}, addresses,
        [writer](const ossia::net::bundle& bundle) {
      writer(bundle.data.data(), bundle.data.size());
        });
  }
};

// Servers can push to GET addresses
template <typename OscVersion>
struct osc_protocol_server : osc_protocol_common<OscVersion>
{
  template <typename T, typename Val_T>
  static bool push(T& self, const ossia::net::parameter_base& addr, Val_T&& v)
  {
    if(addr.get_access() == ossia::access_mode::SET)
      return false;

    return osc_protocol_common<OscVersion>::push(self, addr, std::forward<Val_T>(v));
  }

  template <typename T>
  static bool push_raw(T& self, const ossia::net::full_parameter_data& addr)
  {
    if(addr.get_access() == ossia::access_mode::SET)
      return false;

    return osc_protocol_common<OscVersion>::push_raw(self, addr);
  }

  template <typename T, typename Writer>
  static bool
  push_bundle(T& self, Writer writer, const ossia::net::full_parameter_data& addr)
  {
    if(auto bundle = make_bundle(bundle_server_policy<OscVersion>{}, addr))
    {
      writer(bundle->data.data(), bundle->data.size());
      ossia::buffer_pool::instance().release(std::move(bundle->data));
      return true;
    }
    return false;
  }

  template <typename T, typename Writer>
  static bool push_bundle(
      T& self, Writer writer, const ossia::net::parameter_base& addr, ossia::value v)
  {
    if(auto bundle = make_bundle(bundle_server_policy<OscVersion>{}, addr, v))
    {
      writer(bundle->data.data(), bundle->data.size());
      ossia::buffer_pool::instance().release(std::move(bundle->data));
      return true;
    }
    return false;
  }

  template <typename T, typename Writer, typename Addresses>
  static bool push_bundle(T& self, Writer writer, const Addresses& addresses)
  {
    if(auto bundle = make_bundle(bundle_server_policy<OscVersion>{}, addresses))
    {
      writer(bundle->data.data(), bundle->data.size());
      ossia::buffer_pool::instance().release(std::move(bundle->data));
      return true;
    }
    return false;
  }

  template <typename T, typename Writer>
  static bool
  push_bundle(T& self, Writer writer, std::span<ossia::bundle_element> addresses)
  {
    if(auto bundle = make_bundle(bundle_server_policy<OscVersion>{}, addresses))
    {
      writer(bundle->data.data(), bundle->data.size());
      ossia::buffer_pool::instance().release(std::move(bundle->data));
      return true;
    }
    return false;
  }

  template <typename T, typename Writer>
  static bool
  push_bundle_bounded(T& self, Writer writer, std::span<ossia::bundle_element> addresses)
  {
    return make_bundle_bounded(
        bundle_bounded_server_policy<OscVersion>{}, addresses,
        [writer](const ossia::net::bundle& bundle) {
      writer(bundle.data.data(), bundle.data.size());
        });
  }
};

}
