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

  static void parse_namespace(ossia::net::node_base& root, const rapidjson::Value& obj);

  static void parse_value(ossia::net::address_base& addr, const rapidjson::Value& obj);

  static void parse_path_added(ossia::net::node_base& map, const rapidjson::Value& obj);

  static void parse_path_removed(ossia::net::node_base& map, const rapidjson::Value& obj);

  template<typename Map>
  static void parse_path_changed(Map& map, const rapidjson::Value& mess)
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


  static void parse_attributes_changed(ossia::net::node_base& map, const rapidjson::Value& obj);


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
