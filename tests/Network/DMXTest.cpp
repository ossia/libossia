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

#include "include_catch.hpp"

#include <array>
#include <limits>

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
