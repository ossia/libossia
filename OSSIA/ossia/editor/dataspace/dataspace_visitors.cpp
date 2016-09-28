#include <ossia/editor/dataspace/dataspace_visitors.hpp>
#include <ossia/editor/value/value_conversion.hpp>
#include <ossia/editor/dataspace/detail/dataspace_convert.hpp>
#include <ossia/editor/dataspace/detail/dataspace_merge.hpp>
#include <ossia/editor/dataspace/detail/dataspace_parse.hpp>
#include <ossia/detail/logger.hpp>
#include <unordered_map>

namespace ossia
{
/// Parse ///

boost::string_view get_unit_text(unit_t u)
{
  if(u)
    return eggs::variants::apply(detail::unit_text_visitor{}, u);
  return {};
}

unit_t parse_unit(boost::string_view text, unit_t dataspace)
{
  if(dataspace && !text.empty())
    return eggs::variants::apply(detail::unit_factory_visitor{text}, dataspace);
  return {};
}

template<typename T>
ossia::unit_t parse_unit(boost::string_view text, T dataspace)
{
  return detail::unit_factory_visitor{text}(dataspace);
}

unit_t parse_dataspace(boost::string_view text)
{
  static const std::unordered_map<std::string, unit_t> dataspaces{
    {"color", color_u{}},
    {"distance", distance_u{}},
    {"position", position_u{}},
    {"speed", speed_u{}},
    {"orientation", orientation_u{}},
    {"angle", angle_u{}},
    {"gain", gain_u{}},
  };

  auto it = dataspaces.find(text.to_string());
  return it != dataspaces.end() ? it->second : unit_t{};
}


/// Convert ///

value_with_unit convert(value_with_unit v, unit_t t)
{
  if(v && t)
  {
    return eggs::variants::apply(detail::convert_unit_visitor{}, v, t);
  }
  else
  {
    return v;
  }
}

value to_value(value_with_unit v)
{
  if(v)
    return eggs::variants::apply(detail::convert_to_value_visitor{}, v);
  return {};
}

std::string to_pretty_string(value_with_unit v)
{
  fmt::MemoryWriter s;
  s << to_pretty_string(to_value(v)) << " " << eggs::variants::apply(detail::unit_text_visitor{}, v).to_string();
  return s.str();
}

/// Merge ///

ossia::value_with_unit merge(
    ossia::value_with_unit vu,
    const ossia::value& val,
    ossia::destination_index idx)
{
  if(vu && val.valid())
  {
    return eggs::variants::apply([&] (const auto& dataspace) -> ossia::value_with_unit {
      return eggs::variants::apply(detail::value_merger{idx}, dataspace, val.v);
    }, vu);
  }

  return vu;
}

template OSSIA_EXPORT ossia::unit_t parse_unit(boost::string_view, ossia::color_u);
template OSSIA_EXPORT ossia::unit_t parse_unit(boost::string_view, ossia::distance_u);
template OSSIA_EXPORT ossia::unit_t parse_unit(boost::string_view, ossia::position_u);
template OSSIA_EXPORT ossia::unit_t parse_unit(boost::string_view, ossia::speed_u);
template OSSIA_EXPORT ossia::unit_t parse_unit(boost::string_view, ossia::orientation_u);
template OSSIA_EXPORT ossia::unit_t parse_unit(boost::string_view, ossia::angle_u);
template OSSIA_EXPORT ossia::unit_t parse_unit(boost::string_view, ossia::gain_u);


}
