#include <ossia/detail/config.hpp>

#include <ossia/network/base/node_functions.hpp>
#include <ossia/network/context.hpp>
#include <ossia/network/domain/domain.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/local/local.hpp>
#include <ossia/protocols/artnet/dmx_buffer.hpp>
#include <ossia/protocols/artnet/dmx_led_parameter.hpp>
#include <ossia/protocols/artnet/dmx_parameter.hpp>
#include <ossia/protocols/artnet/dmx_protocol_base.hpp>
#include <ossia/protocols/artnet/artnet_protocol.hpp>
#include <ossia/protocols/artnet/dmxusbpro_protocol.hpp>
#include <ossia/protocols/artnet/e131_protocol.hpp>

#include <boost/asio/ip/udp.hpp>

#include "include_catch.hpp"

#if __has_include(<pty.h>)
#include <fcntl.h>
#include <pty.h>
#include <termios.h>
#include <unistd.h>
#define DMXTEST_HAS_PTY 1
#endif

#include <array>
#include <chrono>
#include <limits>
#include <map>
#include <thread>

using namespace ossia::net;

namespace
{
struct Rig
{
  generic_device dev{std::make_unique<multiplex_protocol>(), "dmx"};
  dmx_buffer buffer{512};

  explicit Rig(int universes = 1) { buffer.set_universe_count(universes); }

  template <typename P, typename... Args>
  P& make(std::string_view name, Args&&... args)
  {
    auto node = dev.get_root_node().create_child(std::string(name));
    auto param = std::make_unique<P>(*node, buffer, std::forward<Args>(args)...);
    auto& p = *param;
    node->set_parameter(std::move(param));
    return p;
  }

  std::vector<ossia::value> ramp(int n, int first = 1)
  {
    std::vector<ossia::value> v;
    for(int i = 0; i < n; i++)
      v.push_back(first + i);
    return v;
  }
};
}

TEST_CASE("write_channels marks every universe it touches", "[dmx]")
{
  dmx_buffer buffer{512};
  buffer.set_universe_count(2);

  CHECK(buffer.write_channels(0, 512).size() == 512);
  CHECK(buffer.dirty[0]);
  CHECK_FALSE(buffer.dirty[1]);

  buffer.dirty.reset();
  CHECK(buffer.write_channels(510, 4).size() == 4);
  CHECK(buffer.dirty[0]);
  CHECK(buffer.dirty[1]);

  buffer.dirty.reset();
  CHECK(buffer.write_channels(1000, 25).empty());
  CHECK(buffer.write_channels(-1, 2).empty());
  CHECK(buffer.write_channels(0, 0).empty());
  CHECK(buffer.dirty.none());
}

TEST_CASE("write_channel ignores channels past the last universe", "[dmx]")
{
  dmx_buffer buffer{100};
  buffer.set_universe_count(2);
  buffer.write_channel(150, 7);
  CHECK(buffer.data[150] == 7);
  CHECK(buffer.dirty[1]);
  CHECK_FALSE(buffer.dirty[0]);

  buffer.dirty.reset();
  buffer.write_channel(200, 9);
  CHECK(buffer.dirty.none());
}

TEST_CASE("A multi-byte parameter writes MSB first into its channels", "[dmx]")
{
  Rig rig;
  const std::array<uint32_t, 2> channels{10, 3};
  auto& p = rig.make<dmx_parameter>("pan", std::span{channels}, 0, 65535);
  CHECK(p.m_bytes == 2);
  CHECK(p.channel() == 10);

  p.push_value(0x1234);
  CHECK(rig.buffer.data[10] == 0x12);
  CHECK(rig.buffer.data[3] == 0x34);

  p.push_value(70000);
  CHECK(rig.buffer.data[10] == 0xFF);
  CHECK(rig.buffer.data[3] == 0xFF);

  p.push_value(-3);
  CHECK(rig.buffer.data[10] == 0);
  CHECK(rig.buffer.data[3] == 0);

  p.push_value(std::numeric_limits<float>::quiet_NaN());
  CHECK(rig.buffer.data[10] == 0);
}

