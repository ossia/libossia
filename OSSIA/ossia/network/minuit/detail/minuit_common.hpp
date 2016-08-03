#pragma once
#include <ossia/editor/value/value.hpp>
#include <ossia/network/exceptions.hpp>
#include <boost/utility/string_ref.hpp>
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

inline boost::string_ref to_minuit_type_text(const ossia::value& val)
{
  // integer, decimal, string, generic, boolean, none, array.
  struct ValueStringVisitor
  {
    boost::string_ref operator()(ossia::Impulse) const
    {
      return "none";
    }
    boost::string_ref operator()(ossia::Int i) const
    {
      return "integer";
    }
    boost::string_ref operator()(ossia::Float f) const
    {
      return "decimal";
    }
    boost::string_ref operator()(ossia::Bool b) const
    {
      return "boolean";
    }
    boost::string_ref operator()(ossia::Char c) const
    {
      return "string";
    }
    boost::string_ref operator()(const ossia::String& str) const
    {
      return "string";
    }
    boost::string_ref operator()(ossia::Vec2f vec) const
    {
      return "array";
    }
    boost::string_ref operator()(ossia::Vec3f vec) const
    {
      return "array";
    }
    boost::string_ref operator()(ossia::Vec4f vec) const
    {
      return "array";
    }
    boost::string_ref operator()(const ossia::Tuple& t) const
    {
      return "array";
    }
    boost::string_ref operator()(const ossia::Destination& d) const
    {
      throw invalid_value_type_error("to_minuit_type_text: "
                                     "Trying to send Destination value");
      return {};
    }
    boost::string_ref operator()(const ossia::Behavior&) const
    {
      throw invalid_value_type_error("to_minuit_type_text: "
                                     "Trying to send Behavior value");
      return {};
    }
    boost::string_ref operator()() const
    {
      throw invalid_value_type_error("to_minuit_type_text: "
                                     "Trying to send null value");
      return {};
    }
  };

  return val.apply(ValueStringVisitor{});
}

inline boost::string_ref to_minuit_type_text(ossia::val_type val)
{
  // integer, decimal, string, generic, boolean, none, array.
  switch(val)
  {
    case val_type::IMPULSE:
      return "none";
    case val_type::INT:
      return "integer";
    case val_type::FLOAT:
      return "decimal";
    case val_type::BOOL:
      return "boolean";
    case val_type::CHAR:
    case val_type::STRING:
      return "string";
    case val_type::VEC2F:
    case val_type::VEC3F:
    case val_type::VEC4F:
    case val_type::TUPLE:
      return "array";
    default:
      throw invalid_value_type_error("to_minuit_type_text: Invalid type");
  }
  return {};
}

inline ossia::value value_from_minuit_type_text(boost::string_ref str)
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
    case 'g': // generic
      return ossia::String{};
    case 'b': // boolean
      return ossia::Bool{};
    case 'n': // none
      return ossia::Impulse{};
    case 'a': // array
      return ossia::Tuple{};
    default:
      return {};
  }
}

inline ossia::val_type type_from_minuit_type_text(boost::string_ref str)
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
    case 'g': // generic
      return ossia::val_type::STRING;
    case 'b': // boolean
      return ossia::val_type::BOOL;
    case 'n': // none
      return ossia::val_type::IMPULSE;
    case 'a': // array
      return ossia::val_type::TUPLE;
    default:
      return {};
  }
}

inline boost::string_ref to_minuit_service_text(ossia::access_mode acc)
{
  switch (acc)
  {
    case ossia::access_mode::BI:
      return "parameter";
    case ossia::access_mode::GET:
      return "return";
    case ossia::access_mode::SET:
      return "message";
    default:
      throw parse_error("to_minuit_service_text: Invalid access mode");
  }
  return {};
}

inline ossia::access_mode from_minuit_service_text(boost::string_ref str)
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

inline boost::string_ref to_minuit_bounding_text(ossia::bounding_mode b)
{
  switch (b)
  {
    case ossia::bounding_mode::FREE:
      return "none";
    case ossia::bounding_mode::CLIP:
      return "both";
    case ossia::bounding_mode::WRAP:
      return "wrap";
    case ossia::bounding_mode::FOLD:
      return "fold";
    default:
      throw parse_error("to_minuit_bounding_text: Invalid bounding mode");
  }
  return {};
}

inline ossia::bounding_mode from_minuit_bounding_text(boost::string_ref str)
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
    default:
      throw parse_error("from_minuit_bounding_text: Invalid bounding mode");
  }
  return {};
}

inline boost::string_ref to_minuit_attribute_text(minuit_attribute str)
{
  switch (str)
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

inline minuit_attribute get_attribute(boost::string_ref str)
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
      throw parse_error("get_type: unhandled type");
  }
  return {};
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

inline minuit_operation get_operation(boost::string_ref str)
{
  return get_operation(str[0]);
}
}
}
