#pragma once
#include <ossia/detail/json.hpp>
#include <ossia/network/common/address_properties.hpp>
#include <ossia/editor/value/value.hpp>
#include <ossia/network/domain/domain.hpp>
#include <ossia/network/base/node.hpp>
#include <ossia/network/base/address.hpp>
#include <ossia/network/base/node_attributes.hpp>
#include <ossia/editor/dataspace/dataspace_visitors.hpp>
#include <ossia/detail/string_view.hpp>
#include <ossia/network/osc/detail/osc.hpp>
#include <oscpack/osc/OscTypes.h>
#include <brigand/algorithms/for_each.hpp>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <ossia/network/exceptions.hpp>

namespace ossia
{
namespace oscquery
{

using allocator = rapidjson::MemoryPoolAllocator<>;
namespace key
{
// Attributes
constexpr ossia::string_view osc_port() { return "OSC_PORT"; }
constexpr auto attribute_full_path() { return "FULL_PATH"; }

constexpr auto attribute_typetag() { return "TYPE"; }
constexpr auto contents() { return "CONTENTS"; }

constexpr auto attribute_value() { return "VALUE"; }
constexpr auto attribute_range() { return "RANGE"; }
constexpr auto attribute_clipmode() { return "CLIPMODE"; }
constexpr auto attribute_accessmode() { return "ACCESS"; }
constexpr auto attribute_description() { return "DESCRIPTION"; }
constexpr auto attribute_tags() { return "TAGS"; }

// Not part of the OSCQuery spec :
constexpr auto attribute_unit() { return "UNIT"; }
constexpr auto attribute_refresh_rate() { return "REFRESH_RATE"; }
constexpr auto attribute_priority() { return "PRIORITY"; }
constexpr auto attribute_step_size() { return "STEP_SIZE"; }
constexpr auto attribute_instance_bounds() { return "INSTANCE_BOUNDS"; }
constexpr auto attribute_critical() { return "CRITICAL"; }
constexpr auto attribute_extended_type() { return "EXTENDED_TYPE"; }
constexpr auto attribute_repetition_filter() { return "REPETITION_FILTER"; }
constexpr auto attribute_app_name() { return "APP_NAME"; }
constexpr auto attribute_app_version() { return "APP_VERSION"; }
constexpr auto attribute_app_creator() { return "APP_CREATOR"; }
constexpr auto attribute_default_value() { return "DEFAULT_VALUE"; }


// Commands
constexpr ossia::string_view command() { return "COMMAND"; }
constexpr ossia::string_view data() { return "DATA"; }

constexpr ossia::string_view path_added() { return "PATH_ADDED"; }
constexpr ossia::string_view path_removed() { return "PATH_REMOVED"; }
constexpr ossia::string_view path_changed() { return "PATH_CHANGED"; }
constexpr ossia::string_view attributes_changed() { return "ATTRIBUTES_CHANGED"; }

constexpr ossia::string_view paths_added() { return "PATHS_ADDED"; }
constexpr ossia::string_view paths_removed() { return "PATHS_REMOVED"; }
constexpr ossia::string_view paths_changed() { return "PATHS_CHANGED"; }
constexpr ossia::string_view attributes_changed_array() { return "ATTRIBUTES_CHANGED_ARRAY"; }
}

inline rapidjson::GenericStringRef<char> StringRef(ossia::string_view s)
{ return rapidjson::GenericStringRef<char>(s.data() ? s.data() : "", s.size()); }

namespace detail
{
struct OSSIA_EXPORT full_path_attribute
{
    static constexpr auto key() { return key::attribute_full_path(); }
    static constexpr const auto func = static_cast<std::string(*)(const ossia::net::node_base&)>(ossia::net::osc_address_string);
};

// Attributes of an address
struct OSSIA_EXPORT value_attribute
{
    static constexpr auto key() { return key::attribute_value(); }
    static constexpr const auto func = ossia::net::clone_value;
};

struct OSSIA_EXPORT typetag_attribute
{
    static constexpr auto key() { return key::attribute_typetag(); }
    static constexpr const auto func = ossia::net::get_osc_typetag;
};

struct OSSIA_EXPORT domain_attribute
{
    static constexpr auto key() { return key::attribute_range(); }
    static constexpr const auto func = ossia::net::get_domain;
};

struct OSSIA_EXPORT access_mode_attribute
{
    static constexpr auto key() { return key::attribute_accessmode(); }
    static constexpr const auto func = ossia::net::get_access_mode;
};

struct OSSIA_EXPORT bounding_mode_attribute
{
    static constexpr auto key() { return key::attribute_clipmode(); }
    static constexpr const auto func = ossia::net::get_bounding_mode;
};

struct OSSIA_EXPORT unit_attribute
{
    static constexpr auto key() { return key::attribute_unit(); }
    static constexpr const auto func = ossia::net::get_unit;
};

struct OSSIA_EXPORT default_value_attribute
{
    static constexpr auto key() { return key::attribute_default_value(); }
    static constexpr const auto func = ossia::net::get_default_value;
};

// Metadata attributes
struct OSSIA_EXPORT tags_attribute
{
    static constexpr auto key() { return key::attribute_tags(); }
    static constexpr const auto func = ossia::net::get_tags;
};
struct OSSIA_EXPORT refresh_rate_attribute
{
    static constexpr auto key() { return key::attribute_refresh_rate(); }
    static constexpr const auto func = ossia::net::get_refresh_rate;
};
struct OSSIA_EXPORT priority_attribute
{
    static constexpr auto key() { return key::attribute_priority(); }
    static constexpr const auto func = ossia::net::get_priority;
};
struct OSSIA_EXPORT step_size_attribute
{
    static constexpr auto key() { return key::attribute_step_size(); }
    static constexpr const auto func = ossia::net::get_value_step_size;
};
struct OSSIA_EXPORT instance_bounds_attribute
{
    static constexpr auto key() { return key::attribute_instance_bounds(); }
    static constexpr const auto func = ossia::net::get_instance_bounds;
};
struct OSSIA_EXPORT critical_attribute
{
    static constexpr auto key() { return key::attribute_critical(); }
    static constexpr const auto func = ossia::net::get_critical;
};
struct OSSIA_EXPORT extended_type_attribute
{
    static constexpr auto key() { return key::attribute_extended_type(); }
    static constexpr const auto func = ossia::net::get_extended_type;
};
struct OSSIA_EXPORT description_attribute
{
    static constexpr auto key() { return key::attribute_description(); }
    static constexpr const auto func = ossia::net::get_description;
};
struct OSSIA_EXPORT repetition_filter_attribute
{
    static constexpr auto key() { return key::attribute_repetition_filter(); }
    static constexpr const auto func = ossia::net::get_repetition_filter;
};

using base_attributes = brigand::list<
    typetag_attribute,
    value_attribute,
    domain_attribute,
    access_mode_attribute,
    bounding_mode_attribute,
    repetition_filter_attribute,
    unit_attribute,
    default_value_attribute
>;

using extended_attributes = brigand::list<
    tags_attribute,
    refresh_rate_attribute,
    priority_attribute,
    step_size_attribute,
    instance_bounds_attribute,
    critical_attribute,
    extended_type_attribute,
    description_attribute>;

inline void writeKey(rapidjson::Writer<rapidjson::StringBuffer>& writer, ossia::string_view k)
{
    writer.Key(k.data(), k.size());
}

inline void writeRef(rapidjson::Writer<rapidjson::StringBuffer>& writer, ossia::string_view k)
{
    writer.String(k.data(), k.size());
}

inline void writeChar(rapidjson::Writer<rapidjson::StringBuffer>& writer, char c)
{
    writer.String(&c, 1);
}


// TODO base64 encode
struct to_json_value
{
    rapidjson::Writer<rapidjson::StringBuffer>& writer;
    void operator()(impulse) const { writer.Null(); }
    void operator()(int v) const { writer.Int(v); }
    void operator()(float v) const { writer.Double(v); }
    void operator()(bool v) const { writer.Bool(v); }
    void operator()(char v) const { writeChar(writer, v); }
    void operator()(const std::string& v) const
    {
        // TODO handle base 64
        // bool b = Base64::Encode(get<coppa::Generic>(val).buf, &out);
        writer.String(v);
    }

