#include <ossia/detail/config.hpp>
#if defined(OSSIA_PROTOCOL_ARTNET)
#include "artnet_protocol.hpp"

#include <ossia/detail/fmt.hpp>
#include <ossia/protocols/artnet/dmx_parameter.hpp>

#include <boost/asio/use_future.hpp>

#include <artnet/artnet.h>

#include <chrono>
#include <iostream>

namespace ossia::net
{
dmx_buffer::dmx_buffer()
    : dirty(false)
{
  std::memset(data, 0, DMX_CHANNEL_COUNT);
}

dmx_buffer::~dmx_buffer() = default;

artnet_protocol::artnet_protocol(
    ossia::net::network_context_ptr ctx, const dmx_config& conf, std::string_view host)
    : dmx_output_protocol_base{ctx, conf}
{
  if(conf.frequency < 1 || conf.frequency > 44)
    throw std::runtime_error("DMX 512 update frequency must be in the range [1, 44] Hz");

  m_timer.set_delay(std::chrono::milliseconds{
      static_cast<int>(1000.0f / static_cast<float>(conf.frequency))});

  //  44  hz limit apply because we send 512 byte frames.
  //  It seem to be possible to send only some value and thus
  //   update at higher frequencies => Work TODO

  //  Do not specify ip address for now, artnet will choose one
#if defined(_NDEBUG)
  bool verbose = 0;
#else
  bool verbose = 1;
#endif
  m_node = artnet_new(host.data(), verbose);

  if(m_node == NULL)
    throw std::runtime_error("Artnet new failed");

  static constexpr int artnet_port_id = 0;
  static constexpr int artnet_subnet_id = 0;
  artnet_set_port_type(m_node, artnet_port_id, ARTNET_ENABLE_INPUT, ARTNET_PORT_DMX);
  artnet_set_port_addr(m_node, artnet_port_id, ARTNET_INPUT_PORT, m_conf.universe);
  artnet_set_subnet_addr(m_node, artnet_subnet_id);

  artnet_set_short_name(m_node, "libossia");
  artnet_set_long_name(m_node, "libossia artnet protocol");
  artnet_set_node_type(m_node, ARTNET_RAW);

  artnet_dump_config(m_node);
  std::fflush(stdout);
  std::fflush(stderr);
  if(artnet_start(m_node) != ARTNET_EOK)
    throw std::runtime_error("Artnet Start failed");
}

artnet_protocol::~artnet_protocol()
{
  stop_processing();
  artnet_destroy(m_node);
}

void artnet_protocol::set_device(ossia::net::device_base& dev)
{
  dmx_protocol_base::set_device(dev);

  m_timer.start([this] { this->update_function(); });
}

void artnet_protocol::update_function()
{
  for(int i = 0, n = m_buffer.size(); i < n; i++)
  {
    if(auto& buffer = m_buffer[i]; buffer.dirty)
    {
      artnet_raw_send_dmx(m_node, i + m_conf.universe, DMX_CHANNEL_COUNT, buffer.data);

      buffer.dirty = false;
    }
  }
}

artnet_input_protocol::artnet_input_protocol(
    ossia::net::network_context_ptr ctx, const dmx_config& conf, std::string_view host)
    : dmx_input_protocol_base{ctx, conf}
{
  if(conf.frequency < 1 || conf.frequency > 44)
    throw std::runtime_error("DMX 512 update frequency must be in the range [1, 44] Hz");

#if defined(_NDEBUG)
  bool verbose = 0;
#else
  bool verbose = 1;
#endif
  m_node = artnet_new(host.data(), verbose);

  if(m_node == NULL)
    throw std::runtime_error("Artnet new failed");

  static constexpr int artnet_port_id = 0;
  static constexpr int artnet_subnet_id = 0;
  artnet_set_port_type(m_node, artnet_port_id, ARTNET_ENABLE_OUTPUT, ARTNET_PORT_DMX);
  artnet_set_port_addr(m_node, artnet_port_id, ARTNET_OUTPUT_PORT, m_conf.universe);
  // artnet_set_subnet_addr(m_node, artnet_subnet_id);

  artnet_set_short_name(m_node, "libossia");
  artnet_set_long_name(m_node, "libossia artnet protocol");
  artnet_set_node_type(m_node, ARTNET_NODE);
  artnet_set_dmx_handler(
      m_node,
      [](artnet_node n, int port, void* d) -> int {
        auto& self = *(artnet_input_protocol*)d;
        self.on_packet(n, port);
        return 0;
      },
      this);

  artnet_dump_config(m_node);
  std::fflush(stdout);
  std::fflush(stderr);
  if(artnet_start(m_node) != ARTNET_EOK)
    throw std::runtime_error("Artnet Start failed");
}

artnet_input_protocol::~artnet_input_protocol() { }

void artnet_input_protocol::set_device(ossia::net::device_base& dev)
{
  dmx_protocol_base::set_device(dev);

  m_socket = std::make_unique<ossia::net::udp_receive_socket>(m_context->context);
  m_socket->assign(artnet_get_sd(m_node));
  auto& sock = m_socket->m_socket;
  sock.non_blocking(true);

  do_read();
}

void artnet_input_protocol::do_read()
{
  auto& sock = m_socket->m_socket;
  sock.async_wait(
      boost::asio::ip::udp::socket::wait_read, [this](boost::system::error_code ec) {
        if(ec == boost::asio::error::operation_aborted)
          return;
        artnet_read_one(m_node);
        do_read();
      });
}

void artnet_input_protocol::on_packet(artnet_node n, int port)
{
  int length = 0;
  auto data = artnet_read_dmx(n, port, &length);
  on_dmx(data, std::min(length, 512));
}

void artnet_input_protocol::stop()
{
  if(m_socket)
  {
    m_socket->close();
  }

  if(m_node)
  {
    artnet_destroy(m_node);
    m_node = {};
  }
}
}

#endif
