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
{ Application = 'A', Container = 'C', Data = 'D', None = 'n' };

enum class minuit_attribute
{ Value, Type, Service, Priority, RangeBounds, RangeClipMode, Description, RepetitionFilter };


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
            return "free";
        case OSSIA::BoundingMode::CLIP:
            return "clip";
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
    switch(str[2]) // only unique character
    {
        case 'e': // frEe
            return OSSIA::BoundingMode::FREE;
        case 'i': // clIp
            return OSSIA::BoundingMode::CLIP;
        case 'a': // wrAp
            return OSSIA::BoundingMode::WRAP;
        case 'l': // foLd
            return OSSIA::BoundingMode::FOLD;
        default:
            throw std::runtime_error("Invalid bounding mode");
    }
}


inline boost::string_ref to_minuit_attribute_text(minuit_attribute str)
{
    switch(str) // only unique character
    {
        case minuit_attribute::Value:
            return "value";
        case minuit_attribute::Service:
            return "service";
        case minuit_attribute::Type:
            return "type";
        case minuit_attribute::Priority:
            return "priority";
        case minuit_attribute::RangeBounds:
            return "rangeBounds";
        case minuit_attribute::RangeClipMode:
            return "clipMode";
        case minuit_attribute::Description:
            return "description";
        case minuit_attribute::RepetitionFilter:
            return "repetitionFilter";
        default:
            throw std::runtime_error("unhandled attribute");
    }
}

inline minuit_attribute get_attribute(boost::string_ref str)
{
    // requires str.size() > 0
    switch(str[0])
    {
        case 'v': // value
            return minuit_attribute::Value;
        case 't': // type
            return minuit_attribute::Type;
        case 's': // service
            return minuit_attribute::Service;
        case 'p': // priority
            return minuit_attribute::Priority;
        case 'r':
        {
            if(str.size() >= 6)
            {
                switch(str[5])
                {
                    case 'B': // rangeBounds
                        return minuit_attribute::RangeBounds;
                    case 'C': // rangeClipMode
                        return minuit_attribute::RangeClipMode;
                    case 'i': // repetitionsFilter
                        return minuit_attribute::RepetitionFilter;
                }
            }
            // if not returning, throw
        }
        default:
            throw std::runtime_error("unhandled attribute");
    }
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
