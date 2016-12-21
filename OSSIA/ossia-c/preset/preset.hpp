#pragma once

#include <string>
#include <hopscotch_map.h>
#include <memory>
#include <regex>
#include <functional>
#include <ossia-c/preset/exception.hpp>
#include <ossia_export.h>

namespace ossia
{
namespace net
{
class device_base;
class node_base;
class address_base;
}
class value;
}

namespace ossia
{
namespace presets
{
using preset =
  tsl::hopscotch_map<
    std::string,
    ossia::value>;

using instance_functions =
  std::vector<
    std::pair<
      std::regex,
      std::function<void(const ossia::net::node_base&)>
    >
  >;

using preset_pair = std::pair<std::string, ossia::value>;

OSSIA_EXPORT preset read_json(const std::string&);
OSSIA_EXPORT preset read_xml(const std::string&);

OSSIA_EXPORT std::string write_json(const preset&);
OSSIA_EXPORT std::string write_xml(const preset&);

OSSIA_EXPORT std::string to_string(const preset&);
OSSIA_EXPORT std::string to_string(const preset_pair&);
}

namespace devices
{

enum keep_arch_type {keep_arch_on, keep_arch_off};

OSSIA_EXPORT void read_json(ossia::net::device_base&, const std::string&);
OSSIA_EXPORT void read_xml(ossia::net::device_base&, const std::string&);

OSSIA_EXPORT std::string write_json(const ossia::net::device_base&);
OSSIA_EXPORT std::string write_xml(const ossia::net::device_base&);

OSSIA_EXPORT void apply_preset(
      ossia::net::device_base&,
      const presets::preset&,
      keep_arch_type t = keep_arch_on,
      presets::instance_functions = {});

OSSIA_EXPORT presets::preset make_preset(ossia::net::device_base&);

OSSIA_EXPORT ossia::net::node_base* get_node(ossia::net::node_base&, const std::string&);
OSSIA_EXPORT std::string to_string(const ossia::net::device_base& ossiadev);
}
}
