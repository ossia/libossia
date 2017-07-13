// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "query_parser.hpp"
#include <ossia/network/base/address_data.hpp>
#include <ossia/network/base/node.hpp>
#include <ossia/network/exceptions.hpp>
#include <ossia/network/oscquery/detail/http_query_parser.hpp>
#include <ossia/network/oscquery/detail/json_parser.hpp>
#include <ossia/network/oscquery/detail/json_writer.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/container/static_vector.hpp>
#include <boost/lexical_cast.hpp>
#include <chobo/small_vector.hpp>
#include <hopscotch_map.h>
#include <string>
#include <vector>
#include <websocketpp/connection.hpp>

namespace ossia
{
namespace oscquery
{

string_map<std::string>
query_parser::parse_http_methods(ossia::string_view str)
{
  string_map<std::string> res;
  auto methods = parse_http_methods_encoded(str);

  for (auto& e : methods)
  {
    std::string key_clean;
    key_clean.reserve(16);
    std::string val_clean;
    val_clean.reserve(16);
    url_decode(e.first, key_clean);
    url_decode(e.second, val_clean);
    res.insert(std::make_pair(std::move(key_clean), std::move(val_clean)));
  }

  return res;
}

void query_parser::parse(
    net::access_mode_attribute, const std::string& data,
    net::address_data& res)
{
  try
  {
    auto i = boost::lexical_cast<int>(data);
    switch (i)
    {
      case 1:
        res.access = ossia::access_mode::GET;
        break;
      case 2:
        res.access = ossia::access_mode::SET;
        break;
      case 3:
        res.access = ossia::access_mode::BI;
        break;
      default:
        break;
    }
  }
  catch (...)
  {
  }
}

void query_parser::parse(
    net::bounding_mode_attribute, const std::string& data,
    net::address_data& res)
{
  if (data.size() > 2)
  {
    // we compare with the first unique letter
    switch (data[2])
    {
      case 'n':
        res.bounding = ossia::bounding_mode::FREE;
        break;
      case 't':
        res.bounding = ossia::bounding_mode::CLIP;
        break;
      case 'w':
        res.bounding = ossia::bounding_mode::LOW;
        break;
      case 'g':
        res.bounding = ossia::bounding_mode::HIGH;
        break;
      case 'a':
        res.bounding = ossia::bounding_mode::WRAP;
        break;
      case 'l':
        res.bounding = ossia::bounding_mode::FOLD;
        break;
      default:
        break;
    }
  }
}

bool query_parser::parse_bool(const std::string& data)
{
  if (data.size() > 0)
  {
    switch (data[0])
    {
      case 't':
      case 'T':
        return true;
      case 'f':
      case 'F':
        return false;
      default:
        break;
    }
  }

  throw std::runtime_error("Invalid bool");
}

void query_parser::parse(
    net::repetition_filter_attribute, const std::string& data,
    net::address_data& res)
{
  try
  {
    bool b = parse_bool(data);
    res.repetition_filter
        = b ? ossia::repetition_filter::ON : ossia::repetition_filter::OFF;
  }
  catch (...)
  {
  }
}

void query_parser::parse(
    net::refresh_rate_attribute attr, const std::string& data,
    net::address_data& res)
{
  try
  {
    using type = decltype(attr)::type;
    auto i = boost::lexical_cast<type>(data);
    attr.setter(res, i);
  }
  catch (...)
  {
  }
}

void query_parser::parse(
    net::priority_attribute attr, const std::string& data,
    net::address_data& res)
{
  try
  {
    using type = decltype(attr)::type;
    auto i = boost::lexical_cast<type>(data);
    attr.setter(res, i);
  }
  catch (...)
  {
  }
}

void query_parser::parse(
    net::value_step_size_attribute attr, const std::string& data,
    net::address_data& res)
{
  try
  {
    using type = decltype(attr)::type;
    auto i = boost::lexical_cast<type>(data);
    attr.setter(res, i);
  }
  catch (...)
  {
  }
}

void query_parser::parse(
    net::critical_attribute attr, const std::string& data,
    net::address_data& res)
{
  try
  {
    bool b = parse_bool(data);
    attr.setter(res, b);
  }
  catch (...)
  {
  }
}

void query_parser::parse(
    net::description_attribute attr, const std::string& data,
    net::address_data& res)
{
  attr.setter(res, data);
}

void query_parser::parse(
    net::app_name_attribute attr, const std::string& data,
    net::address_data& res)
{
  attr.setter(res, data);
}

void query_parser::parse(
    net::app_version_attribute attr, const std::string& data,
    net::address_data& res)
{
  attr.setter(res, data);
}

void query_parser::parse(
    net::app_creator_attribute attr, const std::string& data,
    net::address_data& res)
{
  attr.setter(res, data);
}

void query_parser::parse(
    net::value_attribute attr, const std::string& data, net::address_data& res)
{
  // attr.setter(res, data);
  // Note : we can leverage the fact that this is called after the unit, type,
  // etc. have been parsed
}

void query_parser::parse(
    net::domain_attribute attr, const std::string& data,
    net::address_data& res)
{
  // attr.setter(res, data);
}

void query_parser::parse(
    net::tags_attribute, const std::string& data, net::address_data& res)
{
  // TODO
}

void query_parser::parse(
    net::instance_bounds_attribute attr, const std::string& data,
    net::address_data& res)
{
  // attr.setter(res, data);
}

void query_parser::parse(
    net::unit_attribute attr, const std::string& data, net::address_data& res)
{
  res.unit = parse_pretty_unit(data);
}

void query_parser::parse(
    net::extended_type_attribute attr, const std::string& data,
    net::address_data& res)
{
  attr.setter(res, data);
}

void query_parser::parse(
    net::default_value_attribute attr, const std::string& data,
    net::address_data& res)
{
  attr.setter(res, data);
}

void query_parser::parse(
    detail::typetag_attribute attr, const std::string& data,
    net::address_data& res)
{
  res.type = get_type_from_osc_typetag(data);
}
}
}
