#pragma once
#include <ossia/network/oscquery/detail/json_reader_detail.hpp>
#include <ossia/network/oscquery/detail/attributes.hpp>
#include <memory>
namespace ossia
{
namespace oscquery
{

class json_parser
{
public:
  static std::shared_ptr<rapidjson::Document> parse(std::string& message);

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

};

}
}
