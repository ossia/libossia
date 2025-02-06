#pragma once
#include <ossia/detail/config.hpp>
#if defined(OSSIA_PROTOCOL_ARTNET)
#include <ossia/detail/flat_map.hpp>
#include <ossia/network/common/device_parameter.hpp>
#include <ossia/protocols/artnet/artnet_protocol.hpp>

#include <cstdint>

namespace ossia::net
{

template <std::size_t I>
struct artnet_in_visitor;
template <std::size_t I>
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
  dmx_parameter(
      net::node_base& node, dmx_buffer& buffer, const unsigned int channel, int min = 0,
      int max = 255, int8_t bytes = 1);
  ~dmx_parameter();
  int8_t m_bytes{};

  uint32_t channel() const noexcept { return m_channel; }

  void set_dmx_value(const uint8_t* start, const uint8_t* buffer_end);

private:
  void device_update_value() override;

  dmx_buffer& m_buffer;
  const uint32_t m_channel{};

  template <std::size_t I>
  friend struct artnet_in_visitor;
  template <std::size_t I>
  friend struct artnet_out_visitor;
  friend struct artnet_out_var_visitor;
};

class OSSIA_EXPORT dmx_enum_parameter : public device_parameter
{
public:
  dmx_enum_parameter(
      net::node_base& node, dmx_parameter& p,
      std::vector<std::pair<std::string, uint8_t>> values);
  ~dmx_enum_parameter();

private:
  void device_update_value() override;

  dmx_parameter& m_param;

  ossia::flat_map<std::string, uint8_t> m_map;

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
  int16_t m_bytes{};
  int16_t m_channel{};

  template <std::size_t I>
  friend struct artnet_in_visitor;
  template <std::size_t I>
  friend struct artnet_out_visitor;
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
