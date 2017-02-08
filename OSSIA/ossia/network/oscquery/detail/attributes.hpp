#pragma once
#include <ossia/network/base/node.hpp>
#include <ossia/network/base/address.hpp>
#include <ossia/network/base/node_attributes.hpp>
#include <ossia/network/common/address_properties.hpp>
#include <ossia/network/oscquery/detail/typetag.hpp>

#include <ossia/detail/string_view.hpp>
#include <brigand/sequences/list.hpp>

namespace ossia
{
namespace oscquery
{

namespace detail
{
// Attributes
constexpr auto osc_port() { return "OSC_PORT"; }
constexpr auto attribute_full_path() { return "FULL_PATH"; }

constexpr auto attribute_typetag() { return "TYPE"; }
constexpr auto contents() { return "CONTENTS"; }

constexpr auto attribute_value() { return "VALUE"; }
constexpr auto attribute_range() { return "RANGE"; }
constexpr auto attribute_clipmode() { return "CLIPMODE"; }
constexpr auto attribute_accessmode() { return "ACCESS"; }
constexpr auto attribute_description() { return "DESCRIPTION"; }
constexpr auto attribute_tags() { return "TAGS"; }

// Not part of the OSCQuery spec :
constexpr auto attribute_unit() { return "UNIT"; }
constexpr auto attribute_refresh_rate() { return "REFRESH_RATE"; }
constexpr auto attribute_priority() { return "PRIORITY"; }
constexpr auto attribute_step_size() { return "STEP_SIZE"; }
constexpr auto attribute_instance_bounds() { return "INSTANCE_BOUNDS"; }
constexpr auto attribute_critical() { return "CRITICAL"; }
constexpr auto attribute_extended_type() { return "EXTENDED_TYPE"; }
constexpr auto attribute_repetition_filter() { return "REPETITION_FILTER"; }
constexpr auto attribute_app_name() { return "APP_NAME"; }
constexpr auto attribute_app_version() { return "APP_VERSION"; }
constexpr auto attribute_app_creator() { return "APP_CREATOR"; }
constexpr auto attribute_default_value() { return "DEFAULT_VALUE"; }


// Commands
constexpr auto command() { return "COMMAND"; }
constexpr auto data() { return "DATA"; }

constexpr auto path_added() { return "PATH_ADDED"; }
constexpr auto path_removed() { return "PATH_REMOVED"; }
constexpr auto path_changed() { return "PATH_CHANGED"; }
constexpr auto attributes_changed() { return "ATTRIBUTES_CHANGED"; }

constexpr auto paths_added() { return "PATHS_ADDED"; }
constexpr auto paths_removed() { return "PATHS_REMOVED"; }
constexpr auto paths_changed() { return "PATHS_CHANGED"; }
constexpr auto attributes_changed_array() { return "ATTRIBUTES_CHANGED_ARRAY"; }


struct OSSIA_EXPORT full_path_attribute
{
  using type = std::string;
  static constexpr auto key() { return detail::attribute_full_path(); }
  static constexpr const auto getter = static_cast<std::string(*)(const ossia::net::node_base&)>(ossia::net::osc_address_string);
};

// Attributes of an address
struct OSSIA_EXPORT value_attribute
{
  using type = ossia::value;
  static constexpr auto key() { return detail::attribute_value(); }
  static constexpr const auto getter = ossia::net::clone_value;
  static constexpr const auto setter = ossia::net::push_value;
};

struct OSSIA_EXPORT typetag_attribute
{
  using type = std::string;
  static constexpr auto key() { return detail::attribute_typetag(); }
  static constexpr const auto getter = oscquery::get_osc_typetag;
};

struct OSSIA_EXPORT domain_attribute
{
  using type = ossia::net::domain;
  static constexpr auto key() { return detail::attribute_range(); }
  static constexpr const auto getter = ossia::net::get_domain;
  static constexpr const auto setter = ossia::net::set_domain;
};

struct OSSIA_EXPORT access_mode_attribute
{
  using type = ossia::access_mode;
  static constexpr auto key() { return detail::attribute_accessmode(); }
  static constexpr const auto getter = ossia::net::get_access_mode;
  static constexpr const auto setter = ossia::net::set_access_mode;
};

struct OSSIA_EXPORT bounding_mode_attribute
{
  using type = ossia::bounding_mode;
  static constexpr auto key() { return detail::attribute_clipmode(); }
  static constexpr const auto getter = ossia::net::get_bounding_mode;
  static constexpr const auto setter = ossia::net::set_bounding_mode;
};

struct OSSIA_EXPORT unit_attribute
{
  using type = ossia::unit_t;
  static constexpr auto key() { return detail::attribute_unit(); }
  static constexpr const auto getter = ossia::net::get_unit;
  static constexpr const auto setter = ossia::net::set_unit;
};

struct OSSIA_EXPORT default_value_attribute
{
  using type = ossia::value;
  static constexpr auto key() { return detail::attribute_default_value(); }
  static constexpr const auto getter = ossia::net::get_default_value;
  static constexpr const auto setter = ossia::net::set_default_value;
};

// Metadata attributes
struct OSSIA_EXPORT tags_attribute
{
  using type = net::tags;
  static constexpr auto key() { return detail::attribute_tags(); }
  static constexpr const auto getter = ossia::net::get_tags;
  static constexpr const auto setter = ossia::net::set_tags;
};
struct OSSIA_EXPORT refresh_rate_attribute
{
  using type = net::refresh_rate;
  static constexpr auto key() { return detail::attribute_refresh_rate(); }
  static constexpr const auto getter = ossia::net::get_refresh_rate;
  static constexpr const auto setter = ossia::net::set_refresh_rate;
};
struct OSSIA_EXPORT priority_attribute
{
  using type = net::priority;
  static constexpr auto key() { return detail::attribute_priority(); }
  static constexpr const auto getter = ossia::net::get_priority;
  static constexpr const auto setter = ossia::net::set_priority;
};
struct OSSIA_EXPORT step_size_attribute
{
  using type = net::value_step_size;
  static constexpr auto key() { return detail::attribute_step_size(); }
  static constexpr const auto getter = ossia::net::get_value_step_size;
  static constexpr const auto setter = ossia::net::set_value_step_size;
};
struct OSSIA_EXPORT instance_bounds_attribute
{
  using type = net::instance_bounds;
  static constexpr auto key() { return detail::attribute_instance_bounds(); }
  static constexpr const auto getter = ossia::net::get_instance_bounds;
  static constexpr const auto setter = ossia::net::set_instance_bounds;
};
struct OSSIA_EXPORT critical_attribute
{
  using type = net::critical;
  static constexpr auto key() { return detail::attribute_critical(); }
  static constexpr const auto getter = ossia::net::get_critical;
  static constexpr const auto setter = ossia::net::set_critical;
};
struct OSSIA_EXPORT extended_type_attribute
{
  using type = net::extended_type;
  static constexpr auto key() { return detail::attribute_extended_type(); }
  static constexpr const auto getter = ossia::net::get_extended_type;
  static constexpr const auto setter = ossia::net::set_extended_type;
};
struct OSSIA_EXPORT description_attribute
{
  using type = net::description;
  static constexpr auto key() { return detail::attribute_description(); }
  static constexpr const auto getter = ossia::net::get_description;
  static constexpr const auto setter = static_cast<void(*)(ossia::net::extended_attributes&,optional<net::description>)>(ossia::net::set_description);
};
struct OSSIA_EXPORT repetition_filter_attribute
{
  using type = repetition_filter;
  static constexpr auto key() { return detail::attribute_repetition_filter(); }
  static constexpr const auto getter = ossia::net::get_repetition_filter;
  static constexpr const auto setter = ossia::net::set_repetition_filter;
};

using base_attributes = brigand::list<
typetag_attribute,
value_attribute,
domain_attribute,
access_mode_attribute,
bounding_mode_attribute,
repetition_filter_attribute,
unit_attribute,
default_value_attribute
>;

using base_attributes_without_type = brigand::list<
value_attribute,
domain_attribute,
access_mode_attribute,
bounding_mode_attribute,
repetition_filter_attribute,
unit_attribute,
default_value_attribute
>;

using base_attributes_without_type_and_value = brigand::list<
domain_attribute,
access_mode_attribute,
bounding_mode_attribute,
repetition_filter_attribute,
unit_attribute,
default_value_attribute
>;


using extended_attributes = brigand::list<
tags_attribute,
refresh_rate_attribute,
priority_attribute,
step_size_attribute,
instance_bounds_attribute,
critical_attribute,
extended_type_attribute,
description_attribute>;

}
enum class MessageType
{
  Device,
  Namespace,
  PathChanged, PathAdded, PathRemoved, AttributesChanged,
  PathsChanged, PathsAdded, PathsRemoved, AttributesChangedArray,
};
}
}
