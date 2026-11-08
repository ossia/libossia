#pragma once
#include <ossia/detail/config.hpp>
#if defined(OSSIA_PROTOCOL_ARTNET)
#include <ossia/detail/flat_map.hpp>
#include <ossia/network/common/device_parameter.hpp>
#include <ossia/protocols/artnet/artnet_protocol.hpp>

#include <algorithm>
#include <array>
#include <cstdint>
#include <span>

namespace ossia::net
{

struct artnet_in_visitor;
struct artnet_out_visitor;
struct artnet_out_var_visitor;
struct dmx_buffer;
struct dmx_range
{
  int start{};
  int num_bytes{};
};
class OSSIA_EXPORT dmx_parameter : public device_parameter
{
public:
  static constexpr int max_bytes = 4;

  //! The value spans `bytes` consecutive channels starting at `channel`, MSB first.
  dmx_parameter(
      net::node_base& node, dmx_buffer& buffer, const unsigned int channel, int min = 0,
      int max = 255, int8_t bytes = 1);

  //! The value spans the given channels, MSB first: they need not be consecutive.
  dmx_parameter(
      net::node_base& node, dmx_buffer& buffer, std::span<const uint32_t> channels,
      int min, int max);
  ~dmx_parameter();
  int8_t m_bytes{};

  uint32_t channel() const noexcept { return m_channels[0]; }
  std::span<const uint32_t> channels() const noexcept
  {
    return {m_channels.data(), std::size_t(std::clamp<int>(m_bytes, 1, max_bytes))};
  }

  //! dmx points to channel 0 of the universe the parameter's channels index into.
  void set_dmx_value(const uint8_t* dmx, const uint8_t* buffer_end);

private:
  void device_update_value() override;

  dmx_buffer& m_buffer;
  std::array<uint32_t, max_bytes> m_channels{};

  friend struct artnet_in_visitor;
  friend struct artnet_out_visitor;
};

class OSSIA_EXPORT dmx_enum_parameter : public device_parameter
{
public:
  dmx_enum_parameter(
      net::node_base& node, dmx_parameter& p,
      std::vector<std::pair<std::string, uint32_t>> values);
  ~dmx_enum_parameter();

private:
  void device_update_value() override;

  dmx_parameter& m_param;

  ossia::flat_map<std::string, uint32_t> m_map;

  friend struct artnet_enum_visitor;
};

class OSSIA_EXPORT dmx_range_parameter : public device_parameter
{
public:
  dmx_range_parameter(
      net::node_base& node, dmx_buffer& buffer, dmx_range range, int min = 0,
      int max = 255);
  ~dmx_range_parameter();

  uint32_t channel() const noexcept { return m_channel; }

private:
  void device_update_value() override;

  dmx_buffer& m_buffer;
  int m_bytes{};
  int m_channel{};

  friend struct artnet_out_var_visitor;
};

/*
struct artnet_range_element {
  std::string name;
  int min{0};
  int max{512};
};

class artnet_range_parameter : public device_parameter
{
  using dmx_buffer = artnet_protocol::dmx_buffer;

public:
  artnet_range_parameter(
      net::node_base& node, dmx_buffer& buffer, unsigned int channel, int min,
int max); ~artnet_range_parameter();

private:
  void device_update_value() override;

  dmx_buffer& m_buffer;
  const unsigned int m_channel{};

  friend struct artnet_range_visitor;
};
*/

}
#endif
