#include "json_reader_detail.hpp"
#include "json_reader.hpp"
#include <ossia/network/oscquery/detail/attributes.hpp>
#include <ossia/network/oscquery/detail/value_to_json.hpp>
#include <ossia/network/base/node_attributes.hpp>
#include <ossia/editor/dataspace/dataspace.hpp>
#include <ossia/editor/dataspace/dataspace_visitors.hpp>
#include <ossia/network/domain/domain.hpp>
#include <oscpack/osc/OscTypes.h>
#include <brigand/algorithms/for_each.hpp>
#include <rapidjson/document.h>

namespace ossia
{
namespace oscquery
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
    res = getString(val);
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
    res = parse_pretty_unit(getStringView(val));
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

using map_setter_fun = void(*)(const rapidjson::Value&, ossia::net::node_base&);
template<typename Attr>
static auto make_setter_pair()
{
  return std::make_pair(metadata<Attr>::key(),
                        [] (const rapidjson::Value& val, ossia::net::node_base& node) {
    typename Attr::type res;
    if(json_parser_impl::ReadValue(val, res))
      Attr::setter(node, std::move(res));
  });
}

using reader_map_type = tsl::hopscotch_map<ossia::string_view, map_setter_fun>;

static auto& namespaceSetterMap()
{
  static const reader_map_type map{
    [] {
      reader_map_type attr_impl;
      using namespace ossia::net;

      // Remaining metadata
      brigand::for_each< brigand::list<
          domain_attribute,
          access_mode_attribute,
          bounding_mode_attribute,
          repetition_filter_attribute,
          tags_attribute,
          refresh_rate_attribute,
          priority_attribute,
          value_step_size_attribute,
          instance_bounds_attribute,
          critical_attribute,
          description_attribute
          > >([&] (auto attr) {
        using type = typename decltype(attr)::type;
        attr_impl.insert(make_setter_pair<type>());
      });

      return attr_impl;
    }()};

  return map;
}

static auto& attributesSetterMap()
{
  // This one has the missing attributes.
  // They are not in the previous map because we want to
  // handle them separately when parsing the namespace.
  static const reader_map_type map{
    [] {
      using namespace ossia::net;

      reader_map_type attr_impl = namespaceSetterMap();
      // Remaining metadata
      brigand::for_each< brigand::list<
          typetag_attribute,
          extended_type_attribute,
          unit_attribute
          > >([&] (auto attr) {
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
    const auto n = val.Size();
    if(n >= 2 && n <= 4)
    {
      bool ok = ossia::all_of(val.GetArray(), [] (const rapidjson::Value& v) {
        return v.IsFloat();
      });

      if(ok)
      {
        switch(n)
        {
          case 2: return ossia::val_type::VEC2F;
          case 3: return ossia::val_type::VEC3F;
          case 4: return ossia::val_type::VEC4F;
          default: break;
        }
      }
    }

  }
  return ossia::val_type::TUPLE;
}

void json_parser_impl::readObject(net::node_base& node, const rapidjson::Value& obj)
{
  auto& map = namespaceSetterMap();

  // If it's a real parameter
  if(obj.FindMember(detail::attribute_full_path()) != obj.MemberEnd())
  {
    // Read all the attributes
    // First initialize the address if it's not an empty node
    ossia::string_view typetag;

    // Try to read all the attributes that could give us the concrete type.
    complex_type val_type; // Implementation type
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
      val_type = get_type_from_osc_typetag(typetag);
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
      ext_type = getString(ext_type_it->value);
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
        addr = setup_address(val_type, node);
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
      auto action = map.find(getString(it->name)); // TODO string -> string view
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
      auto cld = node.createChild(getString(child_it->name));
      readObject(*cld, child_it->value);
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
    { std::string(detail::attributes_changed()), message_type::AttributesChanged }
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
    auto mt_it = map.find(getString(it->value)); // TODO string_view
    if(mt_it != map.end())
      return mt_it.value();
  }

  if(obj.FindMember(detail::osc_port()) != obj.MemberEnd())
  {
    return message_type::Device;
  }

  return message_type::Namespace; // TODO More checks needed
}

void json_parser::parse_namespace(net::node_base& root, const rapidjson::Value& obj)
{
  // Get the point from which we must update the namespace.
  auto path_it = obj.FindMember(detail::attribute_full_path());
  if(path_it != obj.MemberEnd())
  {
    auto str = getStringView(path_it->value);
    auto node = ossia::net::find_node(root, str);
    if(node)
    {
      node->clearChildren();
      node->removeAddress();

      detail::json_parser_impl::readObject(*node, obj);
    }
    else
    {
      throw ossia::node_not_found_error{str.to_string() + "not found"};
    }

  }
  else
  {
    root.clearChildren();
    root.removeAddress();

    detail::json_parser_impl::readObject(root, obj);
  }
}

void json_parser::parse_value(net::address_base& addr, const rapidjson::Value& obj)
{
  auto val = addr.cloneValue();
  val.apply(detail::json_to_value_unchecked{obj});
  addr.setValue(std::move(val));
}

// Given a string "/foo/bar/baz", return {"/foo/bar", "baz"}
static auto splitParentChild(ossia::string_view s) ->
optional<
std::pair<
ossia::string_view, ossia::string_view
>
>
{
  auto last_slash_pos = s.rfind('/');
  if(last_slash_pos != std::string::npos)
  {
    return std::make_pair(s.substr(0, last_slash_pos), s.substr(last_slash_pos + 1));
  }
  return ossia::none;
}

void json_parser::parse_path_added(net::node_base& root, const rapidjson::Value& obj)
{
  auto dat_it = obj.FindMember(detail::data());
  if(dat_it != obj.MemberEnd())
  {
    auto& dat = dat_it->value;
    auto path_it = dat.FindMember(detail::attribute_full_path());
    if(path_it != dat.MemberEnd())
    {
      auto opt_str = splitParentChild(getStringView(path_it->value));
      if(opt_str)
      {
        auto& str = *opt_str;
        auto node = ossia::net::find_node(root, str.first);
        if(node)
        {
          auto cld = node->createChild(str.second.to_string());
          detail::json_parser_impl::readObject(*cld, dat);
        }
      }
    }
  }
}

void json_parser::parse_path_removed(net::node_base& root, const rapidjson::Value& obj)
{
  auto dat_it = obj.FindMember(detail::data());
  if(dat_it != obj.MemberEnd())
  {
    auto path = getStringView(dat_it->value);
    if(auto node = ossia::net::find_node(root, path))
    {
      if(auto parent = node->getParent())
      {
        parent->removeChild(*node);
      }
    }
  }
}


void json_parser::parse_attributes_changed(net::node_base& root, const rapidjson::Value& obj)
{
  auto dat_it = obj.FindMember(detail::data());
  if(dat_it != obj.MemberEnd())
  {
    auto& dat = dat_it->value;
    auto path_it = dat.FindMember(detail::attribute_full_path());
    if(path_it != dat.MemberEnd())
    {
      auto path = getStringView(path_it->value);
      auto node = ossia::net::find_node(root, path);
      if(node)
      {
        auto& map = detail::attributesSetterMap();
        auto memb_end = dat.MemberEnd();
        for(auto it = dat.MemberBegin(); it != memb_end; ++it)
        {
          auto action = map.find(getString(it->name)); // TODO string -> string view
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
}
