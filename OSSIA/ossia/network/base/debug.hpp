#pragma once
#include <fmt/format.h>
#include <ossia/network/common/address_properties.hpp>
#include <ossia_export.h>
namespace ossia
{
namespace net
{
class node_base;
class domain;

OSSIA_EXPORT
fmt::BasicWriter<char>& operator<<(fmt::BasicWriter<char>&, ossia::access_mode);
OSSIA_EXPORT
fmt::BasicWriter<char>& operator<<(fmt::BasicWriter<char>&, const ossia::net::domain&);
OSSIA_EXPORT
fmt::BasicWriter<char>& operator<<(fmt::BasicWriter<char>&, ossia::bounding_mode);
OSSIA_EXPORT
fmt::BasicWriter<char>& operator<<(fmt::BasicWriter<char>&, const ossia::net::node_base&);
OSSIA_EXPORT
void debug_recursively(fmt::MemoryWriter&, const ossia::net::node_base&);
}
}
