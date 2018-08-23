// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/network/oscquery/detail/typetag.hpp>

namespace ossia
{
namespace oscquery
{

//! Puts the OSC typetag of a value in a std::string
//! For instance "if", "T", "fTI", etc.
struct osc_type_visitor
{
  std::string& type;
  void operator()()
  {
    type += oscpack::TypeTagValues::NIL_TYPE_TAG;
  }
  void operator()(impulse)
  {
    type += oscpack::TypeTagValues::INFINITUM_TYPE_TAG;
  }
  void operator()(int v)
  {
    type += oscpack::TypeTagValues::INT32_TYPE_TAG;
  }
  void operator()(float v)
  {
    type += oscpack::TypeTagValues::FLOAT_TYPE_TAG;
  }
  void operator()(bool v)
  {
    type += v ? oscpack::TypeTagValues::TRUE_TYPE_TAG
              : oscpack::TypeTagValues::FALSE_TYPE_TAG;
  }
  void operator()(char v)
  {
    type += oscpack::TypeTagValues::CHAR_TYPE_TAG;
  }
  void operator()(const std::string& v)
  {
    // TODO BLOB
    type += oscpack::TypeTagValues::STRING_TYPE_TAG;
  }

  template <std::size_t N>
  void operator()(const std::array<float, N>& vec)
  {
    type += oscpack::TypeTagValues::ARRAY_BEGIN_TYPE_TAG;
    for (std::size_t i = 0; i < N; i++)
    {
      type += oscpack::TypeTagValues::FLOAT_TYPE_TAG;
    }
    type += oscpack::TypeTagValues::ARRAY_END_TYPE_TAG;
  }

  void operator()(const std::vector<ossia::value>& vec)
  {
    type.reserve(type.size() + vec.size() + 2);

    type += oscpack::TypeTagValues::ARRAY_BEGIN_TYPE_TAG;
    for (const auto& sub : vec)
    {
      sub.apply(*this);
    }
    type += oscpack::TypeTagValues::ARRAY_END_TYPE_TAG;
  }
};

static std::string get_osc_typetag_impl(const net::parameter_base& addr)
{
  using namespace std::literals;
  if (addr.get_unit() == ossia::rgba8_u{})
    return "r"s;

  std::string s;
  auto val = addr.value();
  val.apply(osc_type_visitor{s});

  switch (val.get_type())
  {
    case ossia::val_type::LIST:
    case ossia::val_type::VEC2F:
    case ossia::val_type::VEC3F:
    case ossia::val_type::VEC4F:
      // Erase the top-level '[ ]'
      s.pop_back();
      s.erase(0, 1);
      break;
    default:
      break;
  }

  return s;
}

optional<std::string> get_osc_typetag(const net::node_base& n)
{
  if (auto addr = n.get_parameter())
  {
    return get_osc_typetag_impl(*addr);
  }
  else
  {
    return ossia::none;
  }
}

complex_type get_type_from_osc_typetag(ossia::string_view str)
{
  switch (str.size())
  {
    case 0:
      return {}; // maybe an impulse?
    case 1:
      switch (str[0])
      {
        case oscpack::TypeTagValues::NIL_TYPE_TAG:
          return {};

        case oscpack::TypeTagValues::INFINITUM_TYPE_TAG:
          return ossia::val_type::IMPULSE;

        case oscpack::TypeTagValues::INT32_TYPE_TAG:
        case oscpack::TypeTagValues::INT64_TYPE_TAG:
        case oscpack::TypeTagValues::TIME_TAG_TYPE_TAG:
          return ossia::val_type::INT;

        case oscpack::TypeTagValues::FLOAT_TYPE_TAG:
        case oscpack::TypeTagValues::DOUBLE_TYPE_TAG:
          return ossia::val_type::FLOAT;

        case oscpack::TypeTagValues::CHAR_TYPE_TAG:
          return ossia::val_type::CHAR;

        case oscpack::TypeTagValues::TRUE_TYPE_TAG:
        case oscpack::TypeTagValues::FALSE_TYPE_TAG:
          return ossia::val_type::BOOL;

        case oscpack::TypeTagValues::STRING_TYPE_TAG:
        case oscpack::TypeTagValues::SYMBOL_TYPE_TAG:
          return ossia::val_type::STRING;

        case oscpack::TypeTagValues::BLOB_TYPE_TAG:
          return ossia::generic_buffer_type();

        case oscpack::TypeTagValues::RGBA_COLOR_TYPE_TAG:
          return ossia::rgba8_u{};

        default:
          break;
      }
      break;

    default:
      break;
  }

  return ossia::val_type::LIST;
}

void set_osc_typetag(net::node_base& n, ossia::string_view tag)
{
  setup_parameter(get_type_from_osc_typetag(tag), n);
}
}
}
