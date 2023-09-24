#pragma once

#include <ossia/detail/algorithms.hpp>
#include <ossia/detail/audio_spin_mutex.hpp>
#include <ossia/detail/mutex.hpp>
#include <ossia/network/base/protocol.hpp>

#include <vector>

namespace ossia::net
{
class generic_device;
/**
 * \brief A protocol used to expose a local application's data through multiple
 * other protocols.
 *
 * For instance use this when developing an artistic application with
 * parameters
 * that you want to be able to control from another software.
 */
class OSSIA_EXPORT multiplex_protocol final : public ossia::net::protocol_base
{
public:
  multiplex_protocol();
  multiplex_protocol(const multiplex_protocol&) = delete;
  multiplex_protocol(multiplex_protocol&&) = delete;
  multiplex_protocol& operator=(const multiplex_protocol&) = delete;
  multiplex_protocol& operator=(multiplex_protocol&&) = delete;
  virtual ~multiplex_protocol();

  bool pull(ossia::net::parameter_base&) override;
  bool push(const ossia::net::parameter_base& addr, const ossia::value& v) override;
  bool push_raw(const full_parameter_data&) override;
  bool observe(ossia::net::parameter_base&, bool) override;
  bool update(ossia::net::node_base& node_base) override;

  bool echo_incoming_message(
      const message_origin_identifier&, const parameter_base&,
      const ossia::value& v) override;

  void stop() override;
  void set_device(ossia::net::device_base& dev) override;

  //! Use this to add protocols through which you will expose the device. For
  //! instance OSC, Minuit, etc.
  void expose_to(std::unique_ptr<ossia::net::protocol_base> p);

  //! Stop exposition to a protocol. It will be deleted.
  void stop_expose_to(const ossia::net::protocol_base& p);

  void clear();

  //! The protocols we are currently exposing this device through.
  const auto& get_protocols() const { return m_protocols; }

private:
  std::vector<std::unique_ptr<ossia::net::protocol_base>>
      m_protocols TS_GUARDED_BY(m_protocols_mutex);
  std::vector<std::unique_ptr<ossia::net::protocol_base>>
      m_protocols_to_register TS_GUARDED_BY(m_protocols_mutex);
  ossia::audio_spin_mutex m_protocols_mutex;
  ossia::net::device_base* m_device{};
};

using local_protocol = multiplex_protocol;
}
