#pragma once
#include <ossia/detail/json.hpp>
#include <ossia/network/common/address_properties.hpp>
#include <ossia/editor/value/value.hpp>
#include <ossia/network/domain/domain.hpp>
#include <ossia/network/base/node.hpp>
#include <ossia/network/base/address.hpp>
#include <ossia/network/base/node_attributes.hpp>
#include <ossia/detail/string_view.hpp>
#include <oscpack/osc/OscTypes.h>
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
constexpr ossia::string_view full_path() { return "FULL_PATH"; }

constexpr ossia::string_view type() { return "TYPE"; }
constexpr ossia::string_view contents() { return "CONTENTS"; }

constexpr ossia::string_view attribute_value() { return "VALUE"; }
constexpr ossia::string_view attribute_range() { return "RANGE"; }
constexpr ossia::string_view attribute_clipmode() { return "CLIPMODE"; }
constexpr ossia::string_view attribute_accessmode() { return "ACCESS"; }
constexpr ossia::string_view attribute_description() { return "DESCRIPTION"; }
constexpr ossia::string_view attribute_tags() { return "TAGS"; }

// Not part of the OSCQuery spec :
constexpr ossia::string_view attribute_unit() { return "UNIT"; }
constexpr ossia::string_view attribute_refresh_rate() { return "REFRESH_RATE"; }
constexpr ossia::string_view attribute_priority() { return "PRIORITY"; }
constexpr ossia::string_view attribute_step_size() { return "STEP_SIZE"; }
constexpr ossia::string_view attribute_instance_bounds() { return "INSTANCE_BOUNDS"; }
constexpr ossia::string_view attribute_critical() { return "CRITICAL"; }
constexpr ossia::string_view attribute_extended_type() { return "EXTENDED_TYPE"; }


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

inline void writeKey(rapidjson::Writer<rapidjson::StringBuffer>& writer, ossia::string_view k)
{
    writer.Key(k.data(), k.size());
}

inline void writeRef(rapidjson::Writer<rapidjson::StringBuffer>& writer, ossia::string_view k)
{
    writer.String(k.data(), k.size());
}
struct json_writer_impl
{
    rapidjson::Writer<rapidjson::StringBuffer>& writer;

    // TODO base64 encode
    struct to_json_value
    {
        rapidjson::Writer<rapidjson::StringBuffer>& writer;
        void operator()(impulse) const { writer.Null(); }
        void operator()(int v) const { writer.Int(v); }
        void operator()(float v) const { writer.Double(v); }
        void operator()(bool v) const { writer.Bool(v); }
        void operator()(char v) const { writer.String(std::string(v, 1)); }
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

    struct to_osc_type
    {
        std::string& type;
        void operator()() { type += oscpack::TypeTagValues::NIL_TYPE_TAG; }
        void operator()(impulse) { type += oscpack::TypeTagValues::INFINITUM_TYPE_TAG; }
        void operator()(int v)   { type += oscpack::TypeTagValues::INT32_TYPE_TAG; }
        void operator()(float v) { type += oscpack::TypeTagValues::FLOAT_TYPE_TAG; }
        void operator()(bool v)  { type += v ? oscpack::TypeTagValues::TRUE_TYPE_TAG : oscpack::TypeTagValues::FALSE_TYPE_TAG;}
        void operator()(char v)  { type += oscpack::TypeTagValues::CHAR_TYPE_TAG; }
        void operator()(const std::string& v) {
            // TODO BLOB
            type += oscpack::TypeTagValues::STRING_TYPE_TAG;
        }

        template<std::size_t N>
        void operator()(const std::array<float, N>& vec) {
            for(std::size_t i = 0; i < N; i++)
            {
                type += oscpack::TypeTagValues::FLOAT_TYPE_TAG;
            }
        }

        void operator()(const std::vector<ossia::value>& vec)
        {
            type.reserve(type.size() + vec.size());

            for(const auto& sub : vec)
            {
                sub.apply(*this);
            }
        }

        void operator()(const ossia::Destination& d)
        {
            throw;
        }
    };

    void toJson(const ossia::value& val) const
    {
        val.apply(to_json_value{writer});
    }

    void toJson(ossia::bounding_mode b) const
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

