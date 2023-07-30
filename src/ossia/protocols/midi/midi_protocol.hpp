#pragma once
#include <ossia/detail/lockfree_queue.hpp>
#include <ossia/network/base/device.hpp>
#include <ossia/network/base/parameter.hpp>
#include <ossia/network/base/protocol.hpp>
#include <ossia/network/common/parameter_properties.hpp>
#include <ossia/network/context_functions.hpp>
#include <ossia/network/domain/domain.hpp>
#include <ossia/network/value/value.hpp>
#include <ossia/protocols/midi/detail/channel.hpp>

#include <libremidi/api.hpp>
#include <libremidi/message.hpp>
#include <libremidi/observer_configuration.hpp>

#include <array>
#include <atomic>
#include <cassert>
namespace libremidi
{
class midi_in;
class midi_out;
struct message;
}
namespace ossia::net::midi
{
class midi_device;
struct OSSIA_EXPORT midi_info
{
  enum class Type
  {
    Input,
    Output
  };

  midi_info() = default;
  midi_info(const libremidi::input_port& p, bool v)
      : type{Type::Input}
      , handle{p}
      , is_virtual{v}
  {
  }
  midi_info(const libremidi::output_port& p, bool v)
      : type{Type::Output}
      , handle{p}
      , is_virtual{v}
  {
  }

  midi_info(Type t, const libremidi::port_information& p, bool v)
      : type{t}
      , handle{p}
      , is_virtual{v}
  {
  }

  Type type{};
  libremidi::port_information handle{};
  bool is_virtual{};
};

class OSSIA_EXPORT midi_protocol final
    : public ossia::net::protocol_base
    , public Nano::Observer
{
public:
  explicit midi_protocol(
      ossia::net::network_context_ptr, std::string device_name,
      libremidi::API api = libremidi::API::UNSPECIFIED);
  explicit midi_protocol(
      ossia::net::network_context_ptr, midi_info,
      libremidi::API api = libremidi::API::UNSPECIFIED);
  ~midi_protocol();

  bool set_info(midi_info);
  midi_info get_info() const;

  static std::string
  get_midi_port_name(ossia::net::device_base* dev, const midi_info& info);

  static std::vector<midi_info> scan(libremidi::API = libremidi::API::UNSPECIFIED);

  void push_value(const libremidi::message&);

  template <typename T>
  void clone_value(T& port)
  {
    typename T::value_type mess;
    while(messages.try_dequeue(mess))
    {
      port.push_back(mess);
    }
  }

  void enable_registration();

  bool learning() const;
  void set_learning(bool);

private:
  ossia::spsc_queue<libremidi::message> messages;
  ossia::net::network_context_ptr m_context;
  std::unique_ptr<libremidi::midi_in> m_input;
  std::unique_ptr<libremidi::midi_out> m_output;

  std::array<midi_channel, 16> m_channels;

  midi_info m_info{};
  midi_device* m_dev{};
  bool m_registers{};
  std::atomic_bool m_learning{};

  friend class midi_device;
  friend class midi_parameter;
  bool pull(ossia::net::parameter_base&) override;
  bool push(const ossia::net::parameter_base&, const ossia::value& v) override;
  bool push_raw(const ossia::net::full_parameter_data& parameter_base) override;
  bool observe(ossia::net::parameter_base&, bool) override;
  bool update(ossia::net::node_base& node_base) override;
  void set_device(ossia::net::device_base& dev) override;

  void register_parameter(midi_parameter& adrs);
  void unregister_parameter(midi_parameter& adrs);
  void register_parameter_base(const ossia::net::parameter_base& adrs);
  void unregister_parameter_base(const ossia::net::parameter_base& adrs);
  void register_node_base(const ossia::net::node_base& adrs);
  void unregister_node_base(const ossia::net::node_base& adrs);
  void value_callback(
      bool observed, ossia::net::parameter_base& param, const ossia::value& val);

  void midi_callback(const libremidi::message&);
  void on_learn(const libremidi::message& m);
};
}