TEST_CASE("The contiguous constructor spans consecutive channels", "[dmx]")
{
  Rig rig;
  auto& p = rig.make<dmx_parameter>("dimmer", 5u, 0, 0xFFFFFF, int8_t(3));
  p.push_value(0x010203);
  CHECK(rig.buffer.data[5] == 1);
  CHECK(rig.buffer.data[6] == 2);
  CHECK(rig.buffer.data[7] == 3);
}

TEST_CASE("A parameter reads its value back from incoming DMX", "[dmx]")
{
  Rig rig;
  const std::array<uint32_t, 2> channels{4, 1};
  auto& p = rig.make<dmx_parameter>("pan", std::span{channels}, 0, 65535);

  std::array<uint8_t, 8> dmx{};
  dmx[4] = 0xAB;
  dmx[1] = 0xCD;
  p.set_dmx_value(dmx.data(), dmx.data() + dmx.size());
  CHECK(ossia::convert<int>(p.value()) == 0xABCD);

  dmx[4] = 0;
  p.set_dmx_value(dmx.data(), dmx.data() + 3);
  CHECK(ossia::convert<int>(p.value()) == 0xABCD);
}

TEST_CASE("Enum presets drive their channel", "[dmx]")
{
  Rig rig;
  auto& chan = rig.make<dmx_parameter>("gobo", 0u, 0, 65535, int8_t(2));
  auto node = chan.get_node().create_child("preset");
  auto preset = std::make_unique<dmx_enum_parameter>(
      *node, chan,
      std::vector<std::pair<std::string, uint32_t>>{{"Open", 0}, {"Star", 0x8000}});
  auto& e = *preset;
  node->set_parameter(std::move(preset));

  e.push_value(std::string("Star"));
  CHECK(ossia::convert<int>(chan.value()) == 0x8000);
  CHECK(rig.buffer.data[0] == 0x80);
  CHECK(rig.buffer.data[1] == 0);

  e.push_value(std::string("Unknown"));
  CHECK(ossia::convert<int>(chan.value()) == 0x8000);
}

TEST_CASE("LED values are clamped to a byte", "[dmx][led]")
{
  Rig rig;
  auto& led = rig.make<dmx_led_parameter>("strip", 0u, int8_t(4), 128);
  auto values = rig.ramp(512);
  values[0] = 300;
  values[1] = -5;
  led.push_value(values);
  CHECK(rig.buffer.data[0] == 255);
  CHECK(rig.buffer.data[1] == 0);
  CHECK(rig.buffer.data[2] == 3);
  CHECK(rig.buffer.data[511] == 255);
  CHECK(rig.buffer.dirty[0]);
}

TEST_CASE("A reversed LED strip writes its pixels from the end", "[dmx][led]")
{
  Rig rig;
  auto& led = rig.make<dmx_led_parameter>("strip", 2u, int8_t(3), 3, true);
  led.push_value(rig.ramp(9));
  const int expected[] = {7, 8, 9, 4, 5, 6, 1, 2, 3};
  for(int i = 0; i < 9; i++)
    CHECK(rig.buffer.data[2 + i] == expected[i]);

  rig.buffer.data.assign(rig.buffer.data.size(), 0);
  led.push_value(rig.ramp(3, 10));
  CHECK(rig.buffer.data[2 + 6] == 10);
  CHECK(rig.buffer.data[2 + 7] == 11);
  CHECK(rig.buffer.data[2 + 8] == 12);
  CHECK(rig.buffer.data[2] == 0);
}

