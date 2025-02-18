#include <ossia/detail/config.hpp>
#if defined(OSSIA_PROTOCOL_ARTNET)
#include "e131_protocol.hpp"

#include <ossia/detail/fmt.hpp>
#include <ossia/protocols/artnet/dmx_parameter.hpp>

#include <boost/asio/ip/host_name.hpp>
#include <boost/asio/ip/multicast.hpp>

#include <chrono>

namespace
{
#pragma pack(push, 1)
struct e131_acn_root_layer
{                          /* ACN Root Layer: 38 bytes */
  uint16_t preamble_size;  /* Preamble Size */
  uint16_t postamble_size; /* Post-amble Size */
  uint8_t acn_pid[12];     /* ACN Packet Identifier */
  uint16_t flength;        /* Flags (high 4 bits) & Length (low 12 bits) */
  uint32_t vector;         /* Layer Vector */
  uint8_t cid[16];         /* Component Identifier (UUID) */
};

struct e131_framing
{
  uint16_t flength;        /* Flags (high 4 bits) & Length (low 12 bits) */
  uint32_t vector;         /* Layer Vector */
  uint8_t source_name[64]; /* User Assigned Name of Source (UTF-8) */
  uint8_t priority;        /* Packet Priority (0-200, default 100) */
  uint16_t reserved;       /* Reserved (should be always 0) */
  uint8_t seq_number;      /* Sequence Number (detect duplicates or out of order
                              packets) */
  uint8_t options;         /* Options Flags (bit 7: preview data, bit 6: stream
                              terminated) */
  uint16_t universe;       /* DMX Universe Number */
};

struct e131_device_management_protocol
{                        /* Device Management Protocol (DMP) Layer: 523 bytes */
  uint16_t flength;      /* Flags (high 4 bits) / Length (low 12 bits) */
  uint8_t vector;        /* Layer Vector */
  uint8_t type;          /* Address Type & Data Type */
  uint16_t first_addr;   /* First Property Address */
  uint16_t addr_inc;     /* Address Increment */
  uint16_t prop_val_cnt; /* Property Value Count (1 + number of slots) */
  uint8_t prop_val[513]; /* Property Values (DMX start code + slots data) */
};

struct e131_packet
{
  e131_acn_root_layer root;
  e131_framing frame;
  e131_device_management_protocol dmp;
};

/* E1.31 Framing Options Type */
enum class e131_option_t
{
  E131_OPT_TERMINATED = 6,
  E131_OPT_PREVIEW = 7,
};

/* Initialize an E1.31 packet using a universe and a number of slots */
static int
e131_pkt_init(e131_packet* packet, const uint16_t universe, const uint16_t num_slots)
{
  if(packet == NULL || universe < 1 || universe > 63999 || num_slots < 1
     || num_slots > 512)
  {
    errno = EINVAL;
    return -1;
  }

  // compute packet layer lengths
  uint16_t prop_val_cnt = num_slots + 1;
  uint16_t dmp_length = prop_val_cnt + sizeof packet->dmp - sizeof packet->dmp.prop_val;
  uint16_t frame_length = sizeof packet->frame + dmp_length;
  uint16_t root_length = sizeof packet->root.flength + sizeof packet->root.vector
                         + sizeof packet->root.cid + frame_length;

  // clear packet
  memset(packet, 0, sizeof *packet);

  /* E1.31 Private Constants */
  const uint16_t _E131_PREAMBLE_SIZE = 0x0010;
  const uint16_t _E131_POSTAMBLE_SIZE = 0x0000;
  const uint8_t _E131_ACN_PID[]
      = {0x41, 0x53, 0x43, 0x2d, 0x45, 0x31, 0x2e, 0x31, 0x37, 0x00, 0x00, 0x00};
  const uint32_t _E131_ROOT_VECTOR = 0x00000004;
  const uint32_t _E131_FRAME_VECTOR = 0x00000002;
  const uint8_t _E131_DMP_VECTOR = 0x02;
  const uint8_t _E131_DMP_TYPE = 0xa1;
  const uint16_t _E131_DMP_FIRST_ADDR = 0x0000;
  const uint16_t _E131_DMP_ADDR_INC = 0x0001;

  // set Root Layer values
  packet->root.preamble_size = htons(_E131_PREAMBLE_SIZE);
  packet->root.postamble_size = htons(_E131_POSTAMBLE_SIZE);
  memcpy(packet->root.acn_pid, _E131_ACN_PID, sizeof packet->root.acn_pid);
  packet->root.flength = htons(0x7000 | root_length);
  packet->root.vector = htonl(_E131_ROOT_VECTOR);

  // char uuid[17] = {
  // "\xfb\x3c\x10\x65\xa1\x7f\x4d\xe2\x99\x19\x31\x7a\x07\xc1\x00\x52" };
  // memcpy(packet->root.cid, uuid, 16);

  // set Framing Layer values
  packet->frame.flength = htons(0x7000 | frame_length);
  packet->frame.vector = htonl(_E131_FRAME_VECTOR);
  memcpy(packet->frame.source_name, "libossia", 8);
  packet->frame.priority = 0x64;
  packet->frame.universe = htons(universe);

  // set Device Management Protocol (DMP) Layer values
  packet->dmp.flength = htons(0x7000 | dmp_length);
  packet->dmp.vector = _E131_DMP_VECTOR;
  packet->dmp.type = _E131_DMP_TYPE;
  packet->dmp.first_addr = htons(_E131_DMP_FIRST_ADDR);
  packet->dmp.addr_inc = htons(_E131_DMP_ADDR_INC);
  packet->dmp.prop_val_cnt = htons(prop_val_cnt);

  return 0;
}

#pragma pack(pop)
static_assert(sizeof(e131_packet) == 638);
}

