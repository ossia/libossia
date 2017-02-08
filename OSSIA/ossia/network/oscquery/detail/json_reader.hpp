#pragma once
#include <ossia/network/oscquery/detail/json_reader_detail.hpp>
#include <ossia/network/oscquery/detail/attributes.hpp>
namespace ossia
{
namespace oscquery
{

class json_parser
{
public:
  static rapidjson::Document parse(const std::string& message);

  static int getPort(const rapidjson::Value& obj);

  static message_type messageType(const rapidjson::Value& obj);

  static void parseNamespace(ossia::net::node_base& root, const rapidjson::Value& obj);

  static void parseValue(ossia::net::address_base& addr, const rapidjson::Value& obj);

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
