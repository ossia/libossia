// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "json_reader_detail.hpp"

#include "json_parser.hpp"

#include <ossia/detail/for_each.hpp>
#include <ossia/detail/json.hpp>
#include <ossia/network/base/device.hpp>
#include <ossia/network/base/node_attributes.hpp>
#include <ossia/network/dataspace/dataspace.hpp>
#include <ossia/network/dataspace/dataspace_visitors.hpp>
#include <ossia/network/domain/domain.hpp>
#include <ossia/network/generic/generic_node.hpp>
#include <ossia/network/oscquery/detail/attributes.hpp>
#include <ossia/network/oscquery/detail/oscquery_units.hpp>
#include <ossia/network/oscquery/detail/value_to_json.hpp>

#include <oscpack/osc/OscTypes.h>

namespace ossia::oscquery
{
namespace detail
{

bool json_parser_impl::ReadValue(const rapidjson::Value& val, int32_t& res)
{
  bool b = val.IsInt();
  if(b)
    res = val.GetInt();
  return b;
}

bool json_parser_impl::ReadValue(const rapidjson::Value& val, float& res)
{
  bool b = val.IsFloat();
  if(b)
    res = val.GetFloat();
  return b;
}

bool json_parser_impl::ReadValue(const rapidjson::Value& val, double& res)
{
  bool b = val.IsNumber();
  if(b)
    res = val.GetDouble();
  return b;
}

bool json_parser_impl::ReadValue(const rapidjson::Value& val, std::string& res)
{
  bool b = val.IsString();
  if(b)
    res = get_string(val);
  return b;
}

bool json_parser_impl::ReadValue(const rapidjson::Value& val, bool& res)
{
  bool b = val.IsBool();
  if(b)
    res = val.GetBool();
  return b;
}

bool json_parser_impl::ReadValue(const rapidjson::Value& val, repetition_filter& res)
{
  bool b = val.IsBool();
  if(b)
    res = val.GetBool() ? ossia::repetition_filter::ON : ossia::repetition_filter::OFF;
  return b;
}

value json_parser_impl::ReadValue(const rapidjson::Value& val)
{
  return detail::ReadValue(val);
}
static bool read_bounding(const rapidjson::Value& val, bounding_mode& res)
{
  bool b = val.IsString() && val.GetStringLength() > 2;
  if(b)
  {
    const char* str = val.GetString();

    // we compare with the first unique letter
    switch(str[2])
    {
      case 'n':
        res = ossia::bounding_mode::FREE;
        break;
      case 't':
        res = ossia::bounding_mode::CLIP;
        break;
      case 'w':
        res = ossia::bounding_mode::LOW;
        break;
      case 'g':
        res = ossia::bounding_mode::HIGH;
        break;
      case 'a':
        res = ossia::bounding_mode::WRAP;
        break;
      case 'l':
        res = ossia::bounding_mode::FOLD;
        break;
      default:
        b = false;
    }
  }
  return b;
}

bool json_parser_impl::ReadValue(const rapidjson::Value& val, bounding_mode& res)
{
  switch(val.GetType())
  {
    case rapidjson::kStringType:
      return read_bounding(val, res);
    case rapidjson::kArrayType: {
      const auto& arr = val.GetArray();
      if(arr.Size() > 0)
      {
        return read_bounding(arr[0], res);
      }
      return false;
    }
    default:
      return false;
  }

  return false;
}

static bool read_access(int res, ossia::access_mode& am)
{
  switch(res)
  {
    case 1:
      am = ossia::access_mode::GET;
      break;
    case 2:
      am = ossia::access_mode::SET;
      break;
    case 3:
      am = ossia::access_mode::BI;
      break;
    default:
      return false;
  }
  return true;
}

bool json_parser_impl::ReadValue(const rapidjson::Value& val, access_mode& am)
{
  switch(val.GetType())
  {
    case rapidjson::kNumberType:
      return read_access((int)val.GetDouble(), am);
    case rapidjson::kArrayType: {
      const auto& arr = val.GetArray();
      if(arr.Size() > 0)
      {
        if(arr[0].IsInt())
        {
          return read_access(arr[0].GetInt(), am);
        }
      }
      return false;
    }
    default:
      return false;
  }
  return false;
}

static bool ReadValueObject(const rapidjson::Value& val, domain& res)
{
  auto min_it = val.FindMember("MIN");
  auto max_it = val.FindMember("MAX");
  auto values_it = val.FindMember("VALS");

  auto mem_end = val.MemberEnd();

  if(values_it != mem_end)
  {
    if(values_it->value.IsArray())
    {
      std::vector<ossia::value> vals;
      const auto& arr = values_it->value.GetArray();
      for(auto& val : arr)
      {
        vals.push_back(ReadValue(val));
      }
      res = ossia::make_domain(std::move(vals));
      return true;
    }
  }
  else if(min_it != mem_end || max_it != mem_end)
  {
    if(min_it != mem_end && max_it != mem_end)
    {
      res = ossia::make_domain(ReadValue(min_it->value), ReadValue(max_it->value));
      return true;
    }
    else if(min_it != mem_end)
    {
      res = ossia::make_domain(ReadValue(min_it->value), {});
      return true;
    }
    else if(max_it != mem_end)
    {
      res = ossia::make_domain(ossia::value{}, ReadValue(max_it->value));
      return true;
    }
  }

  return false;
}

template <std::size_t N>
vecf_domain<N> parse_vecf(const rapidjson::Value& val)
{
  vecf_domain<N> dom;
  const auto& arr = val.GetArray();
  for(std::size_t i = 0; i < std::min((std::size_t)arr.Size(), N); i++)
  {
    const rapidjson::Value& elt = arr[i];
    if(elt.IsObject())
    {
      auto min_it = elt.FindMember("MIN");
      auto max_it = elt.FindMember("MAX");
      // auto values_it = elt.FindMember("VALS");

      auto mem_end = elt.MemberEnd();

      /* TODO values for vecf
      if (values_it != mem_end)
      {
        if (values_it->value.IsArray())
        {
          for (auto& val : values_it->value.GetArray())
          {
            dom.values[i].insert(ReadValue(val));
          }
        }
      }
      */

      if(min_it != mem_end)
      {
        float f;
        if(json_parser_impl::ReadValue(min_it->value, f))
          dom.min[i] = f;
      }

      if(max_it != mem_end)
      {
        float f;
        if(json_parser_impl::ReadValue(max_it->value, f))
          dom.max[i] = f;
      }
    }
  }
  return dom;
}
bool json_parser_impl::ReadValue(
    const ossia::net::node_base& node, const rapidjson::Value& val, domain& res)
{
  if(val.IsObject())
  {
    return ReadValueObject(val, res);
  }
  else if(val.IsArray())
  {
    auto t = node.get_parameter()->get_value_type();
    switch(t)
    {
      case ossia::val_type::VEC2F:
        res = parse_vecf<2>(val);
        return true;
      case ossia::val_type::VEC3F:
        res = parse_vecf<3>(val);
        return true;
      case ossia::val_type::VEC4F:
        res = parse_vecf<4>(val);
        return true;
      case ossia::val_type::LIST: {
        vector_domain dom;
        dom.min.resize(val.GetArray().Size());
        dom.max.resize(val.GetArray().Size());
        dom.values.resize(val.GetArray().Size());

        int i = 0;
        for(const rapidjson::Value& elt : val.GetArray())
        {
          if(elt.IsObject())
          {
            auto min_it = elt.FindMember("MIN");
            auto max_it = elt.FindMember("MAX");
            auto values_it = elt.FindMember("VALS");

            auto mem_end = elt.MemberEnd();

            if(values_it != mem_end)
            {
              if(values_it->value.IsArray())
              {
                for(auto& val : values_it->value.GetArray())
                {
                  dom.values[i].insert(ReadValue(val));
                }
              }
            }

            if(min_it != mem_end)
            {
              dom.min[i] = ReadValue(min_it->value);
            }

            if(max_it != mem_end)
            {
              dom.max[i] = ReadValue(max_it->value);
            }
          }

          i++;
        }

        res = std::move(dom);
        return true;
      }

      default: {
        const rapidjson::Value& elt = *val.GetArray().Begin();
        if(elt.IsObject())
        {
          return ReadValueObject(elt, res);
        }
      }
    }
  }

  return false;
}

bool json_parser_impl::ReadValue(const rapidjson::Value& val, unit_t& res)
{
  bool b = val.IsArray();
  if(b)
  {
    ossia::small_vector<std::string, 4> str;
    for(auto& v : val.GetArray())
    {
      b &= v.IsString();
      if(!b)
        break;

      str.push_back(v.GetString());
    }

    static detail::unit_parser parser;
    if(auto it = parser.map.find(str); it != parser.map.end())
    {
      res = it->second;
      return true;
    }
  }
  return b;
}

bool json_parser_impl::ReadValue(const rapidjson::Value& val, net::tags& res)
{
  bool b = val.IsArray();
  if(b)
  {
    auto arr = val.GetArray();
    for(auto& tag : arr)
    {
      if(tag.IsString())
      {
        res.emplace_back(tag.GetString(), tag.GetStringLength());
      }
    }
  }
  return b;
}

bool json_parser_impl::ReadValue(const rapidjson::Value& val, net::instance_bounds& res)
{
  bool b = val.IsArray();
  if(b)
  {
    auto arr = val.GetArray();
    if(arr.Size() == 2 && arr[0].IsInt() && arr[1].IsInt())
    {
      res.min_instances = arr[0].GetInt();
      res.max_instances = arr[1].GetInt();
    }
    else
    {
      b = false;
    }
  }
  return b;
}

using map_setter_fun = void (*)(const rapidjson::Value&, ossia::net::node_base&);
template <typename Attr>
static auto make_setter_pair()
{
  return std::make_pair(
      metadata<Attr>::key(),
      [](const rapidjson::Value& val, ossia::net::node_base& node) {
    typename Attr::type res;
    if(json_parser_impl::ReadValue(val, res))
      Attr::setter(node, std::move(res));
      });
}

template <>
auto make_setter_pair<ossia::net::domain_attribute>()
{
  using Attr = ossia::net::domain_attribute;
  return std::make_pair(
      metadata<Attr>::key(),
      [](const rapidjson::Value& val, ossia::net::node_base& node) {
    typename Attr::type res;
    if(json_parser_impl::ReadValue(node, val, res))
      Attr::setter(node, std::move(res));
      });
}

using reader_map_type = string_view_map<map_setter_fun>;

static auto& namespaceSetterMap()
{
  static const reader_map_type map{[] {
    reader_map_type attr_impl;
    using namespace ossia::net;

    // Remaining metadata
    ossia::for_each_tagged(attributes_when_reading{}, [&](auto attr) {
      using type = typename decltype(attr)::type;
      attr_impl.insert(make_setter_pair<type>());
    });

    return attr_impl;
  }()};

  return map;
}

static ossia::val_type read_vec_typetag(ossia::string_view typetag)
{
  if(typetag == "ff" || typetag == "[ff]")
    return ossia::val_type::VEC2F;
  else if(typetag == "fff" || typetag == "[fff]")
    return ossia::val_type::VEC3F;
  else if(typetag == "ffff" || typetag == "[ffff]")
    return ossia::val_type::VEC4F;
  else
    return ossia::val_type::LIST;
}

//! Used to check if an actual value with multiple elements,
//! such as [ 1, 3, "foo" ] has a special typetag
static ossia::val_type read_vec_typetag(const rapidjson::Value& val)
{
  if(val.IsArray())
  {
    const auto n = val.Size();
    switch(n)
    {
      case 2:
        if(val.GetArray()[0].IsFloat() && val.GetArray()[1].IsFloat())
          return ossia::val_type::VEC2F;
        break;
      case 3:
        if(val.GetArray()[0].IsFloat() && val.GetArray()[1].IsFloat()
           && val.GetArray()[2].IsFloat())
          return ossia::val_type::VEC3F;
        break;
      case 4:
        if(val.GetArray()[0].IsFloat() && val.GetArray()[1].IsFloat()
           && val.GetArray()[2].IsFloat() && val.GetArray()[3].IsFloat())
          return ossia::val_type::VEC4F;
        break;
    }
  }
  return ossia::val_type::LIST;
}

static ossia::val_type
underlying_type(const extended_type& e_type, ossia::string_view typetag)
{
  ossia::val_type actual_type = ossia::val_type::LIST; // Generic worse
                                                       // case; also
                                                       // "list_type()"
  if(e_type == generic_buffer_type() || e_type == filesystem_path_type()
     || e_type == url_type())
  {
    actual_type = ossia::val_type::STRING;
  }
  else if(e_type == list_type())
  {
    // nothing to do, but don't remove so that
    // we don't go into the float_array case
  }
  else if(e_type == float_array_type())
  {
    // Look for Vec2f, Vec3f, Vec4f
    actual_type = read_vec_typetag(typetag);
  }

  return actual_type;
}

static ossia::val_type underlying_type(
    const extended_type& e_type, const rapidjson::Value& obj, ossia::string_view typetag,
    rapidjson::Value::ConstMemberIterator value_it,
    rapidjson::Value::ConstMemberIterator default_value_it)
{
  ossia::val_type actual_type = ossia::val_type::LIST; // Generic worse
                                                       // case; also
                                                       // "list_type()"
  if(e_type == generic_buffer_type() || e_type == filesystem_path_type()
     || e_type == url_type())
  {
    actual_type = ossia::val_type::STRING;
  }
  else if(e_type == list_type())
  {
    // nothing to do, but don't remove so that
    // we don't go into the float_array case
  }
  else if(e_type == float_array_type())
  {
    // Look for Vec2f, Vec3f, Vec4f
    actual_type = read_vec_typetag(typetag);
    if(actual_type == ossia::val_type::LIST)
    {
      // We try to find through the actual values
      if(value_it != obj.MemberEnd())
      {
        actual_type = read_vec_typetag(value_it->value);
      }
      else if(default_value_it != obj.MemberEnd())
      {
        actual_type = read_vec_typetag(default_value_it->value);
      }
    }
  }

  return actual_type;
}

static void create_or_update_parameter_type(
    ossia::net::node_base& node, const rapidjson::Value& obj,
    ossia::net::parameter_base*& request_value)
{
  using impl = ossia::oscquery::detail::json_parser_impl;
  // First initialize the address if it's not an empty node
  ossia::string_view typetag;

  // Try to read all the attributes that could give us the concrete type.
  complex_type val_type;                                       // Implementation type
  std::optional<ossia::unit_t> unit = std::nullopt;            // Unit
  std::optional<ossia::extended_type> ext_type = std::nullopt; // Extended type

  // Look for the typetag
  auto type_it = obj.FindMember(detail::attribute_typetag());
  if(type_it != obj.MemberEnd() && type_it->value.IsString())
  {
    typetag = get_string_view(type_it->value);
    val_type = get_type_from_osc_typetag(typetag);
  }

  // Look for the unit
  if(auto unit_it = obj.FindMember(detail::attribute_unit()); unit_it != obj.MemberEnd())
  {
    ossia::unit_t u;
    impl::ReadValue(unit_it->value, u);
    if(u)
      unit = std::move(u);
  }

  if(auto ext_type_it = obj.FindMember(detail::attribute_extended_type());
     ext_type_it != obj.MemberEnd() && !ext_type_it->value.IsNull())
  {
    ossia::unit_t u;
    impl::ReadValue(ext_type_it->value, u);
    if(u)
    {
      unit = std::move(u);
    }
    else if(ext_type_it->value.IsArray())
    {
      // FIXME ext_type should be an array
      if(auto arr = ext_type_it->value.GetArray(); arr.Size() > 0)
        ext_type = get_string(ext_type_it->value.GetArray()[0]);
    }
    else
    {
      ext_type = get_string(ext_type_it->value);
    }
  }

  // If any of these exist, we can update the parameter
  if(val_type || unit || ext_type)
  {
    ossia::net::parameter_base* addr = node.get_parameter();

    if(!addr)
    {
      if(unit) // The unit enforces the value type
      {
        addr = node.create_parameter(ossia::matching_type(*unit));
        addr->set_unit(*unit);
        ossia::net::set_extended_type(node, ext_type);
      }
      else if(ext_type)
      {
        addr = node.create_parameter(detail::underlying_type(*ext_type, typetag));
        ossia::net::set_extended_type(node, ext_type);
      }
      else if(val_type)
      {
        addr = setup_parameter(val_type, node);
      }
    }
    else
    {
      if(unit) // The unit enforces the value type
      {
        addr->set_unit(*unit);
        ossia::net::set_extended_type(node, ext_type);
      }
      else if(ext_type)
      {
        const auto t = detail::underlying_type(*ext_type, typetag);
        if(t != addr->get_value_type())
          addr->set_value_type(t);
        ossia::net::set_extended_type(node, ext_type);
      }
      else if(val_type)
      {
        setup_parameter(val_type, node);
      }
    }
    request_value = addr;
  }
}

ossia::value parse_oscquery_value(
    ossia::net::node_base& node, const rapidjson::Value& v, std::string_view typetag,
    bool& ok)
{
  ossia::value res = node.get_parameter()->value();
  const auto& u = node.get_parameter()->get_unit();
  if(typetag.size() == 1)
  {
    if(v.IsArray())
    {
      const auto& ar = v.GetArray();
      if(!ar.Empty())
      {
        ok = res.apply(oscquery::detail::json_to_single_value{*ar.Begin(), typetag});
      }
    }
    else
    {
      ok = res.apply(oscquery::detail::json_to_single_value{v, typetag});
    }
  }
  else
  {
    int typetag_counter = 0;
    ok = res.apply(oscquery::detail::json_to_value{v, typetag, typetag_counter, u});
  }
  return res;
}

void json_parser_impl::readParameter(net::node_base& node, const rapidjson::Value& obj)
{
  // Read all the attributes
  // First initialize the address if it's not an empty node
  ossia::string_view typetag;

  // Try to read all the attributes that could give us the concrete type.
  complex_type val_type;                                       // Implementation type
  std::optional<ossia::unit_t> unit = std::nullopt;            // Unit
  std::optional<ossia::extended_type> ext_type = std::nullopt; // Extended type

  // TODO maybe read all the attributes and store their iterators, then do
  // them in the order we want ?
  const auto value_it = obj.FindMember(detail::attribute_value());
  const auto default_value_it = obj.FindMember(detail::attribute_default_value());

  // Look for the typetag
  const auto type_it = obj.FindMember(detail::attribute_typetag());
  bool has_typetag = false;
  if(type_it != obj.MemberEnd())
  {
    has_typetag = true;
    typetag = get_string_view(type_it->value);
    val_type = get_type_from_osc_typetag(typetag);
  }

  // Look for the unit
  if(auto unit_it = obj.FindMember(detail::attribute_unit()); unit_it != obj.MemberEnd())
  {
    ossia::unit_t u;
    ReadValue(unit_it->value, u);
    if(u)
      unit = std::move(u);
  }

  if(auto ext_type_it = obj.FindMember(detail::attribute_extended_type());
     ext_type_it != obj.MemberEnd() && !ext_type_it->value.IsNull())
  {
    ossia::unit_t u;
    ReadValue(ext_type_it->value, u);
    if(u)
    {
      unit = std::move(u);
    }
    else if(ext_type_it->value.IsArray())
    {
      if(auto arr = ext_type_it->value.GetArray(); arr.Size() > 0)
        ext_type = get_string(ext_type_it->value.GetArray()[0]);
    }
    else
    {
      ext_type = get_string(ext_type_it->value);
    }
  }

  // If any of these exist, we can create a parameter
  if(has_typetag || unit || ext_type)
  {
    ossia::net::parameter_base* addr = nullptr;
    if(unit) // The unit enforces the value type
    {
      addr = node.create_parameter(ossia::matching_type(*unit));
      addr->set_unit(*unit);
      ossia::net::set_extended_type(node, ext_type);
    }
    // FIXME necessary for R color handling... berk handle that better
    else if(has_typetag)
    {
      addr = setup_parameter(val_type, node);
    }
    else if(ext_type)
    {
      addr = node.create_parameter(
          underlying_type(*ext_type, obj, typetag, value_it, default_value_it));
      ossia::net::set_extended_type(node, ext_type);
    }
    if(!addr && ext_type)
    {
      addr = node.create_parameter(
          underlying_type(*ext_type, obj, typetag, value_it, default_value_it));
      ossia::net::set_extended_type(node, ext_type);
    }
    // We have a type. Now we read the value according to it.
    if(value_it != obj.MemberEnd())
    {
      bool ok = false;
      auto res = parse_oscquery_value(node, value_it->value, typetag, ok);
      if(ok)
        node.get_parameter()->set_value(std::move(res));
    }

    // Same for default value
    if(default_value_it != obj.MemberEnd())
    {
      bool ok = false;
      auto res = parse_oscquery_value(node, default_value_it->value, typetag, ok);
      if(ok)
      {
        if(value_it == obj.MemberEnd())
          node.get_parameter()->set_value(res);

        ossia::net::set_default_value(node, std::move(res));
      }
    }
  }
  else
  {
    // We may be able to use the actual or default value
    if(value_it != obj.MemberEnd())
    {
      auto val = ReadValue(value_it->value);
      auto addr = node.create_parameter(val.get_type());
      addr->set_value(std::move(val));
    }

    if(default_value_it != obj.MemberEnd())
    {
      auto val = ReadValue(default_value_it->value);
      if(node.get_parameter())
      {
        ossia::net::set_default_value(node, std::move(val));
      }
      else
      {
        auto addr = node.create_parameter(val.get_type());
        addr->set_value(val);
        ossia::net::set_default_value(node, std::move(val));
      }
    }
  }

  // Read all the members
  auto& map = namespaceSetterMap();
  auto memb_end = obj.MemberEnd();
  for(auto it = obj.MemberBegin(); it != memb_end; ++it)
  {
    auto action = map.find(get_string_view(it->name));
    if(action != map.end())
    {
      action.value()(it->value, node);
    }
  }
}

void json_parser_impl::readObject(net::node_base& node, const rapidjson::Value& obj)
{
  // If it's a real parameter
  if(obj.FindMember(detail::attribute_full_path()) != obj.MemberEnd())
  {
    readParameter(node, obj);
  }

  // Recurse on the children
  auto contents_it = obj.FindMember(detail::contents());
  if(contents_it != obj.MemberEnd() && contents_it->value.IsObject())
  {
    auto& obj = contents_it->value;
    for(auto child_it = obj.MemberBegin(); child_it != obj.MemberEnd(); ++child_it)
    {
      auto cld = node.create_child(get_string(child_it->name));
      readObject(*cld, child_it->value);
    }
  }
}

void json_parser_impl::reloadObject(net::node_base& node, const rapidjson::Value& obj)
{
  // If it's a real parameter
  if(obj.FindMember(detail::attribute_full_path()) != obj.MemberEnd())
  {
    readParameter(node, obj);
  }

  // Recurse on the children
  ossia::flat_set<std::string> existing_child;
  auto contents_it = obj.FindMember(detail::contents());
  if(contents_it != obj.MemberEnd() && contents_it->value.IsObject())
  {
    auto& obj = contents_it->value;
    for(auto child_it = obj.MemberBegin(); child_it != obj.MemberEnd(); ++child_it)
    {
      auto name = get_string(child_it->name);
      ossia::net::node_base* cld = node.find_child(name);
      if(!cld)
        cld = node.create_child(name);
      else
        ossia::net::set_zombie(*cld, false);
      existing_child.insert(std::move(name));

      reloadObject(*cld, child_it->value);
    }
  }

  for(auto& cld : node.children())
  {
    if(existing_child.find(cld->get_name()) == existing_child.end())
    {
      ossia::net::set_zombie(*cld, true);
    }
  }
}
}

std::shared_ptr<rapidjson::Document> json_parser::parse(const std::string& message)
{
  auto document = std::make_shared<rapidjson::Document>();
  document->Parse(message);
  return document;
}

int json_parser::get_port(const rapidjson::Value& obj)
{
  using namespace detail;
  json_assert(obj[detail::osc_port()].IsInt());

  return obj[detail::osc_port()].GetInt();
}

ossia::oscquery::message_type json_parser::message_type(const rapidjson::Value& obj)
{
  static const string_view_map<ossia::oscquery::message_type> map{
      {detail::path_added(), ossia::oscquery::message_type::PathAdded},
      {detail::path_changed(), ossia::oscquery::message_type::PathChanged},
      {detail::path_removed(), ossia::oscquery::message_type::PathRemoved},
      {detail::path_renamed(), ossia::oscquery::message_type::PathRenamed},
      {detail::attributes_changed(), ossia::oscquery::message_type::AttributesChanged},
      {detail::start_osc_streaming(), ossia::oscquery::message_type::StartOscStreaming},
      {detail::listen(), ossia::oscquery::message_type::Listen},
      {detail::ignore(), ossia::oscquery::message_type::Ignore}};
  using namespace detail;

  auto it = obj.FindMember(detail::command());
  if(it != obj.MemberEnd())
  {
    auto mt_it = map.find(get_string(it->value)); // TODO string_view
    if(mt_it != map.end())
      return mt_it.value();
  }

  if(obj.FindMember(detail::attribute_full_path()) != obj.MemberEnd())
  {
    return ossia::oscquery::message_type::Namespace;
  }

  if(obj.FindMember(detail::name()) != obj.MemberEnd())
  {
    return ossia::oscquery::message_type::HostInfo;
  }

  if(obj.FindMember(detail::osc_port()) != obj.MemberEnd())
  {
    return ossia::oscquery::message_type::Device;
  }

  return ossia::oscquery::message_type::Value; // TODO More checks needed
}

host_info json_parser::parse_host_info(const rapidjson::Value& doc)
{
  host_info info;

  if(auto osc_ip = doc.FindMember("OSC_IP"); osc_ip != doc.MemberEnd())
  {
    if(osc_ip->value.IsString())
      info.osc_ip = std::string(osc_ip->value.GetString());
  }
  if(auto osc_port = doc.FindMember("OSC_PORT"); osc_port != doc.MemberEnd())
  {
    if(osc_port->value.IsInt())
      info.osc_port = osc_port->value.GetInt();
  }

  if(auto osc_transport = doc.FindMember("OSC_TRANSPORT");
     osc_transport != doc.MemberEnd())
  {
    if(osc_transport->value.IsString())
    {
      using namespace std::literals;
      auto str = osc_transport->value.GetString();
      if(str == "TCP"sv)
        info.osc_transport = host_info::TCP;
      else if(str == "UDP"sv)
        info.osc_transport = host_info::UDP;
    }
  }
  if(auto ws_ip = doc.FindMember("WS_IP"); ws_ip != doc.MemberEnd())
  {
    if(ws_ip->value.IsString())
      info.ws_ip = std::string(ws_ip->value.GetString());
  }
  if(auto ws_port = doc.FindMember("WS_PORT"); ws_port != doc.MemberEnd())
  {
    if(ws_port->value.IsInt())
      info.ws_port = ws_port->value.GetInt();
  }

  if(auto ext = doc.FindMember("EXTENSIONS"); ext != doc.MemberEnd())
  {
    if(ext->value.IsObject())
    {
      auto exts = ext->value.GetObject();
      for(auto it = exts.MemberBegin(); it != exts.MemberEnd(); ++it)
      {
        if(it->value.IsBool())
        {
          bool ok = it->value.GetBool();
          info.extensions.insert({it->name.GetString(), ok});
        }
      }
    }
  }

  return info;
}

void json_parser::parse_namespace(net::node_base& root, const rapidjson::Value& obj)
{
  // Get the point from which we must update the namespace.
  auto path_it = obj.FindMember(detail::attribute_full_path());
  if(path_it != obj.MemberEnd())
  {
    auto str = get_string_view(path_it->value);
    auto node = ossia::net::find_node(root.get_device().get_root_node(), str);
    if(node)
    {
      node->clear_children();
      node->remove_parameter();

      detail::json_parser_impl::readObject(*node, obj);
    }
    else
    {
      throw ossia::node_not_found_error{std::string(str) + "not found"};
    }
  }
  else
  {
    root.clear_children();
    root.remove_parameter();

    detail::json_parser_impl::readObject(root, obj);
  }
}

void json_parser::parse_value(net::parameter_base& addr, const rapidjson::Value& obj)
{
  auto val = addr.value();
  val.apply(detail::json_to_value_unchecked{obj});
  addr.set_value(std::move(val));
}

void json_parser::parse_parameter_value(
    net::node_base& root, const rapidjson::Value& obj, ossia::net::device_base& dev)
{
  for(auto it = obj.MemberBegin(), end = obj.MemberEnd(); it != end; ++it)
  {
    auto path = get_string_view(it->name);
    if(!path.empty() && path[0] == '/')
    {
      auto node = ossia::net::find_node(root, path);
      if(node)
      {
        auto addr = node->get_parameter();
        if(addr)
        {
          auto val = addr->value();
          val.apply(detail::json_to_value_unchecked{it->value});

          // TODO don't push it back to the sender
          addr->push_value(std::move(val));
          dev.on_message(*addr);
          continue;
        }
      }
    }

    dev.on_unhandled_message(std::string(path), detail::ReadValue(it->value));
  }
}

// Given a string "/foo/bar/baz", return {"/foo/bar", "baz"}
static std::optional<std::pair<std::string, std::string>>
splitParentChild(ossia::string_view s)
{
  auto last_slash_pos = s.rfind('/');
  if(last_slash_pos != std::string::npos)
  {
    return std::make_pair(
        std::string{s.substr(0, last_slash_pos)},
        std::string{s.substr(last_slash_pos + 1)});
  }
  return std::nullopt;
}

void json_parser::parse_path_added(
    net::node_base& root, const std::string& full_path, const rapidjson::Value& obj)
{

  // TODO potential race condition there,
  // if the node is created in-between the if / else
  auto opt_str = splitParentChild(full_path);
  auto& s = *opt_str;

  if(auto parent = ossia::net::find_node(root, s.first))
  {
    auto cld = parent->find_child(s.second);
    if(!cld)
    {
      // The node does not exist
      auto c = parent->create_child(s.second);
      detail::json_parser_impl::readObject(*c, obj);
    }
    else
    {
      // The node already exists, rescan it
      ossia::net::set_zombie(*cld, false);

      // cld->clear_children();
      // cld->remove_parameter();
      detail::json_parser_impl::reloadObject(*cld, obj);
    }
  }
  else
  {
    // Neither the node nor its parent exists
    auto& cld = ossia::net::create_node(root, full_path);
    detail::json_parser_impl::readObject(cld, obj);
  }
}

void json_parser::parse_path_removed(
    net::node_base& root, const rapidjson::Value& obj, bool zombie_on_remove)
{
  auto dat_it = obj.FindMember(detail::data());
  if(dat_it != obj.MemberEnd())
  {
    auto path = get_string_view(dat_it->value);
    if(auto node = ossia::net::find_node(root, path))
    {
      if(zombie_on_remove)
      {
        ossia::net::set_zombie(*node, true);
      }
      else
      {
        node->get_parent()->remove_child(*node);
      }
    }
  }
}

void json_parser::parse_path_renamed(net::node_base& root, const rapidjson::Value& obj)
{
  auto dat_it = obj.FindMember(detail::data());
  if(dat_it != obj.MemberEnd())
  {
    if(dat_it->value.IsObject())
    {
      auto old_it = dat_it->value.FindMember("OLD");
      auto new_it = dat_it->value.FindMember("NEW");

      if(old_it != dat_it->value.MemberEnd() && new_it != dat_it->value.MemberEnd())
      {
        if(old_it->value.IsString() && new_it->value.IsString())
        {
          if(auto node = ossia::net::find_node(root, old_it->value.GetString()))
          {
            std::string_view new_path = get_string_view(new_it->value);
            auto last_slash = new_path.find_last_of('/');
            if(last_slash != std::string::npos)
            {
              auto last_part = new_path.substr(last_slash + 1);
              node->set_name(std::string{last_part});
            }
          }
        }
      }
    }
  }
}

void json_parser::parse_path_changed(net::node_base& map, const rapidjson::Value& mess)
{
  // TODO
  /*
      using namespace detail;

      // Get the object
      const auto& obj = mess.get<json_map>(detail::path_changed());

      // 2. Remove the missing contents
      map.remove(valToString(obj.get(detail::full_path())));

      // 3. Replace it
      readObject(map, obj);
      */
}

void json_parser::parse_attributes_changed(
    net::node_base& root, const rapidjson::Value& obj,
    ossia::net::parameter_base*& request_value)
{
  auto dat_it = obj.FindMember(detail::data());
  if(dat_it != obj.MemberEnd())
  {
    const rapidjson::Value& dat = dat_it->value;
    auto path_it = dat.FindMember(detail::attribute_full_path());
    if(path_it != dat.MemberEnd())
    {
      auto path = get_string_view(path_it->value);
      auto node = ossia::net::find_node(root, path);
      if(node)
      {
        // First handle the typetag, unit, ext_type, etc
        detail::create_or_update_parameter_type(*node, dat, request_value);

        // Then the remaining attributes
        auto& map = detail::namespaceSetterMap();
        auto memb_end = dat.MemberEnd();
        for(auto it = dat.MemberBegin(); it != memb_end; ++it)
        {
          auto action = map.find(get_string_view(it->name));
          if(action != map.end())
          {
            action.value()(it->value, *node);
          }
        }
      }
    }
  }
}
}
