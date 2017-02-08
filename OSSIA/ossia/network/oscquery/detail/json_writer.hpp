#pragma once
#include <ossia/network/oscquery/detail/domain_to_json.hpp>
#include <ossia/network/oscquery/detail/attributes.hpp>
#include <ossia/network/exceptions.hpp>
#include <ossia/network/domain/domain.hpp>
#include <ossia/editor/value/value.hpp>
#include <ossia/editor/dataspace/dataspace_visitors.hpp>
#include <ossia/detail/string_view.hpp>
#include <brigand/algorithms/for_each.hpp>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
namespace ossia
{
namespace oscquery
{

namespace detail
{

struct json_writer_impl
{
  using writer_t = rapidjson::Writer<rapidjson::StringBuffer>;
  writer_t& writer;


  void writeValue(const ossia::value& val) const
  {
    val.apply(value_to_json{writer});
  }

  void writeValue(ossia::bounding_mode b) const
  {
    switch(b)
    {
      case ossia::bounding_mode::FREE: writer.String("None"); break;
      case ossia::bounding_mode::CLIP: writer.String("Both"); break;
      case ossia::bounding_mode::LOW:  writer.String("Low");  break;
      case ossia::bounding_mode::HIGH: writer.String("High"); break;
      case ossia::bounding_mode::WRAP: writer.String("Wrap"); break;
      case ossia::bounding_mode::FOLD: writer.String("Fold"); break;
      default: throw;
    }
  }

  void writeValue(ossia::access_mode b) const
  {
    switch(b)
    {
      case ossia::access_mode::GET: writer.Int(1); break;
      case ossia::access_mode::SET: writer.Int(2); break;
      case ossia::access_mode::BI: writer.Int(3);  break;
      default: writer.Int(0); // no address case
    }
  }

  void writeValue(const ossia::net::domain& d) const
  {
    writer.StartArray();
    ossia::apply(domain_to_json{writer}, d);
    writer.EndArray();
  }

  void writeValue(const ossia::unit_t& d) const
  {
    writer.String(ossia::get_pretty_unit_text(d));
  }

  void writeValue(const ossia::net::tags& tags) const
  {
    writer.StartArray();

    for(const auto& tag : tags)
    {
      writer.String(tag);
    }

    writer.EndArray();
  }

  void writeValue(int32_t i) const { writer.Int(i); }
  void writeValue(float i) const { writer.Double(i); }
  void writeValue(bool i) const { writer.Bool(i); }
  void writeValue(const std::string& i) const { writer.String(i); }
  void writeValue(const ossia::repetition_filter & i) const
  { writeValue(i == repetition_filter::ON); }

  void writeValue(const ossia::net::instance_bounds & i) const
  {
    writer.StartArray();
    writer.Int(i.min_instances);
    writer.Int(i.max_instances);
    writer.EndArray();
  }

  template<typename T>
  void writeValue(const optional<T>& t) const
  {
    if(t) { writeValue(*t); }
    else  { writer.Null(); }
  }


  template<typename Attr>
  static auto make_fun_pair()
  {
    return std::make_pair(Attr::key(),
                          [] (const json_writer_impl& self, const ossia::net::node_base& n) {
      self.writeValue(Attr::getter(n));
    });
  }

  void attributeToJsonValue(
      const ossia::net::node_base& n,
      ossia::string_view method
      ) const
  {
    // We put all our attributes in a map.
    using map_type = tsl::hopscotch_map<ossia::string_view, void(*)(const json_writer_impl&, const ossia::net::node_base& )>;
    static map_type attr_map{
      [] {
        map_type attr_impl;

        attr_impl.insert(make_fun_pair<full_path_attribute>());

        brigand::for_each<base_attributes>([&] (auto attr) {
          using type = typename decltype(attr)::type;
          attr_impl.insert(make_fun_pair<type>());
        });
        brigand::for_each<extended_attributes>([&] (auto attr) {
          using type = typename decltype(attr)::type;
          attr_impl.insert(make_fun_pair<type>());
        });

        return attr_impl;
      }()
    };

    // Look into the map and call writeValue(theAttribute), c.f. make_fun_pair.
    auto it = attr_map.find(method);
    if(it != attr_map.end())
    {
      it.value()(*this, n);
    }
    else
    {
      throw ossia::bad_request_error{"Attribute not found"};
    }
  }
  void writeKey(ossia::string_view k) const
  {
    ::writeKey(writer, k);
  }

  void nodeAttributesToJson(
      const ossia::net::node_base& n) const
  {
    using namespace std;
    using namespace boost;
    using namespace eggs::variants;

    auto addr = n.getAddress();

    // We are already in an object
    // These attributes are always here
    writeKey(detail::attribute_full_path());

    writer.String(ossia::net::osc_address_string(n));

    // Handling of the types / values
    if(addr)
    {
      // TODO it could be nice to have versions that take an address or a value directly
      brigand::for_each<base_attributes>([&] (auto attr) {
        using type = typename decltype(attr)::type;
        this->writeKey(type::key());
        this->writeValue(type::getter(n));
      });
    }
    else
    {
      writeKey(detail::attribute_accessmode());
      writer.Int(0);
    }

    brigand::for_each<extended_attributes>([&] (auto attr) {
      using type = typename decltype(attr)::type;
      auto res = type::getter(n);
      if(res)
      {
        this->writeKey(type::key());
        this->writeValue(*res);
      }
    });
  }