    template<std::size_t N>
    void operator()(const std::array<float, N>& vec) const {
        writer.StartArray();
        for(std::size_t i = 0; i < N; i++)
        {
            writer.Double(vec[i]);
        }
        writer.EndArray();
    }

    void operator()(const std::vector<ossia::value>& vec) const
    {
        writer.StartArray();
        for(const auto& sub : vec)
        {
            sub.apply(*this);
        }
        writer.EndArray();
    }

    void operator()(const ossia::Destination& d) const
    {
        throw;
    }


    void operator()() const
    {
        throw;
    }
};

// We're in an array
struct to_json_domain
{
    rapidjson::Writer<rapidjson::StringBuffer>& writer;
    void operator()()
    {
        writer.Null();
        writer.Null();
        writer.Null();
    }
    void operator()(const ossia::net::domain_base<impulse> & dom)
    {
        writer.Null();
        writer.Null();
        writer.Null();
    }
    void operator()(const ossia::net::domain_base<int32_t> & dom)
    {
        if(dom.min) writer.Int(*dom.min); else writer.Null();
        if(dom.max) writer.Int(*dom.max); else writer.Null();

        if(!dom.values.empty())
        {
            writer.StartArray();
            for(auto val : dom.values)
                writer.Int(val);
            writer.EndArray();
        }
        else
        {
            writer.Null(); // TODO why not just nothing ?
        }
    }
    void operator()(const ossia::net::domain_base<float> & dom)
    {
        if(dom.min) writer.Double(*dom.min); else writer.Null();
        if(dom.max) writer.Double(*dom.max); else writer.Null();

        if(!dom.values.empty())
        {
            writer.StartArray();
            for(auto val : dom.values)
                writer.Double(val);
            writer.EndArray();
        }
        else
        {
            writer.Null(); // TODO why not just nothing ?
        }
    }

