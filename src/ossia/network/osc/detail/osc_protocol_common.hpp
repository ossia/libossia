#pragma once
#include <ossia/network/base/parameter.hpp>
#include <ossia/network/osc/detail/osc_packet_processor.hpp>
#include <ossia/network/osc/detail/osc_value_write_visitor.hpp>
#include <ossia/network/osc/detail/bidir.hpp>
#include <ossia/network/osc/detail/message_generator.hpp>
#include <ossia/network/osc/detail/osc_receive.hpp>

namespace ossia::net
{

struct osc_protocol_common
{
  template<typename T>
  static void init(T& self)
  {
    self.m_impl->from_client.open();
    self.m_impl->to_client.connect();

    self.m_impl->from_client.receive(
          [&self] (const char* data, std::size_t sz) {
        auto on_message = [&self] (auto&& msg) { self.on_received_message(msg); };
        osc_packet_processor<decltype(on_message)>{on_message}({data, sz});
      }
    );
  }

  template<typename T, typename Value_T>
  static bool push(T& self, const ossia::net::parameter_base& addr, Value_T&& v)
  {
    auto val = filter_value(addr, std::forward<Value_T>(v));
    if (val.valid())
    {
      val.apply(osc_value_send_visitor<typename T::socket_type>{addr.get_node().osc_address(), self.m_impl->to_client});
      return true;
    }
    return false;
  }

  template<typename T>
  static bool push_raw(T& self, const ossia::net::full_parameter_data& addr)
  {
    auto val = filter_value(addr, addr.value());
    if (val.valid())
    {
      val.apply(osc_value_send_visitor<typename T::socket_type>{addr.address, self.m_impl->to_client});
      return true;
    }
    return false;
  }

  template<typename T>
  static bool echo_incoming_message(
      T& self,
      const message_origin_identifier& id,
      const parameter_base& addr,
      const value& val)
  {
    if(&id.protocol == &self)
      return true;

    val.apply(osc_value_send_visitor<typename T::socket_type>{addr.get_node().osc_address(), self.m_impl->to_client});
    return true;
  }

  template<typename T>
  static bool observe(T& self, ossia::net::parameter_base& address, bool enable)
  {
    if (enable)
      self.m_listening.insert(
          std::make_pair(address.get_node().osc_address(), &address));
    else
      self.m_listening.erase(address.get_node().osc_address());

    return true;
  }

  template<typename T>
  static void on_received_message(T& self, const oscpack::ReceivedMessage& m)
  {
    if (!self.m_learning)
    {
      ossia::net::on_input_message<false>(
            m.AddressPattern(),
            ossia::net::osc_message_applier{self.m_id, m},
            self.m_listening, *self.m_device, self.m_logger);
    }
    else
    {
      ossia::net::osc_learn(&self.m_device->get_root_node(), m);
    }
  }
};

// Client can't push to GET addresses
struct osc_protocol_client
{
  template<typename T, typename Value_T>
  static bool push(T& self, const ossia::net::parameter_base& addr, Value_T&& v)
  {
    if (addr.get_access() == ossia::access_mode::GET)
      return false;

    return osc_protocol_common::push(self, addr, std::forward<Value_T>(v));
  }

  template<typename T>
  static bool push_raw(T& self, const ossia::net::full_parameter_data& addr)
  {
    if (addr.get_access() == ossia::access_mode::GET)
      return false;

    return osc_protocol_common::push_raw(self, addr);
  }


  template<typename T, typename Impl>
  static bool push_bundle(T& self, Impl& impl,
      const std::vector<const parameter_base*>& addresses)
  {
    if(auto data = make_bundle_client(addresses)) {
      impl.to_client.send(data->stream.Data(), data->stream.Size());
      return true;
    }
    return false;
  }

  template<typename T, typename Impl>
  static bool push_raw_bundle(T& self, Impl& impl,
      const std::vector<ossia::net::full_parameter_data>& addresses)
  {
    if(auto data = make_raw_bundle_client(addresses)) {
      impl.to_client.send(data->stream.Data(), data->stream.Size());
      return true;
    }
    return false;
  }
};

// Servers can push to GET addresses
struct osc_protocol_server
{
  template<typename T, typename Value_T>
  static bool push(T& self, const ossia::net::parameter_base& addr, Value_T&& v)
  {
    return osc_protocol_common::push(self, addr, std::forward<Value_T>(v));
  }

  template<typename T>
  static bool push_raw(T& self, const ossia::net::full_parameter_data& addr)
  {
    return osc_protocol_common::push_raw(self, addr);
  }

  template<typename T, typename Impl>
  static bool push_bundle(T& self, Impl& impl,
                          const std::vector<const parameter_base*>& addresses)
  {
    if(auto data = make_bundle_server(addresses)) {
      impl.to_client.send(data->stream.Data(), data->stream.Size());
      return true;
    }
    return false;
  }

  template<typename T, typename Impl>
  static bool push_raw_bundle(T& self, Impl& impl,
                              const std::vector<ossia::net::full_parameter_data>& addresses)
  {
    if(auto data = make_raw_bundle_server(addresses)) {
      impl.to_client.send(data->stream.Data(), data->stream.Size());
      return true;
    }
    return false;
  }
};

}
