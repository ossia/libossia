#pragma once
#include <boost/utility/string_ref.hpp>
#include <ossia/editor/value/value.hpp>
#include <exception>

namespace OSSIA
{
namespace minuit
{
enum class minuit_command : char
{ Request = '?', Answer = ':', Error = '!' };

enum class minuit_operation : char
{ Listen = 'l', Namespace = 'n', Get = 'g'};

enum class minuit_action : int {
    NamespaceRequest,
    NamespaceReply,
    NamespaceError,
    GetRequest,
    GetReply,
    GetError,
    ListenRequest,
    ListenReply,
    ListenError
};

enum class minuit_type : char
{ Application = 'A', Container = 'C', Data = 'D', None = 'n', ModelInfo = 'M', UiInfo = 'U', PresetManager = 'P' };

enum class minuit_attribute
{ Value, Type, Service,
  RangeBounds, RangeClipMode, Description, RepetitionFilter,
  Tags, Active, ValueDefault, Priority, Dataspace, DataspaceUnit,
  RampFunction, RampDrive, ValueStepSize, RampFunctionParameters };


inline boost::string_ref to_minuit_type_text(const OSSIA::Value& val)
{
    // integer, decimal, string, generic, boolean, none, array.
    struct ValueStringVisitor {
      boost::string_ref operator()(OSSIA::Impulse) const { return "none"; }
      boost::string_ref operator()(OSSIA::Int i) const   { return "integer"; }
      boost::string_ref operator()(OSSIA::Float f) const { return "decimal"; }
      boost::string_ref operator()(OSSIA::Bool b) const  { return "boolean"; }
      boost::string_ref operator()(OSSIA::Char c) const  { return "string"; }
      boost::string_ref operator()(const OSSIA::String& str) const { return "string"; }
      boost::string_ref operator()(OSSIA::Vec2f vec) const { return "array"; }
      boost::string_ref operator()(OSSIA::Vec3f vec) const { return "array"; }
      boost::string_ref operator()(OSSIA::Vec4f vec) const { return "array"; }
      boost::string_ref operator()(const OSSIA::Destination& d) const { throw std::runtime_error("Invalid type"); }
      boost::string_ref operator()(const OSSIA::Behavior&) const { throw std::runtime_error("Invalid type"); }
      boost::string_ref operator()(const OSSIA::Tuple& t) const { return "array"; }
    };

    if(val.v)
    {
        return eggs::variants::apply(ValueStringVisitor{}, val.v);
    }
    else
    {
        throw std::runtime_error("Invalid value");
    }
}

inline OSSIA::Value value_from_minuit_type_text(boost::string_ref str)
{
    // integer, decimal, string, generic, boolean, none, array.
    // we can differentiate them by the first character

    switch(str[0])
    {
        case 'i': // integer
            return OSSIA::Int{};
        case 'd': // decimal
            return OSSIA::Float{};
        case 's': // string
            return OSSIA::String{};
        case 'b': // boolean
            return OSSIA::Bool{};
        case 'n': // none
            return OSSIA::Impulse{};
        case 'a': // array
            return OSSIA::Tuple{};
        case 'g': // generic
        default:
            return {};
    }
}

inline OSSIA::Type type_from_minuit_type_text(boost::string_ref str)
{
  // integer, decimal, string, generic, boolean, none, array.
  // we can differentiate them by the first character

  switch(str[0])
  {
    case 'i': // integer
      return OSSIA::Type::INT;
    case 'd': // decimal
      return OSSIA::Type::FLOAT;
    case 's': // string
      return OSSIA::Type::STRING;
    case 'b': // boolean
      return OSSIA::Type::BOOL;
    case 'n': // none
      return OSSIA::Type::IMPULSE;
    case 'a': // array
      return OSSIA::Type::TUPLE;
    case 'g': // generic
    default:
      return {};
  }
}


inline boost::string_ref to_minuit_service_text(OSSIA::AccessMode acc)
{
    switch(acc)
    {
        case OSSIA::AccessMode::BI:
            return "parameter";
        case OSSIA::AccessMode::GET:
            return "return";
        case OSSIA::AccessMode::SET:
            return "message";
        default:
            throw std::runtime_error("Invalid access mode");
    }
}

inline OSSIA::AccessMode from_minuit_service_text(boost::string_ref str)
{
    switch(str[0])
    {
        case 'p':
            return OSSIA::AccessMode::BI;
        case 'r':
            return OSSIA::AccessMode::GET;
        case 'm':
            return OSSIA::AccessMode::SET;
        default:
            throw std::runtime_error("Invalid access mode");
    }
}

inline boost::string_ref to_minuit_bounding_text(OSSIA::BoundingMode b)
{
    switch(b)
    {
        case OSSIA::BoundingMode::FREE:
            return "none";
        case OSSIA::BoundingMode::CLIP:
            return "both";
        case OSSIA::BoundingMode::WRAP:
            return "wrap";
        case OSSIA::BoundingMode::FOLD:
            return "fold";
        default:
            throw std::runtime_error("Invalid bounding mode");
    }
}


inline OSSIA::BoundingMode from_minuit_bounding_text(boost::string_ref str)
{
    switch(str[0])
    {
        case 'n': // none
            return OSSIA::BoundingMode::FREE;
        case 'b': // both
            return OSSIA::BoundingMode::CLIP;
        case 'w': // wrap
            return OSSIA::BoundingMode::WRAP;
        case 'f': // fold
            return OSSIA::BoundingMode::FOLD;
        default:
            throw std::runtime_error("Invalid bounding mode");
    }
}


inline boost::string_ref to_minuit_attribute_text(minuit_attribute str)
{
    switch(str)
    {
        case minuit_attribute::Value:
            return "value";
        case minuit_attribute::Service:
            return "service";
        case minuit_attribute::Type:
            return "type";
        case minuit_attribute::RangeBounds:
            return "rangeBounds";
        case minuit_attribute::RangeClipMode:
            return "rangeClipmode";
        case minuit_attribute::Description:
            return "description";
        case minuit_attribute::RepetitionFilter:
            return "repetitionsFilter";
        case minuit_attribute::Tags:
            return "tags";
        case minuit_attribute::Active:
            return "active";
        case minuit_attribute::ValueDefault:
            return "valueDefault";
        case minuit_attribute::Priority:
            return "priority";
        case minuit_attribute::Dataspace:
            return "dataspace";
        case minuit_attribute::DataspaceUnit:
            return "dataspaceUnit";
        case minuit_attribute::RampFunction:
            return "rampFunction";
        case minuit_attribute::RampDrive:
            return "rampDrive";
        case minuit_attribute::ValueStepSize:
            return "valueStepsize";
        case minuit_attribute::RampFunctionParameters:
            return "rampFunctionParameters";
        default:
            throw std::runtime_error("unhandled attribute");
    }
}

inline minuit_attribute get_attribute(boost::string_ref str)
{
    const std::map<std::string, minuit_attribute, std::less<>> map
    {
        { "value", minuit_attribute::Value },
        { "type", minuit_attribute::Type },
        { "service", minuit_attribute::Service },
        { "priority", minuit_attribute::Priority },
        { "rangeBounds", minuit_attribute::RangeBounds },
        { "rangeClipmode", minuit_attribute::RangeClipMode },
        { "description", minuit_attribute::Description },
        { "repetitionsFilter", minuit_attribute::RepetitionFilter },
        { "tags", minuit_attribute::Tags },
        { "active", minuit_attribute::Active },
        { "valueDefault", minuit_attribute::ValueDefault },
        { "priority", minuit_attribute::Priority },
        { "dataspace", minuit_attribute::Dataspace },
        { "dataspaceUnit", minuit_attribute::DataspaceUnit },
        { "rampFunction", minuit_attribute::RampFunction },
        { "rampDrive", minuit_attribute::RampDrive },
        { "valueStepsize", minuit_attribute::ValueStepSize },
        { "rampFunctionParameters", minuit_attribute::RampFunctionParameters },
    };

    auto it = map.find(str);
    if(it != map.end())
        return it->second;
    else
        throw std::runtime_error("unhandled attribute");
}

inline minuit_command get_command(char str)
{
    switch(str)
    {
        case '?':
        case ':':
        case '!':
            return static_cast<minuit_command>(str);
        default :
            throw std::runtime_error("unhandled command");
    }
}

inline minuit_type get_type(char str)
{
    switch(str)
    {
        case 'A':
        case 'C':
        case 'D':
        case 'n':
        case 'M':
        case 'U':
        case 'P':
            return static_cast<minuit_type>(str);
        default :
            throw std::runtime_error("unhandled type");
    }
}

inline minuit_operation get_operation(char str)
{
    switch(str)
    {
        case 'l':
        case 'n':
        case 'g':
            return static_cast<minuit_operation>(str);
        default :
            throw std::runtime_error("unhandled operation");
    }
}

inline minuit_operation get_operation(boost::string_ref str)
{
    return get_operation(str[0]);
}

}
}
