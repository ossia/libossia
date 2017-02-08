#pragma once
#include <ossia/network/oscquery/detail/attributes.hpp>
#include <ossia/network/oscquery/detail/value_to_json.hpp>
#include <ossia/editor/dataspace/dataspace_visitors.hpp>
#include <ossia/detail/json.hpp>
#include <ossia/network/exceptions.hpp>
#include <ossia/network/osc/detail/osc.hpp>
#include <brigand/algorithms/for_each.hpp>

#include <rapidjson/document.h>

namespace ossia
{
namespace oscquery
{
inline void json_assert(bool val)
{
  if(!val)
    throw ossia::bad_request_error{"Bad request"};
}

namespace detail
{

struct json_parser_impl
{
  static optional<ossia::val_type> TypeTagToType(ossia::string_view str)
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

  static bool ReadValue(const rapidjson::Value& val, int32_t& res)
  {
    bool b = val.IsInt();
    if(b)
      res = val.GetInt();
    return b;
  }

  static bool ReadValue(const rapidjson::Value& val, float& res)
  {
    bool b = val.IsDouble();
    if(b)
      res = val.GetDouble();
    return b;
  }

  static bool ReadValue(const rapidjson::Value& val, std::string& res)
  {
    bool b = val.IsString();
    if(b)
      res = std::string(val.GetString(), val.GetStringLength());
    return b;
  }

  static bool ReadValue(const rapidjson::Value& val, bool& res)
  {
    bool b = val.IsBool();
    if(b)
      res = val.GetBool();
    return b;
  }

  static bool ReadValue(const rapidjson::Value& val, ossia::repetition_filter& res)
  {
    bool b = val.IsBool();
    if(b)
      res = val.GetBool() ? ossia::repetition_filter::ON : ossia::repetition_filter::OFF;
    return b;
  }

  static ossia::value ReadValue(const rapidjson::Value& val)
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

  static bool ReadValue(const rapidjson::Value& val, ossia::bounding_mode& res)
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

  static bool ReadValue(const rapidjson::Value& val, ossia::access_mode& am)
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

  static bool ReadValue(const rapidjson::Value& val, ossia::net::domain& res)
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

  static bool ReadValue(const rapidjson::Value& val, ossia::unit_t& res)
  {
    bool b = val.IsString();
    if(b)
      res = parse_pretty_unit(val.GetString());
    return b;
  }

  static bool ReadValue(const rapidjson::Value& val, ossia::net::tags& res)
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
  static bool ReadValue(const rapidjson::Value& val, ossia::net::instance_bounds& res)
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


  template<typename Attr>
  static auto make_setter_pair()
  {
    return std::make_pair(Attr::key(),
                          [] (const rapidjson::Value& val, ossia::net::node_base& node) {
      typename Attr::type res;
      if(ReadValue(val, res))
        Attr::setter(node, std::move(res));
    });
  }

  using map_type = tsl::hopscotch_map<
  ossia::string_view,
  void(*)(const rapidjson::Value&, ossia::net::node_base&)>;


