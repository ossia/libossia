#pragma once
#include <ossia/editor/value/value.hpp>
#include <ossia/network/exceptions.hpp>
#include <ossia/detail/algorithms.hpp>
#include <boost/utility/string_view.hpp>
#include <exception>
#include <unordered_map>

namespace ossia
{
namespace minuit
{
enum class minuit_command : char
{
  Request = '?',
  Answer = ':',
  Error = '!'
};

enum class minuit_operation : char
{
  Listen = 'l',
  Namespace = 'n',
  Get = 'g'
};

enum class minuit_action : int
{
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
{
  Application = 'A',
  Container = 'C',
  Data = 'D',
  None = 'n',
  ModelInfo = 'M',
  UiInfo = 'U',
  PresetManager = 'P'
};

enum class minuit_attribute
{
  Value,
  Type,
  Service,
  RangeBounds,
  RangeClipMode,
  Description,
  RepetitionFilter,
  Tags,
  Active,
  ValueDefault,
  Priority,
  Dataspace,
  DataspaceUnit,
  RampFunction,
  RampDrive,
  ValueStepSize,
  RampFunctionParameters
};

inline boost::string_view to_minuit_type_text(const ossia::value& val)
{
  // integer, decimal, string, generic, boolean, none, array.
  struct ValueStringVisitor
  {
    boost::string_view operator()(ossia::Impulse) const
    {
      return make_string_view("none");
    }
    boost::string_view operator()(ossia::Int i) const
    {
      return make_string_view("integer");
    }
    boost::string_view operator()(ossia::Float f) const
    {
      return make_string_view("decimal");
    }
    boost::string_view operator()(ossia::Bool b) const
    {
      return make_string_view("boolean");
    }
    boost::string_view operator()(ossia::Char c) const
    {
      return make_string_view("string");
    }
    boost::string_view operator()(const ossia::String& str) const
    {
      return make_string_view("string");
    }
    boost::string_view operator()(const ossia::Vec2f& vec) const
    {
      return make_string_view("array");
    }
    boost::string_view operator()(const ossia::Vec3f& vec) const
    {
      return make_string_view("array");
    }
    boost::string_view operator()(const ossia::Vec4f& vec) const
    {
      return make_string_view("array");
    }
    boost::string_view operator()(const ossia::Tuple& t) const
    {
      return make_string_view("array");
    }
    boost::string_view operator()(const ossia::Destination& d) const
    {
      throw invalid_value_type_error("to_minuit_type_text: "
                                     "Trying to send Destination value");
      return {};
    }
    boost::string_view operator()() const
    {
      throw invalid_value_type_error("to_minuit_type_text: "
                                     "Trying to send null value");
      return {};
    }
  };

  return val.apply(ValueStringVisitor{});
}

inline boost::string_view to_minuit_type_text(ossia::val_type val)
{
  // integer, decimal, string, generic, boolean, none, array.
  switch(val)
  {
    case val_type::IMPULSE:
      return make_string_view("none");
    case val_type::INT:
      return make_string_view("integer");
    case val_type::FLOAT:
      return make_string_view("decimal");
    case val_type::BOOL:
      return make_string_view("boolean");
    case val_type::CHAR:
    case val_type::STRING:
      return make_string_view("string");
    case val_type::VEC2F:
    case val_type::VEC3F:
    case val_type::VEC4F:
    case val_type::TUPLE:
      return make_string_view("array");
    default:
      throw invalid_value_type_error("to_minuit_type_text: Invalid type");
  }
  return {};
}

inline ossia::value value_from_minuit_type_text(boost::string_view str)
{
  // integer, decimal, string, generic, boolean, none, array.
  // we can differentiate them by the first character

  switch (str[0])
  {
    case 'i': // integer
      return ossia::Int{};
    case 'd': // decimal
      return ossia::Float{};
    case 's': // string
      return ossia::String{};
    case 'b': // boolean
      return ossia::Bool{};
    case 'n': // none
      return ossia::Impulse{};
    case 'a': // array
    case 'g': // generic
      return ossia::Tuple{};
    default:
      return {};
  }
}

inline ossia::val_type type_from_minuit_type_text(boost::string_view str)
{
  // integer, decimal, string, generic, boolean, none, array.
  // we can differentiate them by the first character

  switch (str[0])
  {
    case 'i': // integer
      return ossia::val_type::INT;
    case 'd': // decimal
      return ossia::val_type::FLOAT;
    case 's': // string
      return ossia::val_type::STRING;
    case 'b': // boolean
      return ossia::val_type::BOOL;
    case 'n': // none
      return ossia::val_type::IMPULSE;
    case 'a': // array
    case 'g': // generic
      return ossia::val_type::TUPLE;
    default:
      return {};
  }
}

inline boost::string_view to_minuit_service_text(ossia::access_mode acc)
{
  switch (acc)
  {
    case ossia::access_mode::BI:
      return make_string_view("parameter");
    case ossia::access_mode::GET:
      return make_string_view("return");
    case ossia::access_mode::SET:
      return make_string_view("message");
    default:
      throw parse_error("to_minuit_service_text: Invalid access mode");
  }
  return {};
}

inline ossia::access_mode from_minuit_service_text(boost::string_view str)
{
  switch (str[0])
  {
    case 'p':
      return ossia::access_mode::BI;
    case 'r':
      return ossia::access_mode::GET;
    case 'm':
      return ossia::access_mode::SET;
    default:
      throw parse_error("from_minuit_service_text: Invalid access mode");
  }
  return {};
}

inline boost::string_view to_minuit_bounding_text(ossia::bounding_mode b)
{
  switch (b)
  {
    case ossia::bounding_mode::FREE:
      return make_string_view("none");
    case ossia::bounding_mode::CLIP:
      return make_string_view("both");
    case ossia::bounding_mode::WRAP:
      return make_string_view("wrap");
    case ossia::bounding_mode::FOLD:
      return make_string_view("fold");
    case ossia::bounding_mode::LOW:
      return make_string_view("low");
    case ossia::bounding_mode::HIGH:
      return make_string_view("high");
    default:
      throw parse_error("to_minuit_bounding_text: Invalid bounding mode");
  }
  return {};
}

inline ossia::bounding_mode from_minuit_bounding_text(boost::string_view str)
{
  switch (str[0])
  {
    case 'n': // none
      return ossia::bounding_mode::FREE;
    case 'b': // both
      return ossia::bounding_mode::CLIP;
    case 'w': // wrap
      return ossia::bounding_mode::WRAP;
    case 'f': // fold
      return ossia::bounding_mode::FOLD;
    case 'l': // low
      return ossia::bounding_mode::LOW;
    case 'h': // high
      return ossia::bounding_mode::HIGH;
    default:
      throw parse_error("from_minuit_bounding_text: Invalid bounding mode");
  }
  return {};
}

inline boost::string_view to_minuit_attribute_text(minuit_attribute str)
{
  switch (str)
  {
    case minuit_attribute::Value:
      return make_string_view("value");
    case minuit_attribute::Service:
      return make_string_view("service");
    case minuit_attribute::Type:
      return make_string_view("type");
    case minuit_attribute::RangeBounds:
      return make_string_view("rangeBounds");
    case minuit_attribute::RangeClipMode:
      return make_string_view("rangeClipmode");
    case minuit_attribute::Description:
      return make_string_view("description");
    case minuit_attribute::RepetitionFilter:
      return make_string_view("repetitionsFilter");
    case minuit_attribute::Tags:
      return make_string_view("tags");
    case minuit_attribute::Active:
      return make_string_view("active");
    case minuit_attribute::ValueDefault:
      return make_string_view("valueDefault");
    case minuit_attribute::Priority:
      return make_string_view("priority");
    case minuit_attribute::Dataspace:
      return make_string_view("dataspace");
    case minuit_attribute::DataspaceUnit:
      return make_string_view("dataspaceUnit");
    case minuit_attribute::RampFunction:
      return make_string_view("rampFunction");
    case minuit_attribute::RampDrive:
      return make_string_view("rampDrive");
    case minuit_attribute::ValueStepSize:
      return make_string_view("valueStepsize");
    case minuit_attribute::RampFunctionParameters:
      return make_string_view("rampFunctionParameters");
    default:
      throw parse_error("to_minuit_attribute_text: unhandled attribute");
  }
  return {};
}

const std::unordered_map<std::string, minuit_attribute>
    attribute_unordered_map{
        {"value", minuit_attribute::Value},
        {"type", minuit_attribute::Type},
        {"service", minuit_attribute::Service},
        {"priority", minuit_attribute::Priority},
        {"rangeBounds", minuit_attribute::RangeBounds},
        {"rangeClipmode", minuit_attribute::RangeClipMode},
        {"description", minuit_attribute::Description},
        {"repetitionsFilter", minuit_attribute::RepetitionFilter},
        {"tags", minuit_attribute::Tags},
        {"active", minuit_attribute::Active},
        {"valueDefault", minuit_attribute::ValueDefault},
        {"priority", minuit_attribute::Priority},
        {"dataspace", minuit_attribute::Dataspace},
        {"dataspaceUnit", minuit_attribute::DataspaceUnit},
        {"rampFunction", minuit_attribute::RampFunction},
        {"rampDrive", minuit_attribute::RampDrive},
        {"valueStepsize", minuit_attribute::ValueStepSize},
        {"rampFunctionParameters", minuit_attribute::RampFunctionParameters},
    };

inline minuit_attribute get_attribute(boost::string_view str)
{
  auto it = attribute_unordered_map.find(str.to_string());
  if (it != attribute_unordered_map.end())
    return it->second;
  else
    throw parse_error("get_attribute: unhandled attribute");
  return {};
}

inline minuit_command get_command(char str)
{
  switch (str)
  {
    case '?':
    case ':':
    case '!':
      return static_cast<minuit_command>(str);
    default:
      throw parse_error("get_command: unhandled command");
  }
  return {};
}

inline minuit_type get_type(char str)
{
  switch (str)
  {
    case 'A':
    case 'C':
    case 'D':
    case 'n':
    case 'M':
    case 'U':
    case 'P':
      return static_cast<minuit_type>(str);
    default:
      return minuit_type::None;
  }
  return minuit_type::None;
}

inline minuit_operation get_operation(char str)
{
  switch (str)
  {
    case 'l':
    case 'n':
    case 'g':
      return static_cast<minuit_operation>(str);
    default:
      throw parse_error("get_operation: unhandled operation");
  }
  return {};
}

inline minuit_operation get_operation(boost::string_view str)
{
  return get_operation(str[0]);
}
}
}