TEST_CASE("A range parameter writes its own universe", "[dmx]")
{
  Rig rig{3};
  auto& u1 = rig.make<dmx_range_parameter>("1", dmx_range{512, 512}, 0, 255);
  u1.push_value(rig.ramp(4, 254));
  CHECK(rig.buffer.data[512] == 254);
  CHECK(rig.buffer.data[513] == 255);
  CHECK(rig.buffer.data[514] == 255);
  CHECK(rig.buffer.data[0] == 0);
  CHECK(rig.buffer.dirty[1]);
  CHECK_FALSE(rig.buffer.dirty[0]);
  CHECK_FALSE(rig.buffer.dirty[2]);

  rig.buffer.dirty.reset();
  auto& u2 = rig.make<dmx_range_parameter>("2", dmx_range{1024, 512}, 0, 255);
  u2.push_value(9);
  CHECK(rig.buffer.data[1024] == 9);
  CHECK(rig.buffer.data[1535] == 9);
  CHECK(rig.buffer.dirty[2]);
  CHECK_FALSE(rig.buffer.dirty[1]);
}

TEST_CASE("Universe autocreation gives each universe its own range", "[dmx]")
{
  dmx_config conf;
  conf.autocreate = dmx_config::just_universes;
  conf.start_universe = 2;
  conf.universe_count = 3;
  conf.channels_per_universe = 100;

  auto proto = std::make_unique<dmx_protocol_base>(
      std::make_shared<network_context>(), conf);
  auto& p = *proto;
  generic_device dev{std::move(proto), "dmx"};

  CHECK(p.buffer().universes() == 3);
  CHECK(find_node(dev.get_root_node(), "/5") == nullptr);
  auto node = find_node(dev.get_root_node(), "/4");
  REQUIRE(node);
  REQUIRE(node->get_parameter());
  node->get_parameter()->push_value(std::vector<ossia::value>{42});
  CHECK(p.buffer().data[200] == 42);
  CHECK(p.buffer().data[0] == 0);
  CHECK(p.buffer().dirty[2]);
}

TEST_CASE("Incoming DMX skips parameters that are not channels", "[dmx]")
{
  dmx_config conf;
  conf.mode = dmx_config::sink;
  auto proto = std::make_unique<dmx_input_protocol_base>(
      std::make_shared<network_context>(), conf);
  auto& p = *proto;
  generic_device dev{std::move(proto), "dmx"};

  auto& root = dev.get_root_node();
  auto cnode = root.create_child("c");
  const std::array<uint32_t, 2> channels{2, 3};
  auto chan = std::make_unique<dmx_parameter>(
      *cnode, p.buffer(), std::span{channels}, 0, 65535);
  auto& c = *chan;
  cnode->set_parameter(std::move(chan));
  auto enode = cnode->create_child("preset");
  enode->set_parameter(std::make_unique<dmx_enum_parameter>(
      *enode, c, std::vector<std::pair<std::string, uint32_t>>{{"A", 0}}));

  p.create_channel_map();
  std::array<uint8_t, 512> dmx{};
  dmx[2] = 1;
  dmx[3] = 2;
  p.on_dmx(dmx.data(), dmx.size());
  CHECK(ossia::convert<int>(c.value()) == 0x0102);
}

TEST_CASE("Dirty flags keep their state when universes are added", "[dmx]")
{
  dmx_buffer buffer{512};
  buffer.set_universe_count(2);
  buffer.dirty[1] = true;
  buffer.set_universe_count(4);
  CHECK(buffer.universes() == 4);
  CHECK_FALSE(buffer.dirty[0]);
  CHECK(buffer.dirty[1]);
  CHECK_FALSE(buffer.dirty[3]);
  CHECK(buffer.read_universe(1).size() == 512);
  CHECK(buffer.dirty.none());
}

TEST_CASE("Enum values beyond the int range saturate", "[dmx]")
{
  Rig rig;
  auto& chan = rig.make<dmx_parameter>("x", 0u, 0, std::numeric_limits<int>::max(), int8_t(4));
  auto node = chan.get_node().create_child("preset");
  auto preset = std::make_unique<dmx_enum_parameter>(
      *node, chan, std::vector<std::pair<std::string, uint32_t>>{{"Top", 0xC0000000u}});
  auto& e = *preset;
  node->set_parameter(std::move(preset));

  e.push_value(std::string("Top"));
  CHECK(ossia::convert<int>(chan.value()) == std::numeric_limits<int>::max());
  CHECK(rig.buffer.data[0] == 0x7F);
  CHECK(rig.buffer.data[3] == 0xFF);
}