    void toJson(ossia::access_mode b) const
    {
        switch(b)
        {
            case ossia::access_mode::GET: writer.Int(1); break;
            case ossia::access_mode::SET: writer.Int(2); break;
            case ossia::access_mode::BI: writer.Int(3);  break;
            default: writer.Int(0); // no address case
        }
    }

    void toJson(const ossia::net::domain& d) const
    {
        /*
        json_array range_arr;
        for(const auto& range : ranges.ranges)
        {
            json_array range_subarray;
            if(!range.min)
            { range_subarray.push_back(json_value::null_t{}); }
            else
            { addValueToJsonArray(range_subarray, range.min); }

            if(!range.max)
            { range_subarray.push_back(json_value::null_t{}); }
            else
            { addValueToJsonArray(range_subarray, range.max); }

            if(range.range_values.empty())
            { range_subarray.push_back(json_value::null_t{}); }
            else
            {
                json_array range_values_array;
                for(auto& elt : range.range_values)
                {
                    addValueToJsonArray(range_values_array, elt);
                }
                range_subarray.push_back(range_values_array);
            }

            range_arr.push_back(range_subarray);
        }

        return range_arr;
        */
    }

    void tagsToJson(const ossia::net::tags& tags) const
    {
        writer.StartArray();

        for(const auto& tag : tags)
        {
            writer.String(tag);
        }

        writer.EndArray();
    }


    std::string getOSCType(const ossia::value& value) const
    {
        std::string s;
        to_osc_type t{s};
        value.apply(t);
        return s;
    }


    void attributeToJsonValue(
            const ossia::net::node_base& n,
            ossia::string_view method
            ) const
    {
        if(method == key::attribute_value())
        {
            auto addr = n.getAddress();
            if(addr)
                toJson(addr->cloneValue());
            else
                writer.Null();
        }
        else if(method == key::attribute_range())
        {
            auto addr = n.getAddress();
            if(addr)
                toJson(addr->getDomain());
            else
                writer.Null();
        }
        else if(method == key::attribute_clipmode())
        {
            auto addr = n.getAddress();
            if(addr)
                toJson(addr->getBoundingMode());
            else
                writer.Null();
        }
        else if(method == key::attribute_accessmode())
        {
            auto addr = n.getAddress();
            if(addr)
                toJson(addr->getAccessMode());
            else
                writer.Null();
        }
        else if(method == key::type())
        {
            auto addr = n.getAddress();
            if(addr)
                // TODO we could have a fast path for the types statically known
                writer.String(getOSCType(addr->cloneValue()));
            else
                writer.Null();
        }
        else if(method == key::attribute_description())
        {
            auto desc = ossia::net::get_description(n);
            if(desc)
                writer.String(*desc);
            else
                writer.Null();
        }
        else if(method == key::attribute_tags())
        {
            auto tags = ossia::net::get_tags(n);
            if(tags)
                tagsToJson(*tags);
            else
                writer.Null();
        }
        else if(method == key::full_path())
        {
            writer.String(ossia::net::address_string_from_node(n));
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
        writeKey(key::full_path());

        writer.String(ossia::net::address_string_from_node(n));

        // Metadata
        auto desc = ossia::net::get_description(n);
        if(desc)
        {
            writeKey(key::attribute_description());
            writer.String(*desc);
        }
        auto tags = ossia::net::get_tags(n);
        if(tags)
        {
            writeKey(key::attribute_tags());
            tagsToJson(*tags);
        }

        // Handling of the types / values
        if(addr)
        {
            auto val = addr->cloneValue();
            writeKey(key::type());
            writer.String(getOSCType(val));

            writeKey(key::attribute_accessmode());
            toJson(addr->getAccessMode());

            writeKey(key::attribute_clipmode());
            toJson(addr->getBoundingMode());

            writeKey(key::attribute_value());
            toJson(val);

            writeKey(key::attribute_range());
            toJson(addr->getDomain());
        }
        else
        {
            writeKey(key::attribute_accessmode());
            writer.Int(0);
        }
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

public:
    // Initialisation
    static std::string device_info(int port)
    {
        json_map map;
        map[key::osc_port()] = port;

        return map.to_string();
    }
    */

    // Format interface
    // Queries
public:
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
