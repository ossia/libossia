#pragma once
#include <ossia/editor/dataspace/dataspace_visitors.hpp>
#include <ossia/editor/dataspace/dataspace_parse.hpp>
#include <boost/spirit/include/qi.hpp>
#include <brigand/algorithms/for_each.hpp>
#include <hopscotch_map.h>

namespace ossia
{
namespace detail
{
struct unit_text_visitor
{
  template<typename... Args>
  OSSIA_INLINE ossia::string_view operator()(const eggs::variant<Args...>& dataspace)
  {
    return ossia::apply(*this, dataspace);
  }

  template<typename Unit>
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

struct dataspace_text_visitor
{
  template<typename Dataspace>
  OSSIA_INLINE ossia::string_view operator()(const Dataspace& dataspace)
  {
    return ossia::dataspace_traits<Dataspace>::text()[0];
  }

  OSSIA_INLINE ossia::string_view operator()()
  {
    return {};
  }
};

using unit_map = tsl::hopscotch_map<std::string, ossia::unit_t>;

template<typename Arg, typename... Args>
struct unit_map_factory
{
  void operator()(unit_map& m)
  {
    for(ossia::string_view v : ossia::unit_traits<Arg>::text())
      m.emplace(v.to_string(), ossia::unit_t{Arg{}});
    unit_map_factory<Args...>{}(m);
  }
};

template<typename Arg>
struct unit_map_factory<Arg>
{
  void operator()(unit_map& m)
  {
    for(ossia::string_view v : ossia::unit_traits<Arg>::text())
      m.emplace(v.to_string(), ossia::unit_t{Arg{}});
  }
};

template<typename... Args>
struct make_unit_map
{
  tsl::hopscotch_map<std::string, ossia::unit_t> operator()()
  {
    tsl::hopscotch_map<std::string, ossia::unit_t> map;
    unit_map_factory<Args...>{}(map);
    return map;
  }
};

struct unit_factory_visitor
{
  ossia::string_view text;

  template<typename Dataspace_T>
  ossia::unit_t operator()(Dataspace_T arg)
  {
    static const auto units = brigand::wrap<Dataspace_T, make_unit_map>{}();
    auto it = units.find(text.to_string());
    return it != units.end() ? it->second : arg;
  }

  OSSIA_INLINE ossia::unit_t operator()()
  { return {}; }
};

template<typename Unit>
using enable_if_multidimensional = std::enable_if_t<Unit::is_multidimensional::value>;

template<typename Dataspace, typename Unit, typename = void>
struct make_unit_symbols_sub_helper
{
  void operator()(unit_parse_symbols_t& map)
  {
    using unit_type = Unit;

    std::string res;
    res.reserve(20);

    for(auto ds : dataspace_traits<Dataspace>::text())
    {
      // For each unit :
      for(auto un : unit_traits<unit_type>::text())
      {
        res.clear();

        res += ds.to_string(); // color
        res += '.'; // color.

        res += un.to_string(); // color.rgb

        // Add the unit in long form
        map.add(res, {{}, unit_type{}});
      }
    }
  }
};

template<typename Dataspace, typename Unit>
struct make_unit_symbols_sub_helper<Dataspace, Unit, enable_if_multidimensional<Unit>>
{
  void operator()(unit_parse_symbols_t& map)
  {
    using unit_type = Unit;

    std::string res;
    res.reserve(20);

    for(auto ds : dataspace_traits<Dataspace>::text())
    {
      // For each unit :
      for(auto un : unit_traits<unit_type>::text())
      {
        res.clear();

        res += ds.to_string(); // color
        res += '.'; // color.

        res += un.to_string(); // color.rgb

        // Add the unit in long form
        map.add(res, {{}, unit_type{}});

        // Add all the accessors
        res += "._"; // color.rgb._

        const auto& params = unit_type::array_parameters();
        const auto n = params.size();
        for(std::size_t i = 0; i < n; i++)
        {
          // replace the last char with the one in the array parameter
          res[res.size() - 1] = params[i]; // color.rgb.r
          map.add(res, {{(uint8_t)i}, unit_type{}});
        }
      }
    }
  }
};

struct make_unit_symbols_helper
{
  unit_parse_symbols_t map;

  make_unit_symbols_helper()
  {
    brigand::for_each<ossia::unit_variant>([&] (auto t) {
      using dataspace_type = typename decltype(t)::type;
      brigand::for_each<dataspace_type>([&] (auto u) {
        using unit_type = typename decltype(u)::type;
        make_unit_symbols_sub_helper<dataspace_type, unit_type>{}(map);
      });
    });
  }
};

}
}