TEST_CASE("Incoming DMX ignores channels outside the received universe", "[dmx]")
{
  dmx_config conf;
  conf.mode = dmx_config::sink;
  conf.channels_per_universe = 100;
  auto proto = std::make_unique<dmx_input_protocol_base>(
      std::make_shared<network_context>(), conf);
  auto& p = *proto;
  generic_device dev{std::move(proto), "dmx"};
  p.buffer().set_universe_count(2);

  auto make = [&](std::string name, std::array<uint32_t, 2> channels) -> dmx_parameter& {
    auto node = dev.get_root_node().create_child(name);
    auto param = std::make_unique<dmx_parameter>(
        *node, p.buffer(), std::span{channels}, 0, 65535);
    auto& ref = *param;
    node->set_parameter(std::move(param));
    ref.set_value(0);
    return ref;
  };
  auto& inside = make("inside", {10, 11});
  auto& second = make("second", {110, 111});
  auto& straddling = make("straddling", {20, 120});

  p.create_channel_map();
  std::array<uint8_t, 512> dmx{};
  dmx[10] = 1;
  dmx[20] = 2;
  dmx[110] = 3;
  dmx[120] = 4;
  p.on_dmx(dmx.data(), dmx.size());
  CHECK(ossia::convert<int>(inside.value()) == 0x0100);
  CHECK(ossia::convert<int>(second.value()) == 0);
  CHECK(ossia::convert<int>(straddling.value()) == 0);
}

namespace
{
using udp = boost::asio::ip::udp;
using namespace std::chrono_literals;

//! Receives the datagrams a protocol sends to 127.0.0.1.
struct udp_sink
{
  boost::asio::io_context io;
  udp::socket sock{io};

  udp_sink()
  {
    sock.open(udp::v4());
    sock.bind({boost::asio::ip::make_address("127.0.0.1"), 0});
    sock.non_blocking(true);
  }

  uint16_t port() const { return sock.local_endpoint().port(); }

  std::vector<std::vector<uint8_t>> receive_for(std::chrono::milliseconds d)
  {
    std::vector<std::vector<uint8_t>> res;
    const auto end = std::chrono::steady_clock::now() + d;
    std::array<uint8_t, 2048> buf;
    while(std::chrono::steady_clock::now() < end)
    {
      boost::system::error_code ec;
      udp::endpoint from;
      const auto n = sock.receive_from(boost::asio::buffer(buf), from, 0, ec);
      if(ec)
        std::this_thread::sleep_for(2ms);
      else
        res.emplace_back(buf.begin(), buf.begin() + n);
    }
    return res;
  }
};

//! Runs a network context on its own thread for the lifetime of a device.
struct net_thread
{
  std::shared_ptr<network_context> ctx = std::make_shared<network_context>();
  std::thread thread{[c = ctx] { c->run(); }};
  ~net_thread()
  {
    ctx->context.stop();
    thread.join();
  }
};

uint16_t free_port()
{
  boost::asio::io_context io;
  udp::socket s{io, {boost::asio::ip::make_address("127.0.0.1"), 0}};
  return s.local_endpoint().port();
}

dmx_parameter& add_channel(generic_device& dev, dmx_buffer& buffer, std::string name, int channel)
{
  auto node = dev.get_root_node().create_child(name);
  auto param = std::make_unique<dmx_parameter>(*node, buffer, channel);
  auto& p = *param;
  node->set_parameter(std::move(param));
  return p;
}

int artnet_universe(const std::vector<uint8_t>& pkt)
{
  return pkt[14] | (pkt[15] << 8);
}

int e131_universe(const std::vector<uint8_t>& pkt)
{
  return (pkt[113] << 8) | pkt[114];
}
}

