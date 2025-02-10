#include <ossia/detail/config.hpp>
#if defined(OSSIA_PROTOCOL_ARTNET)
#include "artnet_protocol.hpp"

#include <ossia/detail/fmt.hpp>
#include <ossia/protocols/artnet/dmx_parameter.hpp>

#include <boost/asio/use_future.hpp>

#include <artnet/artnet.h>

#include <chrono>
namespace ossia::net
{
dmx_buffer::dmx_buffer(int universe_size)
    : universe_size{universe_size}
{
  set_universe_count(1);
}

dmx_buffer::~dmx_buffer() = default;
struct art_dmx_packet_header
{
  char id[8] = {'A', 'r', 't', '-', 'N', 'e', 't', '\0'};
  uint8_t opcode[2] = {0x00, 0x50};
  uint8_t version[2] = {0x00, 0x0e};
  uint8_t seq = 0;
  uint8_t phys = 0;
  uint8_t lo_uni = 0;
  uint8_t hi_uni = 0;
  uint16_t length = 2;
};

struct art_dmx_packet : art_dmx_packet_header
{
  uint8_t data[512];
};

artnet_protocol::artnet_protocol(
    ossia::net::network_context_ptr ctx, const dmx_config& conf,
    const ossia::net::outbound_socket_configuration& socket)
    : dmx_output_protocol_base{ctx, conf}
    , m_socket{socket, ctx->context}
{
  m_socket.m_broadcast = true;
  m_socket.connect();
  m_timer.set_delay(std::chrono::milliseconds{
      static_cast<int>(1000.0f / static_cast<float>(conf.frequency))});
}

artnet_protocol::~artnet_protocol()
{
  stop_processing();
}

void artnet_protocol::set_device(ossia::net::device_base& dev)
{
  dmx_protocol_base::set_device(dev);
  m_timer.start([this] { this->update_function(); });
}

void artnet_protocol::update_function()
{
  for(int current_universe = 0; current_universe < m_buffer.universes();
      current_universe++)
  {
    if(!m_buffer.dirty[current_universe])
      continue;

    int universe = this->m_conf.start_universe + current_universe;
    auto data = m_buffer.read_universe(current_universe);

    art_dmx_packet pkt;
    pkt.lo_uni = 0; //this->m_conf.start_universe + current_universe;
    pkt.length = boost::asio::detail::socket_ops::host_to_network_short(
        m_conf.channels_per_universe);
    std::copy_n(data.data(), data.size(), pkt.data);

    m_socket.write(reinterpret_cast<const char*>(&pkt), sizeof(pkt));
    m_buffer.dirty[current_universe] = false;
  }
}

artnet_input_protocol::artnet_input_protocol(
    ossia::net::network_context_ptr ctx, const dmx_config& conf,
    const ossia::net::inbound_socket_configuration& socket)
    : dmx_input_protocol_base{ctx, conf}
    , m_socket{socket, ctx->context}
{
}

artnet_input_protocol::~artnet_input_protocol() { }

void artnet_input_protocol::set_device(ossia::net::device_base& dev)
{
  dmx_protocol_base::set_device(dev);

  m_socket.m_socket.non_blocking(true);
  m_socket.open();

  // FIXME we must make sure that this is actually called after the fixtures have been assigned
  m_socket.receive(
      [this](const char* bytes, int sz) { on_dmx((const uint8_t*)bytes, sz); });
}

void artnet_input_protocol::stop()
{
  m_socket.close();
}
}

#endif
