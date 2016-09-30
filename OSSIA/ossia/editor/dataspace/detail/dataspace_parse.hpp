#pragma once
#include <ossia/editor/dataspace/dataspace_visitors.hpp>
#include <unordered_map>
namespace ossia
{
namespace detail
{
struct unit_text_visitor
{
  template<typename... Args>
  boost::string_view operator()(const eggs::variant<Args...>& dataspace)
  {
    return ossia::apply(*this, dataspace);
  }

  template<typename Unit>
  boost::string_view operator()(Unit)
  {
    return ossia::unit_traits<Unit>::text()[0];
  }

  boost::string_view operator()()
  {
    return {};
  }
};

struct dataspace_text_visitor
{
  template<typename Dataspace>
  boost::string_view operator()(const Dataspace& dataspace)
  {
    return ossia::dataspace_traits<Dataspace>::text()[0];
  }

  boost::string_view operator()()
  {
    return {};
  }
};

using unit_map = std::unordered_map<std::string, ossia::unit_t>;

template<typename Arg, typename... Args>
struct unit_map_factory
{
  void operator()(unit_map& m)
  {
    for(boost::string_view v : ossia::unit_traits<Arg>::text())
      m.emplace(v.to_string(), ossia::unit_t{Arg{}});
    unit_map_factory<Args...>{}(m);
  }
};

template<typename Arg>
struct unit_map_factory<Arg>
{
  void operator()(unit_map& m)
  {
    for(boost::string_view v : ossia::unit_traits<Arg>::text())
      m.emplace(v.to_string(), ossia::unit_t{Arg{}});
  }
};

template<typename... Args>
struct make_unit_map
{
  std::unordered_map<std::string, ossia::unit_t> operator()()
  {
    std::unordered_map<std::string, ossia::unit_t> map;
    unit_map_factory<Args...>{}(map);
    return map;
  }
};

struct unit_factory_visitor
{
  boost::string_view text;

  template<typename Dataspace_T>
  ossia::unit_t operator()(Dataspace_T arg)
  {
    static const auto units = brigand::wrap<Dataspace_T, make_unit_map>{}();
    auto it = units.find(text.to_string());
    return it != units.end() ? it->second : arg;
  }

  ossia::unit_t operator()()
  { return {}; }
};
}
}
