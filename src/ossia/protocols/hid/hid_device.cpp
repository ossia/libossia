#include "hid_device.hpp"

#include <ossia/detail/fmt.hpp>

#include <SDL3/SDL.h>
#include <SDL3/SDL_hidapi.h>

#include <cstdio>
#include <fstream>
#include <iterator>
#include <stdexcept>

namespace ossia::net
{
namespace
{
bool hid_initialized()
{
  static const bool ok = SDL_hid_init() == 0;
  return ok;
}

std::string to_utf8(const wchar_t* str)
{
  std::string res;
  if(!str)
    return res;

  auto append = [&res](char32_t c) {
    if(c < 0x80)
    {
      res += char(c);
    }
    else if(c < 0x800)
    {
      res += char(0xC0 | (c >> 6));
      res += char(0x80 | (c & 0x3F));
    }
    else if(c < 0x10000)
    {
      res += char(0xE0 | (c >> 12));
      res += char(0x80 | ((c >> 6) & 0x3F));
      res += char(0x80 | (c & 0x3F));
    }
    else
    {
      res += char(0xF0 | (c >> 18));
      res += char(0x80 | ((c >> 12) & 0x3F));
      res += char(0x80 | ((c >> 6) & 0x3F));
      res += char(0x80 | (c & 0x3F));
    }
  };

  for(; *str; ++str)
  {
    char32_t c = static_cast<char32_t>(*str);
    if constexpr(sizeof(wchar_t) == 2)
    {
      if(c >= 0xD800 && c < 0xDC00 && str[1] >= 0xDC00 && str[1] < 0xE000)
      {
        c = 0x10000 + ((c - 0xD800) << 10) + (static_cast<char32_t>(str[1]) - 0xDC00);
        ++str;
      }
    }
    append(c);
  }
  return res;
}

hid_device_info to_info(const SDL_hid_device_info& d)
{
  hid_device_info info;
  info.path = d.path ? d.path : "";
  info.serial = to_utf8(d.serial_number);
  info.manufacturer = to_utf8(d.manufacturer_string);
  info.product = to_utf8(d.product_string);
  info.vendor_id = d.vendor_id;
  info.product_id = d.product_id;
  info.release = d.release_number;
  info.usage_page = d.usage_page;
  info.usage = d.usage;
  info.interface_number = d.interface_number;
  return info;
}

std::string sdl_error()
{
  const char* err = SDL_GetError();
  return err && *err ? err : "unknown error";
}
}

std::string hid_device_info::display_name() const
{
  std::string name = product.empty() ? fmt::format("{:04x}:{:04x}", vendor_id, product_id)
                                     : product;
  if(!serial.empty())
    name += fmt::format(" ({})", serial);
  if(interface_number > 0)
    name += fmt::format(" #{}", interface_number);
  return name;
}

std::vector<hid_device_info> hid_enumerate(uint16_t vendor_id, uint16_t product_id)
{
  std::vector<hid_device_info> res;
  if(!hid_initialized())
    return res;

  auto* list = SDL_hid_enumerate(vendor_id, product_id);
  for(auto* d = list; d; d = d->next)
    res.push_back(to_info(*d));
  SDL_hid_free_enumeration(list);
  return res;
}

uint32_t hid_device_change_count()
{
  if(!hid_initialized())
    return 0;
  return SDL_hid_device_change_count();
}

std::vector<uint8_t> hid_read_report_descriptor(std::string_view path)
{
#if defined(__linux__)
  constexpr std::string_view prefix = "/dev/";
  if(!path.starts_with(prefix))
    return {};
  const auto node = path.substr(prefix.size());
  if(node.find('/') != std::string_view::npos)
    return {};

  std::ifstream f{
      fmt::format("/sys/class/hidraw/{}/device/report_descriptor", node),
      std::ios::binary};
  if(!f)
    return {};
  return {std::istreambuf_iterator<char>{f}, std::istreambuf_iterator<char>{}};
#else
  return {};
#endif
}

std::optional<hid_device_info> hid_find_device(const hid_device_configuration& conf)
{
  const auto devices = hid_enumerate(conf.vendor_id, conf.product_id);
  if(!conf.path.empty())
  {
    for(const auto& d : devices)
      if(d.path == conf.path
         && (conf.usage_page == 0 || d.usage_page == 0 || d.usage_page == conf.usage_page))
        return d;
  }

  for(const auto& d : devices)
  {
    if(conf.vendor_id == 0 && conf.product_id == 0 && conf.serial.empty())
      break;
    if(!conf.serial.empty() && d.serial != conf.serial)
      continue;
    if(conf.interface_number >= 0 && d.interface_number != conf.interface_number)
      continue;
    if(conf.usage_page != 0 && d.usage_page != 0 && d.usage_page != conf.usage_page)
      continue;
    return d;
  }
  return std::nullopt;
}

hid_device::hid_device() = default;

hid_device::~hid_device()
{
  close();
}

void hid_device::open(const hid_device_configuration& conf)
{
  close();
  if(!hid_initialized())
    throw std::runtime_error("HID: could not initialize HID support");

  auto info = hid_find_device(conf);
  if(!info)
    throw std::runtime_error(fmt::format(
        "HID: no device {:04x}:{:04x} {}", conf.vendor_id, conf.product_id, conf.serial));

  auto* h = SDL_hid_open_path(info->path.c_str());
  if(!h)
    throw std::runtime_error(
        fmt::format("HID: could not open {}: {}", info->path, sdl_error()));

  m_handle = h;
  m_info = std::move(*info);
}

void hid_device::start(
    input_callback on_input, error_callback on_error, std::size_t max_report_size)
{
  if(!m_handle || m_running)
    return;

  m_on_input = std::move(on_input);
  m_on_error = std::move(on_error);
  m_running = true;
  m_reader = std::thread{[this, max_report_size] { reader_loop(max_report_size); }};
}

void hid_device::reader_loop(std::size_t max_report_size)
{
  auto* h = static_cast<SDL_hid_device*>(m_handle);
  std::vector<uint8_t> buf(max_report_size);
  while(m_running)
  {
    const int r = SDL_hid_read_timeout(h, buf.data(), buf.size(), 100);
    if(r > 0)
    {
      if(m_on_input)
        m_on_input(std::span<const uint8_t>{buf.data(), std::size_t(r)});
    }
    else if(r < 0)
    {
      if(m_running && m_on_error)
        m_on_error(sdl_error());
      break;
    }
  }
}

void hid_device::close()
{
  m_running = false;
  if(m_reader.joinable())
  {
    if(m_reader.get_id() == std::this_thread::get_id())
      m_reader.detach();
    else
      m_reader.join();
  }

  std::lock_guard lock{m_io_mutex};
  if(m_handle)
  {
    SDL_hid_close(static_cast<SDL_hid_device*>(m_handle));
    m_handle = nullptr;
  }
}

int hid_device::write(std::span<const uint8_t> report)
{
  std::lock_guard lock{m_io_mutex};
  if(!m_handle || report.empty())
    return -1;
  return SDL_hid_write(static_cast<SDL_hid_device*>(m_handle), report.data(), report.size());
}

int hid_device::send_feature_report(std::span<const uint8_t> report)
{
  std::lock_guard lock{m_io_mutex};
  if(!m_handle || report.empty())
    return -1;
  return SDL_hid_send_feature_report(
      static_cast<SDL_hid_device*>(m_handle), report.data(), report.size());
}

int hid_device::get_feature_report(std::span<uint8_t> report)
{
  std::lock_guard lock{m_io_mutex};
  if(!m_handle || report.empty())
    return -1;
  return SDL_hid_get_feature_report(
      static_cast<SDL_hid_device*>(m_handle), report.data(), report.size());
}

std::string hid_device::last_error() const
{
  return sdl_error();
}

}
