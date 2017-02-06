#pragma once
#define RAPIDJSON_HAS_STDSTRING 1
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
constexpr ossia::string_view path_added() { return "PATH_ADDED"; }
constexpr ossia::string_view path_removed() { return "PATH_REMOVED"; }
constexpr ossia::string_view path_changed() { return "PATH_CHANGED"; }
constexpr ossia::string_view attributes_changed() { return "ATTRIBUTES_CHANGED"; }

constexpr ossia::string_view paths_added() { return "PATHS_ADDED"; }
constexpr ossia::string_view paths_removed() { return "PATHS_REMOVED"; }
constexpr ossia::string_view paths_changed() { return "PATHS_CHANGED"; }
constexpr ossia::string_view attributes_changed_array() { return "ATTRIBUTES_CHANGED_ARRAY"; }
}

namespace detail
{

struct json_parser_impl
{
    struct to_json_value
    {
        allocator& alloc;
        rapidjson::Value operator()(impulse) const { return rapidjson::Value{}; }
        rapidjson::Value operator()(int v) const { return rapidjson::Value{v}; }
        rapidjson::Value operator()(float v) const { return rapidjson::Value{v}; }
        rapidjson::Value operator()(bool v) const { return rapidjson::Value{v}; }
        rapidjson::Value operator()(char v) const { return rapidjson::Value{v}; }
        rapidjson::Value operator()(const std::string& v) const
        {
            // TODO handle base 64
            // bool b = Base64::Encode(get<coppa::Generic>(val).buf, &out);
            return rapidjson::Value{v, alloc};
        }

        template<std::size_t N>
        rapidjson::Value operator()(const std::array<float, N>& vec) const {
            rapidjson::Value v{rapidjson::kArrayType};
            v.Reserve(N, alloc);
            for(std::size_t i = 0; i < N; i++)
            {
                v.PushBack(vec[i], alloc);
            }
            return v;
        }

        rapidjson::Value operator()(const std::vector<ossia::value>& vec) const
        {
            rapidjson::Value v{rapidjson::kArrayType};
            v.Reserve(vec.size(), alloc);

            for(const auto& sub : vec)
            {
                v.PushBack(sub.apply(*this), alloc);
            }

            return v;
        }

        rapidjson::Value operator()(const ossia::Destination& d) const
        {
            throw;
        }


        rapidjson::Value operator()() const
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

    // TODO base64 encode
    allocator& alloc;
    rapidjson::Value toJson(const ossia::value& val) const
    {
        return val.apply(to_json_value{alloc});
    }

    rapidjson::Value toJson(ossia::bounding_mode b) const
    {
        switch(b)
        {
            case ossia::bounding_mode::FREE: return rapidjson::Value{"None", alloc};
            case ossia::bounding_mode::CLIP: return rapidjson::Value{"Both", alloc};
            case ossia::bounding_mode::LOW:  return rapidjson::Value{"Low",  alloc};
            case ossia::bounding_mode::HIGH: return rapidjson::Value{"High", alloc};
            case ossia::bounding_mode::WRAP: return rapidjson::Value{"Wrap", alloc};
            case ossia::bounding_mode::FOLD: return rapidjson::Value{"Fold", alloc};
            default: throw;
        }
    }

    rapidjson::Value toJson(ossia::access_mode b) const
    {
        switch(b)
        {
            case ossia::access_mode::GET: return rapidjson::Value{1};
            case ossia::access_mode::SET: return rapidjson::Value{2};
            case ossia::access_mode::BI: return rapidjson::Value{3};
            default: return rapidjson::Value{0}; // no address case
        }
    }

    rapidjson::Value toJson(const ossia::net::domain& d) const
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
        return {};
    }

    rapidjson::Value tagsToJson(const ossia::net::tags& tags) const
    {
        rapidjson::Value v{rapidjson::kArrayType};
        v.Reserve(tags.size(), alloc);

        for(const auto& tag : tags)
        {
            v.PushBack(val(tag), alloc);
        }

        return v;
    }


    std::string getOSCType(const ossia::value& value) const
    {
        std::string s;
        to_osc_type t{s};
        value.apply(t);
        return s;
    }


