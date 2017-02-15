#include "minuit_common.hpp"
#include <ossia/detail/string_map.hpp>

namespace ossia
{
namespace minuit
{

ossia::string_view to_minuit_type_text(const ossia::value& val)
{
  // integer, decimal, string, generic, boolean, none, array.
  struct ValueStringVisitor
  {
    ossia::string_view operator()(ossia::impulse) const
    {
      return make_string_view("none");
    }
    ossia::string_view operator()(int32_t i) const
    {
      return make_string_view("integer");
    }
    ossia::string_view operator()(float f) const
    {
      return make_string_view("decimal");
    }
    ossia::string_view operator()(bool b) const
    {
      return make_string_view("boolean");
    }
    ossia::string_view operator()(char c) const
    {
      return make_string_view("string");
    }
    ossia::string_view operator()(const std::string& str) const
    {
      return make_string_view("string");
    }
    ossia::string_view operator()(const ossia::vec2f& vec) const
    {
      return make_string_view("array");
    }
    ossia::string_view operator()(const ossia::vec3f& vec) const
    {
      return make_string_view("array");
    }
    ossia::string_view operator()(const ossia::vec4f& vec) const
    {
      return make_string_view("array");
    }
    ossia::string_view operator()(const std::vector<ossia::value>& t) const
    {
      return make_string_view("array");
    }
    ossia::string_view operator()(const ossia::Destination& d) const
    {
      throw invalid_value_type_error("to_minuit_type_text: "
                                     "Trying to send Destination value");
      return {};
    }
    ossia::string_view operator()() const
    {
      throw invalid_value_type_error("to_minuit_type_text: "
                                     "Trying to send null value");
      return {};
    }
  };

  return val.apply(ValueStringVisitor{});
}

static const auto& attribute_unordered_map() {
  static const string_view_map<minuit_attribute> attr{
        {make_string_view("value"), minuit_attribute::Value},
        {make_string_view("type"), minuit_attribute::Type},
        {make_string_view("service"), minuit_attribute::Service},
        {make_string_view("priority"), minuit_attribute::Priority},
        {make_string_view("rangeBounds"), minuit_attribute::RangeBounds},
        {make_string_view("rangeClipmode"), minuit_attribute::RangeClipMode},
        {make_string_view("description"), minuit_attribute::Description},
        {make_string_view("repetitionsFilter"), minuit_attribute::RepetitionFilter},
        {make_string_view("tags"), minuit_attribute::Tags},
        {make_string_view("active"), minuit_attribute::Active},
        {make_string_view("valueDefault"), minuit_attribute::ValueDefault},
        {make_string_view("priority"), minuit_attribute::Priority},
        {make_string_view("dataspace"), minuit_attribute::Dataspace},
        {make_string_view("dataspaceUnit"), minuit_attribute::DataspaceUnit},
        {make_string_view("rampFunction"), minuit_attribute::RampFunction},
        {make_string_view("rampDrive"), minuit_attribute::RampDrive},
        {make_string_view("valueStepsize"), minuit_attribute::ValueStepSize},
        {make_string_view("rampFunctionParameters"), minuit_attribute::RampFunctionParameters}
    };
  return attr;
}

minuit_attribute get_attribute(ossia::string_view str)
{
  const auto& map = attribute_unordered_map();
  auto it = map.find(str);
  if (it != map.end())
    return it->second;
  else
    throw parse_error("get_attribute: unhandled attribute");
  return {};
}

ossia::string_view to_minuit_attribute_text(minuit_attribute str)
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
}
}
