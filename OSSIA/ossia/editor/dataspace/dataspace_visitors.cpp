#include <ossia/editor/dataspace/dataspace_visitors.hpp>
#include <unordered_map>

namespace ossia
{

struct unit_text_visitor
{
  template<typename... Args>
  boost::string_ref operator()(const eggs::variant<Args...>& dataspace)
  {
    if(dataspace)
      return eggs::variants::apply(*this, dataspace);
    else
      return "";
  }

  template<typename Unit>
  boost::string_ref operator()(Unit)
  {
    return ossia::unit_traits<Unit>::text()[0];
  }
};


boost::string_ref get_unit_text(unit_t u)
{
  return eggs::variants::apply(unit_text_visitor{}, u);

}


using unit_map = std::unordered_map<std::string, ossia::unit_t>;

template<typename Arg, typename... Args>
struct unit_map_factory
{
  void operator()(unit_map& m)
  {
    for(boost::string_ref v : ossia::unit_traits<Arg>::text())
      m.emplace(v.to_string(), ossia::unit_t{Arg{}});
    unit_map_factory<Args...>{}(m);
  }
};

template<typename Arg>
struct unit_map_factory<Arg>
{
  void operator()(unit_map& m)
  {
    for(boost::string_ref v : ossia::unit_traits<Arg>::text())
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
  boost::string_ref text;

  template<typename Dataspace_T>
  ossia::unit_t operator()(Dataspace_T arg)
  {
    static const auto units = brigand::wrap<Dataspace_T, make_unit_map>{}();
    auto it = units.find(text.to_string());
    return it != units.end() ? it->second : arg;
  }
};

unit_t parse_unit(boost::string_ref text, unit_t dataspace)
{
  return eggs::variants::apply(unit_factory_visitor{text}, dataspace);
}

template<typename T>
ossia::unit_t parse_unit(boost::string_ref text, T dataspace)
{
  return unit_factory_visitor{text}(dataspace);
}

template OSSIA_EXPORT ossia::unit_t parse_unit(boost::string_ref, ossia::color_u);
template OSSIA_EXPORT ossia::unit_t parse_unit(boost::string_ref, ossia::distance_u);
template OSSIA_EXPORT ossia::unit_t parse_unit(boost::string_ref, ossia::position_u);
template OSSIA_EXPORT ossia::unit_t parse_unit(boost::string_ref, ossia::speed_u);
template OSSIA_EXPORT ossia::unit_t parse_unit(boost::string_ref, ossia::orientation_u);
template OSSIA_EXPORT ossia::unit_t parse_unit(boost::string_ref, ossia::angle_u);

}