    rapidjson::Value attributeToJsonValue(
            const ossia::net::node_base& n,
            ossia::string_view method
            ) const
    {
        if(method == key::attribute_value())
        {
            auto addr = n.getAddress();
            if(addr)
                return toJson(addr->cloneValue());
            else
                return {};
        }
        else if(method == key::attribute_range())
        {
            auto addr = n.getAddress();
            if(addr)
                return toJson(addr->getDomain());
            else
                return {};
        }
        else if(method == key::attribute_clipmode())
        {
            auto addr = n.getAddress();
            if(addr)
                return toJson(addr->getBoundingMode());
            else
                return {};
        }
        else if(method == key::attribute_accessmode())
        {
            auto addr = n.getAddress();
            if(addr)
                return toJson(addr->getAccessMode());
            else
                return {};
        }
        else if(method == key::type())
        {
            auto addr = n.getAddress();
            if(addr)
                // TODO we could have a fast path for the types statically known
                return rapidjson::Value{getOSCType(addr->cloneValue()), alloc};
            else
                return {};
        }
        else if(method == key::attribute_description())
        {
            auto desc = ossia::net::get_description(n);
            if(desc)
                return rapidjson::Value{*desc, alloc};
            return {};
        }
        else if(method == key::attribute_tags())
        {
            auto tags = ossia::net::get_tags(n);
            if(tags)
                return tagsToJson(*tags);
            return {};
        }
        else if(method == key::full_path())
        {
            return rapidjson::Value{ossia::net::address_string_from_node(n), alloc};
        }
        else
        {
            throw ossia::bad_request_error{"Attribute not found"};
        }
    }

    rapidjson::GenericStringRef<char> StringRef(ossia::string_view s) const
    { return rapidjson::GenericStringRef<char>(s.data(), s.size()); }

    template<typename T>
    rapidjson::Value val(T&& t) const { return rapidjson::Value{t, alloc}; }

    void nodeAttributesToJson(
            const ossia::net::node_base& n,
            rapidjson::Value& obj) const
    {
        using namespace std;
        using namespace boost;
        using namespace eggs::variants;

        auto addr = n.getAddress();

        // These attributes are always here
        obj.AddMember(StringRef(key::full_path()), val(ossia::net::address_string_from_node(n)), alloc);

        // Metadata
        auto desc = ossia::net::get_description(n);
        if(desc)
            obj.AddMember(StringRef(key::attribute_description()), val(*desc), alloc);

        auto tags = ossia::net::get_tags(n);
        if(tags)
            obj.AddMember(StringRef(key::attribute_tags()), tagsToJson(*tags), alloc);

        // Handling of the types / values
        if(addr)
        {
            obj.AddMember(StringRef(key::type()), val(getOSCType(addr->cloneValue())), alloc);
            obj.AddMember(StringRef(key::attribute_accessmode()), toJson(addr->getAccessMode()), alloc);
            obj.AddMember(StringRef(key::attribute_clipmode()), toJson(addr->getBoundingMode()), alloc);
            obj.AddMember(StringRef(key::attribute_value()), toJson(addr->cloneValue()), alloc);
            obj.AddMember(StringRef(key::attribute_range()), toJson(addr->getDomain()), alloc);
        }
        else
        {
            obj.AddMember(StringRef(key::attribute_accessmode()), 0, alloc);
        }
    }

    rapidjson::Value nodeToJson(const ossia::net::node_base& n)
    {
        rapidjson::Value v(rapidjson::Type::kObjectType);

        nodeAttributesToJson(n, v);

        auto& cld = n.children();

        if(!cld.empty())
        {
            rapidjson::Value contents(rapidjson::Type::kObjectType);
            for(const auto& child : cld)
            {
                contents.AddMember(val(child->getName()), nodeToJson(*child), alloc);
            }
            v.AddMember(StringRef(key::contents()), contents, alloc);
        }

        return v;
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
    static std::string query_namespace(const ossia::net::node_base& dev)
    {
        using namespace detail;
        rapidjson::Document d;
        json_parser_impl p{d.GetAllocator()};

        d.SetObject();
        d.GetObject() = p.nodeToJson(dev).GetObject();
        rapidjson::StringBuffer buf;
        rapidjson::Writer<rapidjson::StringBuffer> wr(buf);
        d.Accept(wr);
        return std::string(buf.GetString(), buf.GetSize()); // TODO avoid this copy
    }
    /*
    template<typename StringVec_T>
    static std::string query_attributes(
            const Parameter& param,
            const StringVec_T& methods)
    {
        using namespace detail;
        json_map map;
        for(auto& method : methods)
            map[method] = attributeToJsonValue(param, method);
        return map.to_string();
    }


    // Update messages
    template<typename... Args>
    static std::string path_added(
            Args&&... args)
    {
        using namespace detail;
        json_map map;
        map[key::path_added()] = mapToJson(std::forward<Args>(args)...);
        return map.to_string();
    }

    template<typename... Args>
    static std::string path_changed(
            Args&&... args)
    {
        using namespace detail;
        json_map map;
        map[key::path_changed()] = mapToJson(std::forward<Args>(args)...);
        return map.to_string();
    }

    static std::string path_removed(
            const std::string& path)
    {
        using namespace detail;
        json_map map;
        map[key::path_removed()] = path;
        return map.to_string();
    }

public:
    template<typename... Attributes>
    static std::string attributes_changed(
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


    template<typename Map, typename Vector>
    static std::string paths_added(
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