namespace ossia::net
{
static boost::asio::ip::address e131_host(
    const dmx_config& conf, const ossia::net::outbound_socket_configuration& socket)
{
  if(conf.multicast)
  {
    return boost::asio::ip::address_v4(0xefff0000 | conf.start_universe);
  }
  else
  {
    return boost::asio::ip::make_address(socket.host);
  }
}

e131_protocol::e131_protocol(
    ossia::net::network_context_ptr ctx, const dmx_config& conf,
    const ossia::net::outbound_socket_configuration& socket)
    : dmx_output_protocol_base{ctx, conf}
    , m_socket{e131_host(conf, socket), socket.port, ctx->context}
{
  m_socket.connect();

  if(conf.multicast && !socket.host.empty())
  {
    auto outbound_address = boost::asio::ip::make_address(socket.host).to_v4();
    auto mcast_address = boost::asio::ip::address_v4(0xefff0000 | conf.start_universe);

    m_socket.m_socket.set_option(boost::asio::ip::multicast::enable_loopback(false));
    m_socket.m_socket.set_option(
        boost::asio::ip::multicast::outbound_interface(outbound_address));
    m_socket.m_socket.set_option(
        boost::asio::ip::multicast::join_group(mcast_address, outbound_address));
  }

  m_timer.set_delay(std::chrono::milliseconds{
      static_cast<int>(1000.0f / static_cast<float>(conf.frequency))});
}

e131_protocol::~e131_protocol()
{
  stop_processing();
}

void e131_protocol::set_device(ossia::net::device_base& dev)
{
  dmx_protocol_base::set_device(dev);
  m_timer.start([this] { this->update_function(); });
}

void e131_protocol::update_function()
{
  static std::atomic_int seq = 0;
  for(int current_universe = 0; current_universe < m_buffer.universes();
      current_universe++)
  {
    if(!m_buffer.dirty[current_universe])
      continue;
    try
    {
      int universe = this->m_conf.start_universe + current_universe;
      e131_packet pkt;
      e131_pkt_init(&pkt, universe, this->m_conf.channels_per_universe);

      for(size_t pos = 0; pos < this->m_conf.channels_per_universe; pos++)
        pkt.dmp.prop_val[pos + 1]
            = m_buffer.data[current_universe * m_conf.channels_per_universe + pos];
      pkt.frame.seq_number = seq.fetch_add(1, std::memory_order_relaxed);

      m_socket.write(reinterpret_cast<const char*>(&pkt), sizeof(pkt));
      m_buffer.dirty[current_universe] = false;
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
}

e131_input_protocol::e131_input_protocol(
    ossia::net::network_context_ptr ctx, const dmx_config& conf,
    const ossia::net::inbound_socket_configuration& socket)
    : dmx_input_protocol_base{ctx, conf}
    , m_socket{socket, ctx->context}
{
  if(conf.multicast)
  {
    auto listen_address = boost::asio::ip::make_address_v4(socket.bind);
    auto mcast_address = boost::asio::ip::address_v4(0xefff0000 | conf.start_universe);

    m_socket.m_socket.open(boost::asio::ip::udp::v4());
    m_socket.m_socket.set_option(boost::asio::ip::udp::socket::reuse_address(true));
    m_socket.m_socket.set_option(boost::asio::ip::multicast::enable_loopback(true));
    boost::asio::ip::udp::endpoint listen_endpoint(mcast_address, socket.port);
    m_socket.m_endpoint = listen_endpoint;
    m_socket.m_socket.bind(listen_endpoint);

    m_socket.m_socket.set_option(
        boost::asio::ip::multicast::join_group(mcast_address, listen_address));
  }
  else
  {
    m_socket.open();
  }
}

e131_input_protocol::~e131_input_protocol() { }

void e131_input_protocol::set_device(ossia::net::device_base& dev)
{
  dmx_protocol_base::set_device(dev);

  // FIXME we must make sure that this is actually called after the fixtures have been assigned
  m_socket.receive([this](const char* bytes, int sz) { on_packet(bytes, sz); });
}

void e131_input_protocol::on_packet(const char* bytes, int sz)
{
  if(sz != sizeof(e131_packet))
    return;

  auto& pkt = *reinterpret_cast<const e131_packet*>(bytes);

  int universe = ntohs(pkt.frame.universe);
  if(universe != this->m_conf.start_universe)
    return;

  on_dmx(pkt.dmp.prop_val + 1, std::min(pkt.dmp.prop_val_cnt - 1, 512));
}
}

#endif
