#include "json_reader_detail.hpp"
#include "json_reader.hpp"
#include <ossia/network/oscquery/detail/attributes.hpp>
#include <ossia/network/oscquery/detail/value_to_json.hpp>
#include <ossia/network/base/node_attributes.hpp>
#include <ossia/editor/dataspace/dataspace.hpp>
#include <ossia/editor/dataspace/dataspace_visitors.hpp>
#include <oscpack/osc/OscTypes.h>
#include <brigand/algorithms/for_each.hpp>
#include <rapidjson/document.h>

namespace ossia
{
namespace oscquery
{
namespace detail
{

optional<val_type> json_parser_impl::TypeTagToType(ossia::string_view str)
{
  switch(str.size())
  {
    case 0:
      return ossia::none; // todo throw instead ? return optional ?
    case 1:
      switch(str[0])
      {
        case oscpack::TypeTagValues::INFINITUM_TYPE_TAG: return ossia::val_type::IMPULSE;
        case oscpack::TypeTagValues::INT32_TYPE_TAG: return ossia::val_type::INT;
        case oscpack::TypeTagValues::FLOAT_TYPE_TAG: return ossia::val_type::FLOAT;
        case oscpack::TypeTagValues::CHAR_TYPE_TAG: return ossia::val_type::CHAR;

        case oscpack::TypeTagValues::TRUE_TYPE_TAG:
        case oscpack::TypeTagValues::FALSE_TYPE_TAG: return ossia::val_type::BOOL;

        case oscpack::TypeTagValues::STRING_TYPE_TAG:
        case oscpack::TypeTagValues::SYMBOL_TYPE_TAG: return ossia::val_type::STRING;

        default: throw bad_request_error("Unsupported type");
      }
    default:
      return ossia::val_type::TUPLE;
  }
}

bool json_parser_impl::ReadValue(const rapidjson::Value& val, int32_t& res)
{
  bool b = val.IsInt();
  if(b)
    res = val.GetInt();
  return b;
}

bool json_parser_impl::ReadValue(const rapidjson::Value& val, float& res)
{
  bool b = val.IsDouble();
  if(b)
    res = val.GetDouble();
  return b;
}

bool json_parser_impl::ReadValue(const rapidjson::Value& val, std::string& res)
{
  bool b = val.IsString();
  if(b)
    res = std::string(val.GetString(), val.GetStringLength());
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
  switch(val.GetType())
  {
    case rapidjson::kNumberType:
      return val.GetDouble();

    case rapidjson::kFalseType:
      return false;
    case rapidjson::kTrueType:
      return true;

    case rapidjson::kArrayType:
    {
      std::vector<ossia::value> tpl;
      tpl.reserve(val.Size());
      for(auto& elt : val.GetArray())
      {
        tpl.push_back(ReadValue(elt));
      }
      return tpl;
    }

    case rapidjson::kStringType:
      return std::string(val.GetString(), val.GetStringLength());

    case rapidjson::kObjectType:
    case rapidjson::kNullType:
    default:
      return ossia::impulse{};
  }
}

bool json_parser_impl::ReadValue(const rapidjson::Value& val, bounding_mode& res)
{
  bool b = val.IsString() && val.GetStringLength() > 2;
  if(b)
  {
    const char* str = val.GetString();

    // we compare with the first unique letter
    switch(str[2])
    {
      case 'n': res = ossia::bounding_mode::FREE; break;
      case 't': res = ossia::bounding_mode::CLIP; break;
      case 'w': res = ossia::bounding_mode::LOW;  break;
      case 'g': res = ossia::bounding_mode::HIGH; break;
      case 'a': res = ossia::bounding_mode::WRAP; break;
      case 'l': res = ossia::bounding_mode::FOLD; break;
      default:
        b = false;
    }
  }
  return b;
}

bool json_parser_impl::ReadValue(const rapidjson::Value& val, access_mode& am)
{
  bool b = val.IsInt();
  if(b)
  {
    int res = val.GetInt();
    switch(res)
    {
      case 1: am = ossia::access_mode::GET; break;
      case 2: am = ossia::access_mode::SET; break;
      case 3: am = ossia::access_mode::BI;  break;
      default:
        b = false;
    }
  }
  return b;
}

bool json_parser_impl::ReadValue(const rapidjson::Value& val, net::domain& res)
{
  if(!val.IsArray())
    return false;

  // Read the domain as it is
  if(val.Size() == 2) // min, max
  {
    res = ossia::net::make_domain(ReadValue(val[0]), ReadValue(val[1]));
    return true;
  }

  else if(val.Size() >= 3 && val[2].IsArray())
  {
    std::vector<ossia::value> tpl;
    tpl.reserve(val[2].Size());
    for(auto& elt : val[2].GetArray())
    {
      tpl.push_back(ReadValue(elt));
    }
    res = ossia::net::make_domain(ReadValue(val[0]), ReadValue(val[1]), std::move(tpl));
    return true;
  }
  return false;
}

bool json_parser_impl::ReadValue(const rapidjson::Value& val, unit_t& res)
{
  bool b = val.IsString();
  if(b)
    res = parse_pretty_unit(val.GetString());
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
        res.push_back(tag.GetString());
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

using map_setter_fun = void(*)(const rapidjson::Value&, ossia::net::node_base&);
template<typename Attr>
static auto make_setter_pair()
{
  return std::make_pair(Attr::key(),
                        [] (const rapidjson::Value& val, ossia::net::node_base& node) {
    typename Attr::type res;
    if(json_parser_impl::ReadValue(val, res))
      Attr::setter(node, std::move(res));
  });
}

using map_type = tsl::hopscotch_map<ossia::string_view, map_setter_fun>;

static auto& setterMap()
{
  static const map_type map{
    [] {
      map_type attr_impl;

      // Remaining metadata
      brigand::for_each< brigand::list<
          domain_attribute,
          access_mode_attribute,
          bounding_mode_attribute,
          repetition_filter_attribute,
          tags_attribute,
          refresh_rate_attribute,
          priority_attribute,
          step_size_attribute,
          instance_bounds_attribute,
          critical_attribute,
          description_attribute
          > >([&] (auto attr) {
        using type = typename decltype(attr)::type;
        attr_impl.insert(make_setter_pair<type>());
      });
      brigand::for_each<extended_attributes>([&] (auto attr) {
        using type = typename decltype(attr)::type;
        attr_impl.insert(make_setter_pair<type>());
      });

      return attr_impl;
    }()};

  return map;
}

static ossia::val_type VecTypetag(ossia::string_view typetag)
{
  if     (typetag == "ff"   || typetag == "[ff]")   return ossia::val_type::VEC2F;
  else if(typetag == "fff"  || typetag == "[fff]")  return ossia::val_type::VEC3F;
  else if(typetag == "ffff" || typetag == "[ffff]") return ossia::val_type::VEC4F;
  else                                              return ossia::val_type::TUPLE;
}

static ossia::val_type VecTypetag(const rapidjson::Value& val)
{
  if(val.IsArray())
  {
    switch(val.Size())
    {
      case 2: return ossia::val_type::VEC2F;
      case 3: return ossia::val_type::VEC3F;
      case 4: return ossia::val_type::VEC4F;
    }
  }
  return ossia::val_type::TUPLE;
}

void json_parser_impl::readObject(net::node_base& node, const rapidjson::Value& obj)
{
  auto& map = setterMap();

  // If it's a real parameter
  if(obj.FindMember(detail::attribute_full_path()) != obj.MemberEnd())
  {
    // Read all the attributes
    // First initialize the address if it's not an empty node
    ossia::string_view typetag;

    // Try to read all the attributes that could give us the concrete type.
    optional<ossia::val_type> val_type = ossia::none; // Implementation type
    optional<ossia::unit_t> unit = ossia::none; // Unit
    optional<ossia::net::extended_type> ext_type = ossia::none; // Extended type

    // TODO maybe read all the attributes and store their iterators, then do them in the order we want ?
    auto value_it = obj.FindMember(detail::attribute_value());
    auto default_value_it = obj.FindMember(detail::attribute_default_value());

    // Look for the typetag
    auto type_it = obj.FindMember(detail::attribute_typetag());
    if(type_it != obj.MemberEnd())
    {
      typetag = getStringView(type_it->value);
      val_type = TypeTagToType(typetag);
    }

    // Look for the unit
    auto unit_it = obj.FindMember(detail::attribute_unit());
    if(unit_it != obj.MemberEnd())
    {
      ossia::unit_t u;
      ReadValue(unit_it->value, u);
      if(u)
        unit = std::move(u);
    }

    // Look for an extended type
    auto ext_type_it = obj.FindMember(detail::attribute_extended_type());
    if(ext_type_it != obj.MemberEnd() && ext_type_it->value.IsString())
    {
      ext_type = std::string(ext_type_it->value.GetString(), ext_type_it->value.GetStringLength());
    }

    // If any of these exist, we can create an address
    if(val_type || unit || ext_type)
    {
      ossia::net::address_base* addr = nullptr;
      if(unit) // The unit enforces the value type
      {
        addr = node.createAddress(ossia::matching_type(*unit));

        addr->setUnit(*unit);
        ossia::net::set_extended_type(node, ext_type);
      }
      else if(ext_type)
      {
        ossia::val_type actual_type = ossia::val_type::TUPLE; // Generic worse case
        if(*ext_type == net::generic_buffer_type() || *ext_type == net::filesystem_path_type())
        {
          actual_type = ossia::val_type::STRING;
        }
        else if(*ext_type == net::float_array_type())
        {
          // Look for Vec2f, Vec3f, Vec4f
          actual_type = VecTypetag(typetag);
          if(actual_type == ossia::val_type::TUPLE)
          {
            // We try to find through the actual values
            if(value_it != obj.MemberEnd())
            {
              actual_type = VecTypetag(value_it->value);
            }
            else if(default_value_it != obj.MemberEnd())
            {
              actual_type = VecTypetag(default_value_it->value);
            }
          }
        }

        addr = node.createAddress(actual_type);
        ossia::net::set_extended_type(node, ext_type);
      }
      else if(val_type)
      {
        addr = node.createAddress(*val_type);
      }

      // We have a type. Now we read the value according to it.
      if(value_it != obj.MemberEnd())
      {
        ossia::value res = node.getAddress()->cloneValue();
        int typetag_counter = 0;
        bool ok = res.apply(oscquery::detail::json_to_value{value_it->value, typetag, typetag_counter});
        if(ok)
          node.getAddress()->setValue(std::move(res));
      }

      // Same for default value
      if(default_value_it != obj.MemberEnd())
      {
        ossia::value res = node.getAddress()->cloneValue();
        int typetag_counter = 0;
        bool ok = res.apply(oscquery::detail::json_to_value{default_value_it->value, typetag, typetag_counter});
        if(ok)
          ossia::net::set_default_value(node, std::move(res));
      }
    }
    else
    {
      // We may be able to use the actual or default value
      if(value_it != obj.MemberEnd())
      {
        auto val = ReadValue(value_it->value);
        auto addr = node.createAddress(val.getType());
        addr->setValue(std::move(val));
      }

      if(default_value_it != obj.MemberEnd())
      {
        auto val = ReadValue(default_value_it->value);
        if(node.getAddress())
        {
          ossia::net::set_default_value(node, std::move(val));
        }
        else
        {
          auto addr = node.createAddress(val.getType());
          addr->setValue(val);
          ossia::net::set_default_value(node, std::move(val));
        }
      }
    }

    // Read all the members
    auto memb_end = obj.MemberEnd();
    for(auto it = obj.MemberBegin(); it != memb_end; ++it)
    {
      auto action = map.find(it->name.GetString()); // TODO string -> string view
      if(action != map.end())
      {
        action.value()(it->value, node);
      }
    }
  }

  // Recurse on the children
  auto contents_it = obj.FindMember(detail::contents());
  if(contents_it != obj.MemberEnd() && contents_it->value.IsObject())
  {
    auto& obj = contents_it->value;
    for(auto child_it = obj.MemberBegin(); child_it != obj.MemberEnd(); ++child_it)
    {
      auto cld = node.createChild(child_it->name.GetString());
      readObject(*cld, child_it->value);
    }
  }
}


}



rapidjson::Document json_parser::parse(const std::string& message)
{
  rapidjson::Document document;
  document.Parse(message);
  return document;
}

int json_parser::getPort(const rapidjson::Value& obj)
{
  using namespace detail;
  json_assert(obj[detail::osc_port()].IsInt());

  return obj[detail::osc_port()].GetInt();
}

message_type json_parser::messageType(const rapidjson::Value& obj)
{
  static tsl::hopscotch_map<std::string, message_type> map{
    { std::string(detail::path_added()), message_type::PathAdded },
    { std::string(detail::path_changed()), message_type::PathChanged },
    { std::string(detail::path_removed()), message_type::PathRemoved },
    { std::string(detail::attributes_changed()), message_type::AttributesChanged },
    { std::string(detail::paths_added()), message_type::PathsAdded },
    { std::string(detail::paths_changed()), message_type::PathsChanged },
    { std::string(detail::paths_removed()), message_type::PathsRemoved },
    { std::string(detail::attributes_changed_array()), message_type::AttributesChangedArray }
  };
  using namespace detail;
  auto val_it = obj.FindMember(detail::attribute_value());
  if(val_it != obj.MemberEnd())
  {
    return message_type::Value;
  }

  auto it = obj.FindMember(detail::command());
  if(it != obj.MemberEnd())
  {
    auto mt_it = map.find(it->value.GetString()); // TODO this returns a char*
    if(mt_it != map.end())
      return mt_it.value();
  }

  if(obj.FindMember(detail::osc_port()) != obj.MemberEnd())
  {
    return message_type::Device;
  }

  return message_type::Namespace; // TODO More checks needed
}

void json_parser::parseNamespace(net::node_base& root, const rapidjson::Value& obj)
{
  // Get the point from which we must update the namespace.
  auto path_it = obj.FindMember(detail::attribute_full_path());
  if(path_it != obj.MemberEnd())
  {
    std::string str = path_it->value.GetString(); // TODO this returns a char*
    auto node = ossia::net::find_node(root, str);
    if(node)
    {
      node->clearChildren();
      node->removeAddress();

      detail::json_parser_impl::readObject(*node, obj);
    }
    else
    {
      throw ossia::node_not_found_error{str + "not found"};
    }

  }
  else
  {
    root.clearChildren();
    root.removeAddress();

    detail::json_parser_impl::readObject(root, obj);
  }
}

void json_parser::parseValue(net::address_base& addr, const rapidjson::Value& obj)
{
  // TODO
}

}
}
