#pragma once
#include <ossia/network/http/http_query_parser.hpp>
#include <ossia/network/oscquery/detail/attributes.hpp>

namespace ossia
{
namespace net
{
struct parameter_data;
}
namespace oscquery
{
/**
 * @brief The query_parser class
 *
 * Parse OSCQuery queries received by a server.
 * The queries are similar to the GET part of an http request.
 * i.e. /a/b?value, etc...
 */
class query_parser
{
public:
  template <typename Mapper>
  static auto parse_http_request(const std::string& request, Mapper&& mapper)
  {
    // Split on "?"
    std::string_view path;
    std::string_view queries;

    auto idx = request.find_first_of('?');
    if(idx != std::string::npos)
    {
      path = std::string_view(request.data(), idx);
      queries = std::string_view(request.data() + idx + 1, request.size() - idx);
    }
    else
    {
      path = request;
    }

    std::string clean_path;
    ossia::net::url_decode(path, clean_path);
    return mapper(clean_path, parse_http_methods(queries));
  }

  static string_map<std::string> parse_http_methods(std::string_view str);

  static bool parse_bool(const std::string& data);

  static void
  parse(net::access_mode_attribute, const std::string& data, net::parameter_data& res);
  static void
  parse(net::bounding_mode_attribute, const std::string& data, net::parameter_data& res);
  static void parse(
      net::repetition_filter_attribute, const std::string& data,
      net::parameter_data& res);
  static void parse(
      net::refresh_rate_attribute attr, const std::string& data,
      net::parameter_data& res);
  static void
  parse(net::priority_attribute attr, const std::string& data, net::parameter_data& res);
  static void parse(
      net::value_step_size_attribute attr, const std::string& data,
      net::parameter_data& res);
  static void
  parse(net::critical_attribute attr, const std::string& data, net::parameter_data& res);
  static void
  parse(net::hidden_attribute attr, const std::string& data, net::parameter_data& res);
  static void
  parse(net::disabled_attribute attr, const std::string& data, net::parameter_data& res);
  static void parse(
      net::description_attribute attr, const std::string& data,
      net::parameter_data& res);
  static void
  parse(net::app_name_attribute attr, const std::string& data, net::parameter_data& res);
  static void parse(
      net::app_version_attribute attr, const std::string& data,
      net::parameter_data& res);
  static void parse(
      net::app_creator_attribute attr, const std::string& data,
      net::parameter_data& res);
  static void
  parse(net::value_attribute attr, const std::string& data, net::parameter_data& res);
  static void
  parse(net::domain_attribute attr, const std::string& data, net::parameter_data& res);
  static void
  parse(net::tags_attribute, const std::string& data, net::parameter_data& res);
  static void parse(
      net::instance_bounds_attribute attr, const std::string& data,
      net::parameter_data& res);
  static void
  parse(net::unit_attribute attr, const std::string& data, net::parameter_data& res);
  static void parse(
      net::extended_type_attribute attr, const std::string& data,
      net::parameter_data& res);
  static void parse(
      net::default_value_attribute attr, const std::string& data,
      net::parameter_data& res);
  static void parse(
      oscquery::detail::typetag_attribute attr, const std::string& data,
      net::parameter_data& res);
};
}
}
