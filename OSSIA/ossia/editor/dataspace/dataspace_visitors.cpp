#include <ossia/editor/dataspace/dataspace_visitors.hpp>
#include <ossia/editor/value/value_conversion.hpp>
#include <ossia/editor/value/detail/value_conversion_impl.hpp>
#include <ossia/editor/dataspace/detail/dataspace_convert.hpp>
#include <ossia/editor/dataspace/detail/dataspace_merge.hpp>
#include <ossia/editor/dataspace/detail/dataspace_parse.hpp>
#include <ossia/editor/dataspace/detail/make_value.hpp>
#include <ossia/editor/dataspace/detail/make_unit.hpp>
#include <ossia/detail/logger.hpp>
#include <unordered_map>

namespace ossia
{
/// Parse ///
boost::string_view get_dataspace_text(unit_t u)
{
  return ossia::apply(detail::dataspace_text_visitor{}, u);
}

boost::string_view get_unit_text(unit_t u)
{
  return ossia::apply(detail::unit_text_visitor{}, u);
}

std::string get_pretty_unit_text(unit_t u)
{
    using namespace std::literals;
    if(u)
    {
        auto str = get_dataspace_text(u).to_string();
        auto str2 = get_unit_text(u);
        if(!str2.empty())
          return str + "." + str2.to_string();
        return str;
    }
    else
    {
        return "none"s;
    }
}

unit_t parse_unit(boost::string_view text, unit_t dataspace)
{
  if(!text.empty())
    return ossia::apply(detail::unit_factory_visitor{text}, dataspace);
  return {};
}

template<typename T>
ossia::unit_t parse_unit(boost::string_view text, T dataspace)
{
  return detail::unit_factory_visitor{text}(dataspace);
}

unit_t parse_pretty_unit(boost::string_view text)
{
  auto idx = text.find_first_of('.');
  if(idx != std::string::npos)
  {
    if(auto d = parse_dataspace(text.substr(0, idx)))
    {
      return parse_unit(text.substr(idx + 1), d);
    }
  }

  return {};
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
    {"time", time_u{}},
  };

  auto it = dataspaces.find(text.to_string());
  return it != dataspaces.end() ? it->second : unit_t{};
}

const unit_parse_symbols_t& get_unit_parser()
{
  static const detail::make_unit_symbols_helper m;
  return m.map;
}

namespace detail
{
template<typename Unit, typename = void>
struct unit_accessor_helper
{
  char operator()(uint8_t n)
  {
    return 0;
  }
};

template<typename Unit>
struct unit_accessor_helper<Unit, detail::enable_if_multidimensional<Unit>>
{
  char operator()(uint8_t n)
  {
    const auto& a = Unit::array_parameters();
    if(n < a.size())
      return a[n];
    return 0;
  }
};
}
char get_unit_accessor(const ossia::unit_t& unit, uint8_t n)
{
  if(unit)
  {
    return eggs::variants::apply([=] (auto d) {
      if(d)
      {
        return eggs::variants::apply([=] (auto u) {
          return detail::unit_accessor_helper<decltype(u)>{}(n);
        }, d);
      }

      return '\0';
    }, unit);
  }

  return 0;
}

/// Convert ///
value_with_unit make_value(const ossia::value& v, const ossia::unit_t& u)
{
  if(!u || !v.valid())
    return v;

  return eggs::variants::apply([&] (const auto& dataspace) -> ossia::value_with_unit {
    if(!dataspace)
      return v;
    return eggs::variants::apply(make_value_with_unit_visitor{}, v.v, dataspace);
  }, u);
}

unit_t make_unit(uint64_t dataspace, uint64_t unit)
{
  static const ossia::make_unit_helper f;
  return f.get_unit(dataspace, unit);
}

val_type matching_type(const unit_t& u)
{
  if(!u)
    return ossia::val_type::IMPULSE;

  return eggs::variants::apply(
        [&] (const auto& dataspace) -> ossia::val_type
  {
    if(!dataspace)
      return ossia::val_type::IMPULSE;

    return eggs::variants::apply(
          [] (auto unit) -> ossia::val_type
    {
      using unit_t = decltype(unit);
      return ossia::value_trait<typename unit_t::value_type>::ossia_enum;
    }, dataspace);
  }, u);

}
value_with_unit convert(value_with_unit v, unit_t t)
{
  if(v && t)
  {
    if(auto value = v.target<ossia::value>())
      return make_value(*value, t);
    else
      return eggs::variants::apply(detail::convert_unit_visitor{}, v, t);
  }
  else
  {
    return v;
  }
}

value to_value(value_with_unit v)
{
  return ossia::apply(detail::convert_to_value_visitor{}, v);

}

unit_t to_unit(value_with_unit v)
{
  return ossia::apply(detail::convert_to_unit_visitor{}, v);
}


std::string to_pretty_string(value_with_unit v)
{
  fmt::MemoryWriter s;
  s << value_to_pretty_string(to_value(v)) << " " << get_pretty_unit_text(to_unit(v));
  return s.str();
}

/// Merge ///
struct merger_impl
{
  const ossia::value& val;
  const ossia::destination_index& idx;

  ossia::value_with_unit operator()(const ossia::value& val)
  {
    // TODO use the standard merge algorithm in ossia::value ??return value_unit;
    return {};
  }

  template<typename Dataspace_T>
  ossia::value_with_unit operator()(const Dataspace_T& ds)
  {
    if(ds && val.v)
      return eggs::variants::apply(detail::value_merger{idx}, ds, val.v);
    return {};
  }
};

template<std::size_t N>
struct vec_merger_impl_helper
{
  const ossia::Vec<float, N>& val;
  const std::bitset<N>& idx;

  template<typename Unit_T>
  ossia::value_with_unit operator()(const Unit_T& unit)
  {
    return detail::vec_value_merger<N>{idx}(unit, val);
  }
};

template<std::size_t N>
struct vec_merger_impl
{
  const ossia::Vec<float, N>& val;
  const std::bitset<N>& idx;

  ossia::value_with_unit operator()(const ossia::value& val)
  {
    // TODO use the standard merge algorithm in ossia::value ??return value_unit;
    return {};
  }

  template<typename Dataspace_T>
  ossia::value_with_unit operator()(const Dataspace_T& ds)
  {
    if(ds)
    {
      return eggs::variants::apply(vec_merger_impl_helper<N>{val, idx}, ds);
    }
    return {};
  }
};
ossia::value_with_unit merge(
    ossia::value_with_unit vu,
    const ossia::value& val,
    ossia::destination_index idx)
{
  if(vu && val.valid())
  {
    return eggs::variants::apply(merger_impl{val, idx}, vu);
  }

  return vu;
}

ossia::value_with_unit merge(
    ossia::value_with_unit vu,
    const ossia::Vec2f& val,
    std::bitset<2> idx)
{
  if(vu)
  {
    return eggs::variants::apply(vec_merger_impl<2>{val, idx}, vu);
  }

  return vu;
}

ossia::value_with_unit merge(
    ossia::value_with_unit vu,
    const ossia::Vec3f& val,
    std::bitset<3> idx)
{
  if(vu)
  {
    return eggs::variants::apply(vec_merger_impl<3>{val, idx}, vu);
  }

  return vu;
}

ossia::value_with_unit merge(
    ossia::value_with_unit vu,
    const ossia::Vec4f& val,
    std::bitset<4> idx)
{
  if(vu)
  {
    return eggs::variants::apply(vec_merger_impl<4>{val, idx}, vu);
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
template OSSIA_EXPORT ossia::unit_t parse_unit(boost::string_view, ossia::time_u);

}
