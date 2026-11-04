#include <ossia/protocols/hid/hid_report_descriptor.hpp>

#include "include_catch.hpp"

#include <algorithm>

using namespace ossia::net::hid;

// Report descriptors read from real devices.
static const uint8_t keyboard[] = {
    0x05, 0x01, 0x09, 0x06, 0xa1, 0x01, 0x05, 0x07, 0x19, 0xe0, 0x29, 0xe7, 0x15, 0x00,
    0x25, 0x01, 0x75, 0x01, 0x95, 0x08, 0x81, 0x02, 0x95, 0x01, 0x75, 0x08, 0x81, 0x01,
    0x95, 0x03, 0x75, 0x01, 0x05, 0x08, 0x19, 0x01, 0x29, 0x03, 0x91, 0x02, 0x95, 0x05,
    0x75, 0x01, 0x91, 0x01, 0x95, 0x06, 0x75, 0x08, 0x05, 0x07, 0x19, 0x00, 0x2a, 0xff,
    0x00, 0x15, 0x00, 0x26, 0xff, 0x00, 0x81, 0x00, 0xc0};

static const uint8_t mouse[] = {
    0x05, 0x01, 0x09, 0x02, 0xa1, 0x01, 0x09, 0x01, 0xa1, 0x00, 0x05, 0x09, 0x19, 0x01,
    0x29, 0x08, 0x15, 0x00, 0x25, 0x01, 0x95, 0x08, 0x75, 0x01, 0x81, 0x02, 0x06, 0x00,
    0xff, 0x09, 0x40, 0x95, 0x02, 0x75, 0x08, 0x15, 0x81, 0x25, 0x7f, 0x81, 0x02, 0x05,
    0x01, 0x09, 0x38, 0x95, 0x01, 0x81, 0x06, 0x09, 0x30, 0x09, 0x31, 0x16, 0x01, 0x80,
    0x26, 0xff, 0x7f, 0x75, 0x10, 0x95, 0x02, 0x81, 0x06, 0xc0, 0xc0};

static const uint8_t stream_deck_mk2[] = {
    0x05, 0x0c, 0x09, 0x01, 0xa1, 0x01, 0x0a, 0x00, 0xff, 0x15, 0x00, 0x26, 0xff, 0x00,
    0x75, 0x08, 0x96, 0xff, 0x03, 0x85, 0x02, 0x91, 0x02, 0x0a, 0x00, 0xff, 0x15, 0x00,
    0x26, 0xff, 0x00, 0x75, 0x08, 0x96, 0xff, 0x01, 0x85, 0x01, 0x81, 0x02, 0x0a, 0x00,
    0xff, 0x15, 0x00, 0x26, 0xff, 0x00, 0x75, 0x08, 0x96, 0x1f, 0x00, 0x85, 0x03, 0xb1,
    0x04, 0x0a, 0x00, 0xff, 0x15, 0x00, 0x26, 0xff, 0x00, 0x75, 0x08, 0x96, 0x1f, 0x00,
    0x85, 0x07, 0xb1, 0x04, 0xc0};

static const report_field&
find_field(const report_descriptor& d, report_type t, uint32_t usage)
{
  auto it = std::find_if(d.fields.begin(), d.fields.end(), [&](const report_field& f) {
    return f.type == t && std::find(f.usages.begin(), f.usages.end(), usage) != f.usages.end();
  });
  REQUIRE(it != d.fields.end());
  return *it;
}