    void operator()(const ossia::net::domain_base<char> & dom)
    {
        if(dom.min) writeChar(writer, *dom.min); else writer.Null();
        if(dom.max) writeChar(writer, *dom.max); else writer.Null();

        if(!dom.values.empty())
        {
            writer.StartArray();
            for(auto val : dom.values)
                writeChar(writer, val);
            writer.EndArray();
        }
        else
        {
            writer.Null(); // TODO why not just nothing ?
        }
    }

    void operator()(const ossia::net::domain_base<bool> & dom)
    {
        if(dom.min) writer.Bool(*dom.min); else writer.Null();
        if(dom.max) writer.Bool(*dom.max); else writer.Null();

        if(!dom.values.empty())
        {
            writer.StartArray();
            for(auto val : dom.values)
                writer.Bool(val);
            writer.EndArray();
        }
        else
        {
            writer.Null(); // TODO why not just nothing ?
        }
    }

    void operator()(const ossia::net::domain_base<std::string> & dom)
    {
        writer.Null();
        writer.Null();

        if(!dom.values.empty())
        {
            writer.StartArray();
            for(const auto& val : dom.values)
                writer.String(val);
            writer.EndArray();
        }
        else
        {
            writer.Null(); // TODO why not just nothing ?
        }
    }

    void operator()(const ossia::net::domain_base<std::vector<ossia::value>> & dom)
    {
        if(dom.min)
        {
            writer.StartArray();
            for(auto& val : *dom.min)
                val.apply(to_json_value{writer});
            writer.EndArray();
        }
        else
        {
            writer.Null();
        }

        if(dom.max)
        {
            writer.StartArray();
            for(auto& val : *dom.max)
                val.apply(to_json_value{writer});
            writer.EndArray();
        }
        else
        {
            writer.Null();
        }

        if(!dom.values.empty())
        {
            writer.StartArray();
            for(const auto& val : dom.values)
            {
                writer.StartArray();
                for(auto& sub : val)
                    sub.apply(to_json_value{writer});
                writer.EndArray();
            }
            writer.EndArray();
        }
        else
        {
            writer.Null(); // TODO why not just nothing ?
        }
    }

