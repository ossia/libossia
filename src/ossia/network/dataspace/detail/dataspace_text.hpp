#pragma once
#include <ossia/network/dataspace/dataspace.hpp>
#include <ossia/network/dataspace/detail/dataspace_list.hpp>
#include <ossia/detail/hash_map.hpp>
#include <ossia/detail/for_each.hpp>

namespace ossia::detail
{

struct unit_text_visitor
{
  OSSIA_INLINE ossia::string_view operator()(const angle_u& dataspace)
  {
    return ossia::apply(*this, dataspace);
  }
  OSSIA_INLINE ossia::string_view operator()(const color_u& dataspace)
  {
    return ossia::apply(*this, dataspace);
  }
  OSSIA_INLINE ossia::string_view operator()(const distance_u& dataspace)
  {
    return ossia::apply(*this, dataspace);
  }
  OSSIA_INLINE ossia::string_view operator()(const gain_u& dataspace)
  {
    return ossia::apply(*this, dataspace);
  }
  OSSIA_INLINE ossia::string_view operator()(const position_u& dataspace)
  {
    return ossia::apply(*this, dataspace);
  }
  OSSIA_INLINE ossia::string_view operator()(const orientation_u& dataspace)
  {
    return ossia::apply(*this, dataspace);
  }
  OSSIA_INLINE ossia::string_view operator()(const speed_u& dataspace)
  {
    return ossia::apply(*this, dataspace);
  }
  OSSIA_INLINE ossia::string_view operator()(const timing_u& dataspace)
  {
    return ossia::apply(*this, dataspace);
  }

  template <typename Unit>
  OSSIA_INLINE ossia::string_view operator()(Unit)
  {
    return ossia::unit_traits<Unit>::text()[0];
  }

  OSSIA_INLINE ossia::string_view operator()(const ossia::value&)
  {
    return {};
  }

  OSSIA_INLINE ossia::string_view operator()()
  {
    return {};
  }
};



struct unit_accessor_visitor
{
  OSSIA_INLINE ossia::string_view operator()(const angle_u& dataspace)
  {
    return {};
  }
  OSSIA_INLINE ossia::string_view operator()(const color_u& dataspace)
  {
    return ossia::apply(*this, dataspace);
  }
  OSSIA_INLINE ossia::string_view operator()(const distance_u& dataspace)
  {
    return {};
  }
  OSSIA_INLINE ossia::string_view operator()(const gain_u& dataspace)
  {
    return {};
  }
  OSSIA_INLINE ossia::string_view operator()(const position_u& dataspace)
  {
    return ossia::apply(*this, dataspace);
  }
  OSSIA_INLINE ossia::string_view operator()(const orientation_u& dataspace)
  {
    return ossia::apply(*this, dataspace);
  }
  OSSIA_INLINE ossia::string_view operator()(const speed_u& dataspace)
  {
    return {};
  }
  OSSIA_INLINE ossia::string_view operator()(const timing_u& dataspace)
  {
    return {};
  }

  template <typename Unit>
  OSSIA_INLINE ossia::string_view operator()(Unit)
  {
    return Unit::array_parameters();
  }

  OSSIA_INLINE ossia::string_view operator()()
  {
    return {};
  }
};

struct dataspace_text_visitor
{
  template <typename Dataspace>
  OSSIA_INLINE ossia::string_view operator()(const Dataspace& dataspace)
  {
    return ossia::dataspace_traits<Dataspace>::text()[0];
  }

  OSSIA_INLINE ossia::string_view operator()()
  {
    return {};
  }
};

using unit_pretty_text_map = ossia::fast_hash_map<ossia::unit_t, std::string>;
template <typename Dataspace, typename Unit>
std::string make_pretty_unit_text()
{
  using unit_type = Unit;

  std::string res;
  res.reserve(20);

  auto ds = dataspace_traits<Dataspace>::text()[0];
  auto un = unit_traits<unit_type>::text()[0];

  res.append(ds.data(), ds.size()); // color
  res += '.';                       // color.

  res.append(un.data(), un.size()); // color.rgb

  return res;
}


struct unit_pretty_texts
{
  unit_pretty_text_map map;

  unit_pretty_texts()
  {
    ossia::for_each_tagged(dataspace_u_list{}, [&](auto t) {
      using dataspace_type = typename decltype(t)::type;
      ossia::for_each_tagged(dataspace_type{}, [&](auto u) {
        using unit_type = typename decltype(u)::type;
        map[unit_type{}] = make_pretty_unit_text<dataspace_type, unit_type>();
      });
    });
  }
};

}
