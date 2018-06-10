// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/network/oscquery/detail/attributes.hpp>
#include <ossia/detail/for_each.hpp>

namespace ossia
{
namespace oscquery
{

const key_map_type& ossia_to_oscquery_key()
{
  static const key_map_type attr_map{[] {
    using namespace detail;
    key_map_type attr_impl;

    ossia::for_each_tagged(base_attributes{}, [&](auto attr) {
      using type = typename decltype(attr)::type;
      attr_impl.insert(std::make_pair(type::text(), metadata<type>::key()));
    });
    ossia::for_each_tagged(extended_attributes{}, [&](auto attr) {
      using type = typename decltype(attr)::type;
      attr_impl.insert(std::make_pair(type::text(), metadata<type>::key()));
    });
    return attr_impl;
  }()};
  return attr_map;
}

const key_map_type& oscquery_to_ossia_key()
{
  static const key_map_type attr_map{[] {
    using namespace detail;
    key_map_type attr_impl;

    ossia::for_each_tagged(base_attributes{}, [&](auto attr) {
      using type = typename decltype(attr)::type;
      attr_impl.insert(std::make_pair(metadata<type>::key(), type::text()));
    });
    ossia::for_each_tagged(extended_attributes{}, [&](auto attr) {
      using type = typename decltype(attr)::type;
      attr_impl.insert(std::make_pair(metadata<type>::key(), type::text()));
    });
    return attr_impl;
  }()};
  return attr_map;
}

OSSIA_EXPORT optional<ossia::string_view>
ossia_to_oscquery_key(ossia::string_view s)
{
  auto& m = ossia_to_oscquery_key();
  auto it = m.find(s);
  if (it != m.end())
    return it.value();
  return ossia::none;
}

OSSIA_EXPORT optional<ossia::string_view>
oscquery_to_ossia_key(ossia::string_view s)
{
  auto& m = oscquery_to_ossia_key();
  auto it = m.find(s);
  if (it != m.end())
    return it.value();
  return ossia::none;
}
}
}