    template<std::size_t N>
    void operator()(const ossia::net::domain_base<std::array<float, N>> & dom)
    {
        if(dom.min)
        {
            auto& vec = *dom.min;
            writer.StartArray();
            for(std::size_t i = 0; i < N; i++)
                writer.Double(vec[i]);
            writer.EndArray();
        }
        else
        {
            writer.Null();
        }

        if(dom.max)
        {
            auto& vec = *dom.max;
            writer.StartArray();
            for(std::size_t i = 0; i < N; i++)
                writer.Double(vec[i]);
            writer.EndArray();
        }
        else
        {
            writer.Null();
        }

        if(!dom.values.empty())
        {
            writer.StartArray();
            for(const auto& vec : dom.values)
            {
                writer.StartArray();
                for(std::size_t i = 0; i < N; i++)
                    writer.Double(vec[i]);
                writer.EndArray();
            }
            writer.EndArray();
        }
        else
        {
            writer.Null(); // TODO why not just nothing ?
        }
    }
    void operator()(const ossia::net::domain_base<ossia::value> & dom)
    {
        if(dom.min)
        {
            dom.min->apply(to_json_value{writer});
        }
        else
        {
            writer.Null();
        }

        if(dom.max)
        {
            dom.max->apply(to_json_value{writer});
        }
        else
        {
            writer.Null();
        }

        if(!dom.values.empty())
        {
            writer.StartArray();
            for(const auto& val: dom.values)
            {
                val.apply(to_json_value{writer});
            }
            writer.EndArray();
        }
        else
        {
            writer.Null(); // TODO why not just nothing ?
        }
    }
};

struct json_writer_impl
{
    using writer_t = rapidjson::Writer<rapidjson::StringBuffer>;
    writer_t& writer;


    void writeValue(const ossia::value& val) const
    {
        val.apply(to_json_value{writer});
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
        ossia::apply(to_json_domain{writer}, d);
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
            self.writeValue(Attr::func(n));
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
        detail::writeKey(writer, k);
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
        writeKey(key::attribute_full_path());

        writer.String(ossia::net::osc_address_string(n));

        // Handling of the types / values
        if(addr)
        {
            // TODO it could be nice to have versions that take an address or a value directly
            brigand::for_each<base_attributes>([&] (auto attr) {
                using type = typename decltype(attr)::type;
                this->writeKey(type::key());
                this->writeValue(type::func(n));
            });
        }
        else
        {
            writeKey(key::attribute_accessmode());
            writer.Int(0);
        }

        brigand::for_each<extended_attributes>([&] (auto attr) {
            using type = typename decltype(attr)::type;
            auto res = type::func(n);
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
            writeKey(key::contents());
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
        detail::writeKey(wr, key::osc_port());
        wr.Int(port);

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
            detail::writeKey(wr, method);
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
        detail::writeKey(wr, key::command());
        detail::writeRef(wr, key::path_added());

        detail::writeKey(wr, key::data());
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

        detail::writeKey(wr, key::command());
        detail::writeRef(wr, key::path_changed());

        detail::writeKey(wr, key::data());
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

        detail::writeKey(wr, key::command());
        detail::writeRef(wr, key::path_removed());

        detail::writeKey(wr, key::data());
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