  static void readObject(ossia::net::node_base& node, const rapidjson::Value& obj)
  {
    static map_type map{
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

      auto type_it = obj.FindMember(detail::attribute_typetag());
      if(type_it != obj.MemberEnd())
      {
        typetag = getStringView(type_it->value);
        val_type = TypeTagToType(typetag);
      }

      auto unit_it = obj.FindMember(detail::attribute_unit());
      if(unit_it != obj.MemberEnd())
      {
        ossia::unit_t u;
        ReadValue(unit_it->value, u);
        if(u)
          unit = std::move(u);
      }

      auto ext_type_it = obj.FindMember(detail::attribute_extended_type());
      if(ext_type_it != obj.MemberEnd() && ext_type_it->value.IsString())
      {
        ext_type = std::string(ext_type_it->value.GetString(), ext_type_it->value.GetStringLength());
      }

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
            if(typetag == "ff" || typetag == "[ff]") actual_type = ossia::val_type::VEC2F;
            else if(typetag == "fff" || typetag == "[fff]") actual_type = ossia::val_type::VEC3F;
            else if(typetag == "ffff" || typetag == "[ffff]") actual_type = ossia::val_type::VEC4F;
            else
            {
              int n = 0;
              if(value_it != obj.MemberEnd() && value_it->value.IsArray())
              {
                n = value_it->value.Size();
              }
              else if(default_value_it != obj.MemberEnd() && default_value_it->value.IsArray())
              {
                n = default_value_it->value.Size();
              }

              switch(n)
              {
                case 2: actual_type = ossia::val_type::VEC2F; break;
                case 3: actual_type = ossia::val_type::VEC3F; break;
                case 4: actual_type = ossia::val_type::VEC4F; break;
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
};
}

using val_t = rapidjson::Value;


class json_parser
{
public:
  static rapidjson::Document parse(const std::string& message)
  {
    rapidjson::Document document;
    document.Parse(message);
    return document;
  }

  static int getPort(const rapidjson::Value& obj)
  {
    using namespace detail;
    json_assert(obj[detail::osc_port()].IsInt());

    return obj[detail::osc_port()].GetInt();
  }

  static MessageType messageType(const rapidjson::Value& obj)
  {
    static tsl::hopscotch_map<std::string, MessageType> map{
      { std::string(detail::path_added()), MessageType::PathAdded },
      { std::string(detail::path_changed()), MessageType::PathChanged },
      { std::string(detail::path_removed()), MessageType::PathRemoved },
      { std::string(detail::attributes_changed()), MessageType::AttributesChanged },
      { std::string(detail::paths_added()), MessageType::PathsAdded },
      { std::string(detail::paths_changed()), MessageType::PathsChanged },
      { std::string(detail::paths_removed()), MessageType::PathsRemoved },
      { std::string(detail::attributes_changed_array()), MessageType::AttributesChangedArray }
    };
    using namespace detail;
    auto val_it = obj.FindMember(detail::attribute_value());
    if(val_it != obj.MemberEnd())
    {
      return MessageType::Value;
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
      return MessageType::Device;
    }

    return MessageType::Namespace; // TODO More checks needed
  }

  static void parseNamespace(ossia::net::node_base& root, const rapidjson::Value& obj)
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

  static void parseValue(ossia::net::address_base& addr, const rapidjson::Value& obj)
  {
    // TODO
  }

  template<typename BaseMapType, typename Map>
  static void path_added(Map& map, const rapidjson::Value& obj)
  {
    /*
      using namespace detail;
      map.merge(parseNamespace<BaseMapType>(obj.get<json_map>(detail::path_added())));
      */
  }

  template<typename Map>
  static void path_changed(Map& map, const rapidjson::Value& mess)
  {
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

  template<typename Map>
  static void path_removed(Map& map, const rapidjson::Value& obj)
  {
    /*
      using namespace detail;

      const auto& path = valToString(obj.get(detail::path_removed()));
      json_assert(map.existing_path(path));
      map.remove(path);
      */
  }

  template<typename Map>
  static void attributes_changed(Map& map, const rapidjson::Value& obj)
  {
    /*
      using namespace detail;
      const auto& attr_changed = obj.get<json_map>(detail::attributes_changed());

      // 1. Search for the paths
      for(const auto& path : attr_changed.get_keys())
      {
        json_assert(map.has(path));
        const auto& path_obj = attr_changed.template get<json_map>(path);

        // A lambda used to update the boost map.
        // Here, since we are in attributes_changed, we will just ignore
        // the missing members.
        auto mapper = [&] (const std::string& name, auto&& member, auto&& method)
        {
          if(path_obj.find(name) != path_obj.end())
            map.update(path, [&] (Parameter& p) { p.*member = method(path_obj.template get(name)); });
        };
        // 2. Map the values
        // TODO check that they are correct (and put in detail).
        mapper(detail::attribute<Description>(), &Parameter::description, &valToString);
        mapper(detail::attribute<Tags>(),        &Parameter::tags,        &jsonToTags);
        mapper(detail::attribute<Access>(),      &Parameter::access,  &jsonToAccessMode);

        auto type_it = obj.find(detail::type());
        if(type_it != obj.end())
        {
          // We can be type-safe
          auto type_vec = jsonToTypeVector(type_it->second);
          auto mapper_tyepsafe = [&] (const std::string& name, auto&& member, auto&& method)
          {
            if(path_obj.find(name) != path_obj.end())
              map.update(path, [&] (Parameter& p) { p.*member = method(path_obj.template get(name), type_vec); });
          };

          mapper_tyepsafe(detail::attribute<Values>(),      &Parameter::values,      &jsonToVariantArray_checked);
          mapper_tyepsafe(detail::attribute<Ranges>(),      &Parameter::ranges,      &jsonToRangeArray_checked);
        }
        else
        {
          // Degrade to unsafe parsing that will just replace the values without checking.
          // bool, nil are unexpressable in such mode.
          mapper(detail::attribute<Values>(),      &Parameter::values,      &jsonToVariantArray);
          mapper(detail::attribute<Ranges>(),      &Parameter::ranges,      &jsonToRangeArray);
        }

        mapper(detail::attribute<ClipModes>(),   &Parameter::clipmodes,   &jsonToClipModeArray);
      }
      */
  }


  // Plural forms
  template<typename BaseMapType, typename Map>
  static void paths_added(Map& map, const rapidjson::Value& obj)
  {
    /*
      using namespace detail;

      const auto& arr = detail::valToArray(obj[detail::paths_added()]);
      for(const auto& elt : arr)
      {
        map.merge(parseNamespace<BaseMapType>(elt.as<json_map>()));
      }
      */
  }

  template<typename Map>
  static void paths_changed(Map& map, const rapidjson::Value& obj)
  {
    std::cerr << "TODO" << std::endl;
  }

  template<typename Map>
  static void paths_removed(Map& map, const rapidjson::Value& obj)
  {
    /*
      const auto& arr = detail::valToArray(obj[detail::paths_removed()]);

      for(const auto& elt : arr)
      {
        const auto& path = detail::valToString(elt);
        json_assert(map.existing_path(path));
        map.remove(path);
      }
      */
  }

  template<typename Map>
  static void attributes_changed_array(Map& map, const rapidjson::Value& obj)
  {
    std::cerr << "TODO" << std::endl;
  }
};
}
}
