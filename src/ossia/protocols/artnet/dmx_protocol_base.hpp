#pragma once
#include <ossia/detail/config.hpp>

#include <ossia/detail/timer.hpp>
#include <ossia/network/base/protocol.hpp>
#include <ossia/network/common/complex_type.hpp>
#include <ossia/network/context.hpp>
#include <ossia/network/domain/domain.hpp>
#include <ossia/protocols/artnet/dmx_buffer.hpp>

#include <array>
#include <cstdint>
#include <vector>

namespace ossia::net
{
struct dmx_config;
class dmx_parameter;
class OSSIA_EXPORT dmx_protocol_base : public ossia::net::protocol_base
{
public:
  dmx_protocol_base(ossia::net::network_context_ptr, const dmx_config& conf);
  ~dmx_protocol_base();

  void set_device(ossia::net::device_base& dev) override;

  bool pull(ossia::net::parameter_base& param) override;
  bool push(const ossia::net::parameter_base& param, const ossia::value& v) override;
  bool push_raw(const ossia::net::full_parameter_data&) override;
  bool observe(ossia::net::parameter_base& param, bool enable) override;

  bool update(ossia::net::node_base&) override;

  dmx_buffer& buffer() noexcept { return m_buffer; }

protected:
  ossia::net::network_context_ptr m_context;

  dmx_buffer m_buffer;

  ossia::net::device_base* m_device{};
  dmx_config m_conf{};
};

class OSSIA_EXPORT dmx_output_protocol_base : public dmx_protocol_base
{
public:
  dmx_output_protocol_base(ossia::net::network_context_ptr, const dmx_config& conf);
  void stop_processing();

protected:
  //! Called by the send timer for each universe: true when it changed, and once a
  //! second after it was first sent, as receivers drop a source that goes silent.
  bool should_send(int universe);

  ossia::timer m_timer;
  std::vector<uint32_t> m_idle_ticks;
};

class OSSIA_EXPORT dmx_input_protocol_base : public dmx_protocol_base
{
public:
  using dmx_protocol_base::dmx_protocol_base;

  //! Caches the DMX address -> ossia parameter mapping for fast operation
  //! during input. Packets are applied only once it exists, so it is called
  //! after the parameters are created, from the thread that created them.
  void create_channel_map();

  //! Applies a received universe. Called where the packets are received.
  void on_dmx(const uint8_t* dmx, int count);

protected:
  using channel_map = std::array<dmx_parameter*, 512>;

  //! Gives the map to the thread that calls on_dmx: m_cache is only touched
  //! there. Protocols that receive on another thread post it there.
  virtual void apply_channel_map(const channel_map& map);

  channel_map m_cache{};
};
}