TEST_CASE("Art-Net sends the Net of universes above 255", "[dmx][artnet]")
{
  udp_sink sink;
  net_thread net;
  {
    dmx_config conf;
    conf.start_universe = 255;
    auto proto = std::make_unique<artnet_protocol>(
        net.ctx, conf, outbound_socket_configuration{"127.0.0.1", sink.port()});
    auto& p = *proto;
    p.buffer().set_universe_count(2);
    generic_device dev{std::move(proto), "dmx"};
    add_channel(dev, p.buffer(), "a", 3).push_value(7);
    add_channel(dev, p.buffer(), "b", 512 + 3).push_value(9);

    std::array<int, 2> seen{};
    for(auto& pkt : sink.receive_for(200ms))
    {
      REQUIRE(pkt.size() == 18 + 512);
      if(artnet_universe(pkt) == 255 && pkt[18 + 3] == 7)
        seen[0]++;
      if(artnet_universe(pkt) == 256 && pkt[18 + 3] == 9)
        seen[1]++;
      CHECK(artnet_universe(pkt) != 0);
    }
    CHECK(seen[0] > 0);
    CHECK(seen[1] > 0);
  }
}

TEST_CASE("Art-Net frames carry an even length and nothing after it", "[dmx][artnet]")
{
  for(int channels : {510, 511, 7})
  {
    udp_sink sink;
    net_thread net;
    dmx_config conf;
    conf.channels_per_universe = channels;
    auto proto = std::make_unique<artnet_protocol>(
        net.ctx, conf, outbound_socket_configuration{"127.0.0.1", sink.port()});
    auto& p = *proto;
    generic_device dev{std::move(proto), "dmx"};
    add_channel(dev, p.buffer(), "a", channels - 1).push_value(5);

    const auto pkts = sink.receive_for(150ms);
    REQUIRE(!pkts.empty());
    const auto& pkt = pkts.back();
    const int length = (pkt[16] << 8) | pkt[17];
    CHECK(length == (channels + 1) / 2 * 2);
    CHECK(pkt.size() == 18 + std::size_t(length));
    CHECK(pkt[18 + channels - 1] == 5);
  }
}

TEST_CASE("sACN universes have their own sequence and are kept alive", "[dmx][e131]")
{
  udp_sink sink;
  net_thread net;
  {
    dmx_config conf;
    conf.start_universe = 0; // not a valid sACN universe: never sent
    conf.multicast = false;
    auto proto = std::make_unique<e131_protocol>(
        net.ctx, conf, outbound_socket_configuration{"127.0.0.1", sink.port()});
    auto& p = *proto;
    p.buffer().set_universe_count(3);
    generic_device dev{std::move(proto), "dmx"};
    add_channel(dev, p.buffer(), "u0", 0).push_value(1);
    add_channel(dev, p.buffer(), "u1", 512).push_value(2);
    add_channel(dev, p.buffer(), "u2", 1024).push_value(3);

    // Nothing changes: each universe still goes out about once a second. The
    // window leaves room for timer jitter on loaded machines.
    const auto pkts = sink.receive_for(3500ms);
    std::array<std::vector<uint8_t>, 3> seq;
    std::array<uint8_t, 16> cid{};
    for(auto& pkt : pkts)
    {
      REQUIRE(pkt.size() == 638);
      const int u = e131_universe(pkt);
      REQUIRE(u >= 1);
      REQUIRE(u <= 2);
      seq[u].push_back(pkt[111]);
      CHECK(pkt[126 + 0] == u + 1);
      std::copy_n(pkt.begin() + 22, 16, cid.begin());
    }
    CHECK(seq[1].size() >= 3);
    CHECK(seq[2].size() >= 3);
    for(int u : {1, 2})
      for(std::size_t i = 1; i < seq[u].size(); i++)
        CHECK(uint8_t(seq[u][i] - seq[u][i - 1]) == 1);
    CHECK(std::any_of(cid.begin(), cid.end(), [](uint8_t b) { return b != 0; }));
  }
}

