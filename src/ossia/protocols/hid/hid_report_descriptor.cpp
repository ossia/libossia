#include "hid_report_descriptor.hpp"

#include <ossia/detail/fmt.hpp>

#include <algorithm>
#include <limits>
#include <map>

namespace ossia::net::hid
{
namespace
{
struct global_state
{
  uint16_t usage_page{};
  int32_t logical_minimum{};
  int32_t logical_maximum_signed{};
  uint32_t logical_maximum_unsigned{};
  uint32_t report_size{};
  uint32_t report_count{};
  uint8_t report_id{};
};

struct local_usage
{
  uint32_t value{};
  bool extended{};
};

struct local_state
{
  std::vector<local_usage> usages;
  local_usage usage_minimum;
  local_usage usage_maximum;
  bool has_minimum{};
  bool has_maximum{};

  void clear() { *this = {}; }
};

uint32_t resolve(local_usage u, uint16_t page) noexcept
{
  return u.extended ? u.value : (uint32_t(page) << 16) | (u.value & 0xFFFF);
}

constexpr uint32_t max_expanded_usages = 4096;
}

bool report_descriptor::uses_report_ids() const noexcept
{
  return std::any_of(
      fields.begin(), fields.end(), [](const report_field& f) { return f.report_id != 0; });
}

std::size_t report_descriptor::report_size(report_type t, uint8_t id) const noexcept
{
  uint32_t bits = 0;
  for(const auto& f : fields)
    if(f.type == t && f.report_id == id)
      bits = std::max(bits, f.bit_offset + f.report_size * f.report_count);
  return (bits + 7) / 8;
}

std::vector<uint8_t> report_descriptor::report_ids(report_type t) const
{
  std::vector<uint8_t> ids;
  for(const auto& f : fields)
    if(f.type == t && std::find(ids.begin(), ids.end(), f.report_id) == ids.end())
      ids.push_back(f.report_id);
  return ids;
}

report_descriptor parse_report_descriptor(std::span<const uint8_t> data)
{
  report_descriptor desc;
  global_state global;
  std::vector<global_state> global_stack;
  local_state local;
  std::vector<uint32_t> collections;
  std::map<std::pair<report_type, uint8_t>, uint32_t> offsets;

  std::size_t i = 0;
  while(i < data.size())
  {
    const uint8_t prefix = data[i];
    if(prefix == 0xFE)
    {
      if(i + 1 >= data.size())
        break;
      i += 3 + data[i + 1];
      continue;
    }

    static constexpr uint8_t sizes[4] = {0, 1, 2, 4};
    const uint8_t size = sizes[prefix & 0x3];
    const uint8_t type = (prefix >> 2) & 0x3;
    const uint8_t tag = prefix >> 4;
    if(i + 1 + size > data.size())
      break;

    uint32_t u = 0;
    for(uint8_t b = 0; b < size; b++)
      u |= uint32_t(data[i + 1 + b]) << (8 * b);

    int32_t s = 0;
    switch(size)
    {
      case 1:
        s = int8_t(u);
        break;
      case 2:
        s = int16_t(u);
        break;
      case 4:
        s = int32_t(u);
        break;
    }

    i += 1 + size;

    switch(type)
    {
      case 0: {
        switch(tag)
        {
          case 0x8:
          case 0x9:
          case 0xB: {
            report_field f;
            f.type = tag == 0x8   ? report_type::input
                     : tag == 0x9 ? report_type::output
                                  : report_type::feature;
            f.report_id = global.report_id;
            f.report_size = global.report_size;
            f.report_count = global.report_count;
            f.flags = u;
            f.logical_minimum = global.logical_minimum;
            f.logical_maximum
                = global.logical_minimum < 0
                      ? global.logical_maximum_signed
                      : int32_t(std::min<uint32_t>(
                          global.logical_maximum_unsigned,
                          std::numeric_limits<int32_t>::max()));

            for(auto usage : local.usages)
              f.usages.push_back(resolve(usage, global.usage_page));
            if(local.has_minimum && local.has_maximum)
            {
              const uint32_t lo = resolve(local.usage_minimum, global.usage_page);
              const uint32_t hi = resolve(local.usage_maximum, global.usage_page);
              for(uint32_t usage = lo; usage <= hi && usage - lo < max_expanded_usages;
                  usage++)
                f.usages.push_back(usage);
            }
            f.collections = collections;

            auto& offset = offsets[{f.type, f.report_id}];
            f.bit_offset = offset;
            offset += f.report_size * f.report_count;

            if(f.report_size > 0 && f.report_count > 0)
              desc.fields.push_back(std::move(f));
            break;
          }
          case 0xA: {
            uint32_t usage = 0;
            if(!local.usages.empty())
              usage = resolve(local.usages.front(), global.usage_page);
            else if(local.has_minimum)
              usage = resolve(local.usage_minimum, global.usage_page);
            collections.push_back(usage);
            break;
          }
          case 0xC:
            if(!collections.empty())
              collections.pop_back();
            break;
        }
        local.clear();
        break;
      }

      case 1: {
        switch(tag)
        {
          case 0x0:
            global.usage_page = uint16_t(u);
            break;
          case 0x1:
            global.logical_minimum = s;
            break;
          case 0x2:
            global.logical_maximum_signed = s;
            global.logical_maximum_unsigned = u;
            break;
          case 0x7:
            global.report_size = u;
            break;
          case 0x8:
            global.report_id = uint8_t(u);
            break;
          case 0x9:
            global.report_count = u;
            break;
          case 0xA:
            global_stack.push_back(global);
            break;
          case 0xB:
            if(!global_stack.empty())
            {
              global = global_stack.back();
              global_stack.pop_back();
            }
            break;
        }
        break;
      }

      case 2: {
        const local_usage usage{u, size == 4};
        switch(tag)
        {
          case 0x0:
            local.usages.push_back(usage);
            break;
          case 0x1:
            local.usage_minimum = usage;
            local.has_minimum = true;
            break;
          case 0x2:
            local.usage_maximum = usage;
            local.has_maximum = true;
            break;
        }
        break;
      }
    }
  }

  return desc;
}

int32_t extract_field(
    const report_field& f, uint32_t index, std::span<const uint8_t> data) noexcept
{
  const uint32_t size = std::min<uint32_t>(f.report_size, 32);
  const uint64_t start = uint64_t(f.bit_offset) + uint64_t(index) * f.report_size;

  uint32_t value = 0;
  for(uint32_t b = 0; b < size; b++)
  {
    const uint64_t bit = start + b;
    const uint64_t byte = bit / 8;
    if(byte >= data.size())
      break;
    if(data[byte] & (1u << (bit % 8)))
      value |= 1u << b;
  }

  if(f.is_signed() && size > 0 && size < 32 && (value & (1u << (size - 1))))
    value |= ~((1u << size) - 1);

  return int32_t(value);
}

void insert_field(
    const report_field& f, uint32_t index, int32_t value, std::span<uint8_t> data) noexcept
{
  const uint32_t size = std::min<uint32_t>(f.report_size, 32);
  const uint64_t start = uint64_t(f.bit_offset) + uint64_t(index) * f.report_size;
  const uint32_t v = uint32_t(value);

  for(uint32_t b = 0; b < size; b++)
  {
    const uint64_t bit = start + b;
    const uint64_t byte = bit / 8;
    if(byte >= data.size())
      break;
    const uint8_t mask = uint8_t(1u << (bit % 8));
    if(v & (1u << b))
      data[byte] |= mask;
    else
      data[byte] &= ~mask;
  }
}

std::string usage_page_name(uint16_t page)
{
  switch(page)
  {
    case 0x01:
      return "generic_desktop";
    case 0x02:
      return "simulation";
    case 0x05:
      return "game";
    case 0x07:
      return "key";
    case 0x08:
      return "led";
    case 0x09:
      return "button";
    case 0x0A:
      return "ordinal";
    case 0x0C:
      return "consumer";
    case 0x0D:
      return "digitizer";
    case 0x0F:
      return "haptics";
    case 0x14:
      return "auxiliary_display";
    case 0x20:
      return "sensor";
    case 0x59:
      return "lighting";
    default:
      if(page >= 0xFF00)
        return fmt::format("vendor_{:04x}", page);
      return fmt::format("page_{:04x}", page);
  }
}

namespace
{
const char* generic_desktop_name(uint16_t id)
{
  switch(id)
  {
    case 0x01: return "pointer";
    case 0x02: return "mouse";
    case 0x04: return "joystick";
    case 0x05: return "gamepad";
    case 0x06: return "keyboard";
    case 0x07: return "keypad";
    case 0x08: return "multi_axis";
    case 0x30: return "x";
    case 0x31: return "y";
    case 0x32: return "z";
    case 0x33: return "rx";
    case 0x34: return "ry";
    case 0x35: return "rz";
    case 0x36: return "slider";
    case 0x37: return "dial";
    case 0x38: return "wheel";
    case 0x39: return "hat";
    case 0x3D: return "start";
    case 0x3E: return "select";
    case 0x40: return "vx";
    case 0x41: return "vy";
    case 0x42: return "vz";
    case 0x80: return "system_control";
    case 0x81: return "power_down";
    case 0x82: return "sleep";
    case 0x83: return "wake_up";
    case 0x90: return "dpad_up";
    case 0x91: return "dpad_down";
    case 0x92: return "dpad_right";
    case 0x93: return "dpad_left";
    default: return nullptr;
  }
}

const char* simulation_name(uint16_t id)
{
  switch(id)
  {
    case 0xBA: return "rudder";
    case 0xBB: return "throttle";
    case 0xC4: return "accelerator";
    case 0xC5: return "brake";
    case 0xC8: return "steering";
    default: return nullptr;
  }
}

std::string keyboard_name(uint16_t id)
{
  if(id >= 0x04 && id <= 0x1D)
    return std::string(1, char('a' + (id - 0x04)));
  if(id >= 0x1E && id <= 0x26)
    return std::string(1, char('1' + (id - 0x1E)));
  if(id >= 0x3A && id <= 0x45)
    return fmt::format("f{}", id - 0x3A + 1);
  switch(id)
  {
    case 0x27: return "0";
    case 0x28: return "enter";
    case 0x29: return "escape";
    case 0x2A: return "backspace";
    case 0x2B: return "tab";
    case 0x2C: return "space";
    case 0x39: return "caps_lock";
    case 0x4F: return "right";
    case 0x50: return "left";
    case 0x51: return "down";
    case 0x52: return "up";
    case 0xE0: return "left_control";
    case 0xE1: return "left_shift";
    case 0xE2: return "left_alt";
    case 0xE3: return "left_gui";
    case 0xE4: return "right_control";
    case 0xE5: return "right_shift";
    case 0xE6: return "right_alt";
    case 0xE7: return "right_gui";
    default: return fmt::format("usage_{:02x}", id);
  }
}

const char* led_name(uint16_t id)
{
  switch(id)
  {
    case 0x01: return "num_lock";
    case 0x02: return "caps_lock";
    case 0x03: return "scroll_lock";
    case 0x04: return "compose";
    case 0x05: return "kana";
    default: return nullptr;
  }
}

const char* consumer_name(uint16_t id)
{
  switch(id)
  {
    case 0x01: return "consumer_control";
    case 0x6F: return "brightness_up";
    case 0x70: return "brightness_down";
    case 0xB0: return "play";
    case 0xB1: return "pause";
    case 0xB2: return "record";
    case 0xB3: return "fast_forward";
    case 0xB4: return "rewind";
    case 0xB5: return "next_track";
    case 0xB6: return "previous_track";
    case 0xB7: return "stop";
    case 0xCD: return "play_pause";
    case 0xE0: return "volume";
    case 0xE2: return "mute";
    case 0xE9: return "volume_up";
    case 0xEA: return "volume_down";
    default: return nullptr;
  }
}

const char* digitizer_name(uint16_t id)
{
  switch(id)
  {
    case 0x01: return "digitizer";
    case 0x02: return "pen";
    case 0x04: return "touch_screen";
    case 0x05: return "touch_pad";
    case 0x22: return "finger";
    case 0x30: return "tip_pressure";
    case 0x32: return "in_range";
    case 0x42: return "tip_switch";
    case 0x48: return "width";
    case 0x49: return "height";
    case 0x51: return "contact_id";
    case 0x54: return "contact_count";
    default: return nullptr;
  }
}
}

std::string usage_name(uint32_t usage)
{
  const uint16_t page = uint16_t(usage >> 16);
  const uint16_t id = uint16_t(usage & 0xFFFF);

  const char* known = nullptr;
  switch(page)
  {
    case 0x01:
      known = generic_desktop_name(id);
      break;
    case 0x02:
      known = simulation_name(id);
      break;
    case 0x07:
      return "key/" + keyboard_name(id);
    case 0x08:
      known = led_name(id);
      break;
    case 0x09:
      return fmt::format("button/{}", id);
    case 0x0A:
      return fmt::format("instance/{}", id);
    case 0x0C:
      known = consumer_name(id);
      break;
    case 0x0D:
      known = digitizer_name(id);
      break;
  }

  if(known)
    return known;
  return fmt::format("{}/usage_{:04x}", usage_page_name(page), id);
}
}