TEST_CASE("keyboard descriptor", "[hid]")
{
  const auto d = parse_report_descriptor(keyboard);
  REQUIRE(!d.uses_report_ids());
  REQUIRE(d.report_size(report_type::input, 0) == 8);
  REQUIRE(d.report_size(report_type::output, 0) == 1);

  const auto& modifiers = find_field(d, report_type::input, 0x000700E0);
  REQUIRE(modifiers.is_variable());
  REQUIRE(modifiers.report_size == 1);
  REQUIRE(modifiers.report_count == 8);
  REQUIRE(modifiers.bit_offset == 0);
  REQUIRE(usage_name(modifiers.usage_for(1)) == "key/left_shift");

  const auto& keys = find_field(d, report_type::input, 0x00070004);
  REQUIRE(!keys.is_variable());
  REQUIRE(keys.bit_offset == 16);
  REQUIRE(keys.report_count == 6);
  REQUIRE(keys.usages.size() == 256);

  const auto& leds = find_field(d, report_type::output, 0x00080002);
  REQUIRE(leds.report_count == 3);
  REQUIRE(usage_name(leds.usage_for(1)) == "caps_lock");

  const uint8_t report[8] = {0x02, 0, 0x04, 0x1E, 0, 0, 0, 0};
  REQUIRE(extract_field(modifiers, 0, report) == 0);
  REQUIRE(extract_field(modifiers, 1, report) == 1);
  REQUIRE(extract_field(keys, 0, report) == 0x04);
  REQUIRE(usage_name(keys.usages[extract_field(keys, 0, report)]) == "key/a");
  REQUIRE(usage_name(keys.usages[extract_field(keys, 1, report)]) == "key/1");
  REQUIRE(usage_name(0x00070001) != "key/1");

  uint8_t out[1] = {};
  insert_field(leds, 1, 1, out);
  REQUIRE(out[0] == 0x02);
  insert_field(leds, 1, 0, out);
  REQUIRE(out[0] == 0x00);
}

TEST_CASE("mouse descriptor", "[hid]")
{
  const auto d = parse_report_descriptor(mouse);
  REQUIRE(!d.uses_report_ids());

  const auto& axes = find_field(d, report_type::input, 0x00010030);
  REQUIRE(axes.is_relative());
  REQUIRE(axes.is_signed());
  REQUIRE(axes.report_size == 16);
  REQUIRE(axes.bit_offset == 32);
  REQUIRE(axes.logical_minimum == -32767);
  REQUIRE(axes.logical_maximum == 32767);
  REQUIRE(usage_name(axes.usage_for(1)) == "y");

  const auto& wheel = find_field(d, report_type::input, 0x00010038);
  REQUIRE(wheel.logical_minimum == -127);

  const uint8_t report[8] = {0x05, 0, 0, 0xFF, 0xFB, 0xFF, 0x10, 0x00};
  REQUIRE(extract_field(axes, 0, report) == -5);
  REQUIRE(extract_field(axes, 1, report) == 16);
  REQUIRE(extract_field(wheel, 0, report) == -1);

  const auto& buttons = find_field(d, report_type::input, 0x00090001);
  REQUIRE(usage_name(buttons.usage_for(2)) == "button/3");
  REQUIRE(extract_field(buttons, 0, report) == 1);
  REQUIRE(extract_field(buttons, 1, report) == 0);
  REQUIRE(extract_field(buttons, 2, report) == 1);
}

TEST_CASE("vendor descriptor with report ids", "[hid]")
{
  const auto d = parse_report_descriptor(stream_deck_mk2);
  REQUIRE(d.uses_report_ids());
  REQUIRE(d.report_size(report_type::input, 1) == 511);
  REQUIRE(d.report_size(report_type::output, 2) == 1023);
  REQUIRE(d.report_size(report_type::feature, 3) == 31);
  REQUIRE(d.report_ids(report_type::feature) == std::vector<uint8_t>{3, 7});
  REQUIRE(d.fields.front().collections == std::vector<uint32_t>{0x000C0001});
  REQUIRE(usage_name(0xFF000001) == "vendor_ff00/usage_0001");
  REQUIRE(usage_name(0x000C00E9) == "volume_up");
}

TEST_CASE("truncated descriptors do not overrun", "[hid]")
{
  for(std::size_t n = 0; n < sizeof(mouse); n++)
    parse_report_descriptor(std::span<const uint8_t>{mouse, n});

  const uint8_t long_item[] = {0xFE, 0x10, 0x00};
  REQUIRE(parse_report_descriptor(long_item).fields.empty());
}