TEST_CASE("Art-Net input reads its own universe only", "[dmx][artnet]")
{
  net_thread net;
  const uint16_t port = free_port();
  dmx_config conf;
  conf.mode = dmx_config::sink;
  conf.start_universe = 3;
  auto proto = std::make_unique<artnet_input_protocol>(
      net.ctx, conf, inbound_socket_configuration{"127.0.0.1", port});
  auto& p = *proto;
  generic_device dev{std::move(proto), "dmx"};
  auto& a = add_channel(dev, p.buffer(), "a", 2);
  a.set_value(0);
  p.create_channel_map();

  auto send = [&](int universe, uint8_t value) {
    std::vector<uint8_t> pkt{'A', 'r', 't', '-', 'N', 'e', 't', 0, 0x00, 0x50, 0, 14, 0, 0,
                             uint8_t(universe & 0xFF), uint8_t(universe >> 8), 0, 4};
    pkt.insert(pkt.end(), {0, 0, value, 0});
    boost::asio::io_context io;
    udp::socket s{io, udp::v4()};
    s.send_to(boost::asio::buffer(pkt), {boost::asio::ip::make_address("127.0.0.1"), port});
    std::this_thread::sleep_for(100ms);
  };
  send(3, 42);
  CHECK(ossia::convert<int>(a.value()) == 42);
  send(4, 99);
  CHECK(ossia::convert<int>(a.value()) == 42);
  send(3 + 256, 98);
  CHECK(ossia::convert<int>(a.value()) == 42);
}

TEST_CASE("sACN input reads every slot, and short packets", "[dmx][e131]")
{
  net_thread net;
  const uint16_t port = free_port();
  dmx_config conf;
  conf.mode = dmx_config::sink;
  conf.start_universe = 1;
  conf.multicast = false;
  auto proto = std::make_unique<e131_input_protocol>(
      net.ctx, conf, inbound_socket_configuration{"127.0.0.1", port});
  auto& p = *proto;
  generic_device dev{std::move(proto), "dmx"};
  auto& low = add_channel(dev, p.buffer(), "low", 2);
  auto& high = add_channel(dev, p.buffer(), "high", 400);
  low.set_value(0);
  high.set_value(0);
  p.create_channel_map();

  auto send = [&](std::vector<uint8_t> slots, uint8_t start_code = 0) {
    std::vector<uint8_t> pkt(126, 0);
    pkt[1] = 0x10;
    std::memcpy(pkt.data() + 4, "ASC-E1.17", 9);
    pkt[113] = 0;
    pkt[114] = 1;
    const int count = slots.size() + 1;
    pkt[123] = count >> 8;
    pkt[124] = count & 0xFF;
    pkt[125] = start_code;
    pkt.insert(pkt.end(), slots.begin(), slots.end());
    boost::asio::io_context io;
    udp::socket s{io, udp::v4()};
    s.send_to(boost::asio::buffer(pkt), {boost::asio::ip::make_address("127.0.0.1"), port});
    std::this_thread::sleep_for(100ms);
  };

  std::vector<uint8_t> full(512);
  full[2] = 10;
  full[400] = 20;
  send(full);
  CHECK(ossia::convert<int>(low.value()) == 10);
  CHECK(ossia::convert<int>(high.value()) == 20);

  send({0, 0, 11});
  CHECK(ossia::convert<int>(low.value()) == 11);
  CHECK(ossia::convert<int>(high.value()) == 20);

  send({0, 0, 12}, 0xCC);
  CHECK(ossia::convert<int>(low.value()) == 11);
}

#if defined(DMXTEST_HAS_PTY)
namespace
{
//! A pseudo-terminal standing in for the serial port of a DMX interface.
struct fake_serial_port
{
  int master{-1};
  int slave{-1};
  std::string name;

  fake_serial_port()
  {
    char path[256]{};
    REQUIRE(openpty(&master, &slave, path, nullptr, nullptr) == 0);
    name = path;
    termios t{};
    tcgetattr(master, &t);
    cfmakeraw(&t);
    tcsetattr(master, TCSANOW, &t);
    fcntl(master, F_SETFL, fcntl(master, F_GETFL) | O_NONBLOCK);
  }
  ~fake_serial_port()
  {
    close(slave);
    close(master);
  }