  void nodeToJson(const ossia::net::node_base& n)
  {
    writer.StartObject();
    nodeAttributesToJson(n);

    auto& cld = n.children();

    if(!cld.empty())
    {
      writeKey(detail::contents());
      writer.StartObject();
      for(const auto& child : cld)
      {
        writer.String(child->getName());
        nodeToJson(*child);
      }
      writer.EndObject();
    }

    writer.EndObject();
  }


};
} // End of namespace detail

class writer
{
private:/*
    // The following three methods are here
    // to make the attributes_changed message.
    // End of recursion
    template<typename Attribute>
    static void addAttributes(
            json_map& map,
            const Attribute& attr)
    {
        using namespace detail;
        map[key::attribute(attr)] = attributeToJson(attr);
    }

    template<typename Attribute, typename... Attributes>
    static void addAttributes(
            json_map& map,
            const Attribute& attr,
            Attributes&&... attrs)
    {
        using namespace detail;
        map[attributeToKey(attr)] = attributeToJson(attr);
        addAttributes(std::forward<Attributes>(attrs)...);
    }
*/
public:
  // Initialisation
  static auto device_info(int port)
  {
    rapidjson::StringBuffer buf;
    rapidjson::Writer<rapidjson::StringBuffer> wr(buf);

    wr.StartObject();
    writeKey(wr, detail::osc_port());
    wr.Int(port);
    wr.EndObject();

    return buf;
  }

  // Format interface
  // Queries
  using string_type = rapidjson::StringBuffer;

  static auto query_namespace(
      const ossia::net::node_base& node)
  {
    rapidjson::StringBuffer buf;
    rapidjson::Writer<rapidjson::StringBuffer> wr(buf);

    detail::json_writer_impl p{wr};

    p.nodeToJson(node);

    return buf;
  }

  template<typename StringVec_T>
  static auto query_attributes(
      const ossia::net::node_base& node,
      const StringVec_T& methods)
  {
    rapidjson::StringBuffer buf;
    rapidjson::Writer<rapidjson::StringBuffer> wr(buf);

    detail::json_writer_impl p{wr};

    wr.StartObject();
    for(auto& method : methods)
    {
      writeKey(wr, method);
      p.attributeToJsonValue(node, method);
    }
    wr.EndObject();

    return buf;
  }

  // Update messages
  static auto path_added(const ossia::net::node_base& n)
  {
    rapidjson::StringBuffer buf;
    rapidjson::Writer<rapidjson::StringBuffer> wr(buf);

    detail::json_writer_impl p{wr};

    wr.StartObject();
    writeKey(wr, detail::command());
    writeRef(wr, detail::path_added());

    writeKey(wr, detail::data());
    p.nodeToJson(n);
    wr.EndObject();

    return buf;
  }

  static auto path_changed(const ossia::net::node_base& n)
  {
    rapidjson::StringBuffer buf;
    rapidjson::Writer<rapidjson::StringBuffer> wr(buf);

    detail::json_writer_impl p{wr};

    wr.StartObject();

    writeKey(wr, detail::command());
    writeRef(wr, detail::path_changed());

    writeKey(wr, detail::data());
    p.nodeToJson(n);

    wr.EndObject();

    return buf;
  }

  static auto path_removed(
      const std::string& path)
  {
    rapidjson::StringBuffer buf;
    rapidjson::Writer<rapidjson::StringBuffer> wr(buf);

    wr.StartObject();

    writeKey(wr, detail::command());
    writeRef(wr, detail::path_removed());

    writeKey(wr, detail::data());
    wr.String(path);

    wr.EndObject();

    return buf;
  }

  /*
    template<typename... Attributes>
    static std::string attributes_changed(
            const std::string& path,
            Attributes&&... attrs)
    {
        rapidjson::StringBuffer buf;
        rapidjson::Writer<rapidjson::StringBuffer> wr(buf);

        wr.StartObject();
        d.AddMember(StringRef(key::command()), StringRef(key::attributes_changed()), alloc);

        rapidjson::Value v(rapidjson::kObjectType);
        v.AddMember(StringRef(key::full_path()), rapidjson::Value(path, alloc), alloc);
        d.AddMember(StringRef(key::data()), rapidjson::Value(path, alloc), alloc);

        // TODO what if type changed? We have to add the type array.
        json_map objmap;
        objmap[key::full_path()] = path;

        addAttributes(objmap, std::forward<Attributes>(attrs)...);

        json_map map;
        map[key::attributes_changed()] = objmap;
        return map.to_string();
    }


    template<typename Map, typename Vector>
    static std::string paths_added(
            const Map& theMap,
            const Vector& vec)
    {
        /*
        using namespace detail;
        json_array arr;
        for(const auto& elt : vec)
        {
            arr.push_back(mapToJson(theMap, elt));
        }

        json_map map;
        map[key::paths_added()] = arr;
        return map.to_string();
    }

    template<typename Map, typename Vector>
    static std::string paths_changed(
            const Map& theMap,
            const Vector& vec)
    {
        using namespace detail;
        json_array arr;
        for(const auto& elt : vec)
        {
            arr.push_back(mapToJson(theMap, elt));
        }

        json_map map;
        map[key::paths_changed()] = arr;
        return map.to_string();
    }

    template<typename Vector>
    static std::string paths_removed(
            const Vector& vec)
    {
        using namespace detail;
        json_array arr;
        for(const auto& elt : vec)
        {
            arr.push_back(elt);
        }

        json_map map;
        map[key::paths_removed()] = arr;
        return map.to_string();
    }

    template<typename... Attributes>
    static std::string attributes_changed_array(
            const std::string& path,
            Attributes&&... attrs)
    {
        using namespace detail;
        // TODO what if type changed? We have to add the type array.
        json_map objmap;
        objmap[key::full_path()] = path;

        addAttributes(objmap, std::forward<Attributes>(attrs)...);

        json_map map;
        map[key::attributes_changed()] = objmap;
        return map.to_string();
    }
    */
};
}
}