  std::vector<uint8_t> read_for(std::chrono::milliseconds d)
  {
    std::vector<uint8_t> res;
    const auto end = std::chrono::steady_clock::now() + d;
    std::array<uint8_t, 4096> buf;
    while(std::chrono::steady_clock::now() < end)
    {
      const auto n = ::read(master, buf.data(), buf.size());
      if(n > 0)
        res.insert(res.end(), buf.begin(), buf.begin() + n);
      else
        std::this_thread::sleep_for(2ms);
    }
    return res;
  }
};

//! The Enttec messages in a byte stream: 0x7E, label, length LSB / MSB, data, 0xE7.
std::vector<std::pair<int, std::vector<uint8_t>>> enttec_messages(const std::vector<uint8_t>& bytes)
{
  std::vector<std::pair<int, std::vector<uint8_t>>> res;
  std::size_t i = 0;
  while(i + 4 < bytes.size())
  {
    if(bytes[i] != 0x7E)
    {
      i++;
      continue;
    }
    const std::size_t len = bytes[i + 2] | (bytes[i + 3] << 8);
    if(i + 4 + len >= bytes.size())
      break;
    if(bytes[i + 4 + len] == 0xE7)
      res.emplace_back(
          bytes[i + 1], std::vector<uint8_t>(
                            bytes.begin() + i + 4, bytes.begin() + i + 4 + len));
    i += 5 + len;
  }
  return res;
}
}

TEST_CASE("A DMX USB Pro Mk2 sends universe 0 and 1 out of its two ports", "[dmx][dmxusbpro]")
{
  for(int start : {0, 1})
  {
    fake_serial_port tty;
    net_thread net;
    dmx_config conf;
    conf.frequency = 40;
    conf.start_universe = start;
    serial_configuration serial;
    serial.port = tty.name;
    auto proto = std::make_unique<dmxusbpro_protocol>(
        net.ctx, conf, serial, dmxusbpro_protocol::dmx_usb_pro_mk2);
    auto& p = *proto;
    p.buffer().set_universe_count(2);
    generic_device dev{std::move(proto), "dmx"};
    add_channel(dev, p.buffer(), "a", 3).push_value(7);
    add_channel(dev, p.buffer(), "b", 512 + 3).push_value(9);

    std::map<int, int> last_ch4;
    for(auto& [label, data] : enttec_messages(tty.read_for(200ms)))
    {
      if(label != 0x06 && label != 0xCA)
        continue;
      REQUIRE(data.size() == 513);
      CHECK(data[0] == 0);
      last_ch4[label] = data[1 + 3];
    }

    if(start == 0)
    {
      // Universe 0 on the first port, universe 1 on the second.
      CHECK(last_ch4[0x06] == 7);
      CHECK(last_ch4[0xCA] == 9);
    }
    else
    {
      // Universe 1 is the start universe: buffer universe 0 goes to the second port.
      CHECK(!last_ch4.contains(0x06));
      CHECK(last_ch4[0xCA] == 7);
    }
  }
}

TEST_CASE("A DMX USB Pro sends a single universe", "[dmx][dmxusbpro]")
{
  fake_serial_port tty;
  net_thread net;
  dmx_config conf;
  conf.frequency = 40;
  conf.channels_per_universe = 100;
  serial_configuration serial;
  serial.port = tty.name;
  auto proto = std::make_unique<dmxusbpro_protocol>(
      net.ctx, conf, serial, dmxusbpro_protocol::dmx_usb_pro_mk1);
  auto& p = *proto;
  p.buffer().set_universe_count(2);
  generic_device dev{std::move(proto), "dmx"};
  add_channel(dev, p.buffer(), "a", 99).push_value(7);
  add_channel(dev, p.buffer(), "b", 100).push_value(9);

  int frames = 0;
  for(auto& [label, data] : enttec_messages(tty.read_for(200ms)))
  {
    CHECK(label == 0x06);
    REQUIRE(data.size() == 513);
    CHECK(data[1 + 99] == 7);
    // The channels past the universe's size are not the next universe's.
    CHECK(data[1 + 100] == 0);
    frames++;
  }
  CHECK(frames > 0);
}
#endif
