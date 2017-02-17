#pragma once
#include <ossia/network/exceptions.hpp>
#include <ossia/network/oscquery/detail/attributes.hpp>
#include <ossia/network/oscquery/detail/json_reader.hpp>
#include <ossia/network/oscquery/detail/json_writer.hpp>
#include <ossia/network/base/address_data.hpp>
#include <ossia/network/base/node.hpp>
#include <chobo/small_vector.hpp>
#include <boost/container/static_vector.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/spirit/home/qi.hpp>
#include <boost/spirit/home/qi/directive/omit.hpp>
#include <boost/fusion/include/std_pair.hpp>
#include <hopscotch_map.h>
#include <websocketpp/connection.hpp>
#include <string>
#include <vector>

namespace ossia
{
namespace net { struct address_data; }
namespace oscquery
{

// query_grammar : taken from https://github.com/ssiloti/http/blob/master/http/parsers/request.hpp
// Copyright (c) 2010 Steven Siloti (ssiloti@gmail.com)
// Distributed under the Boost Software License, Version 1.0.
template <typename Iterator>
struct query_grammar : public boost::spirit::qi::grammar<Iterator, string_map<std::string>()>
{
    query_grammar() : query_grammar::base_type(query_string)
    {
        using namespace boost::spirit;

        query_string = (+qchar >> -(qi::lit('=') >> +qchar)) % qi::lit("&:");
        qchar = ~qi::char_("&:=");
    }

    boost::spirit::qi::rule<Iterator, string_map<std::string>()> query_string;
    boost::spirit::qi::rule<Iterator, char()> qchar;
};

/**
* @brief The query_parser class
*
* Parse OSCQuery queries.
* The queries are similar to the GET part of an http request.
* i.e. /a/b?value, etc...
*/
class query_parser
{
public:
  //! url_decode taken from boost
  // Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
  // Distributed under the Boost Software License, Version 1.0.
  static bool url_decode(const std::string& in, std::string& out)
  {
    out.clear();
    out.reserve(in.size());
    const int N = in.size();
    for (int i = 0; i < N; ++i)
    {
      switch(in[i])
      {
        case '%':
        {
          if (i + 3 <= N)
          {
            int value = 0;
            std::istringstream is(in.substr(i + 1, 2));
            if (is >> std::hex >> value)
            {
              out += static_cast<char>(value);
              i += 2;
            }
            else
            {
              return false;
            }
          }
          else
          {
            return false;
          }
        }
        case '+':
          out += ' ';
          break;
        case '\0':
          return true;
        default:
          out += in[i];
          break;
      }

    }
    return true;
  }

  static auto parse_http_methods(ossia::string_view str)
  {
    // TODO a vector would be more efficient.
     string_map<std::string> methods;
     boost::spirit::qi::parse(
           str.cbegin(), str.cend(),
           query_grammar<ossia::string_view::const_iterator>(),
           methods);

     string_map<std::string> res;
     std::string key_clean; key_clean.reserve(64);
     std::string val_clean; val_clean.reserve(64);
     for(auto& e : methods)
     {
       url_decode(e.first, key_clean);
       url_decode(e.second, val_clean);
       res.insert(std::make_pair(std::move(key_clean), std::move(val_clean)));
     }

     return res;
  }
/*
  static auto parse_http_methods_old(const std::string& requests)
  {
    using string_view = std::experimental::string_view;
    // Parse the methods
    string_map<std::string> arguments_map;
    // Parse foo=bar&bluh=" hello & \"bye\" ! "
    if(!requests.empty())
    {
      string_view key;

      while(!requests.empty())
      {
        // Find the end of the key.
        auto end_equal = requests.find_first_of('=');
        auto end_ampers = requests.find_first_of('&');
        if(end_equal < end_ampers)
        { // key=value
          key = requests.substr(0, end_equal);
          requests.remove_prefix(end_equal + 1); // requests[0] is now first character of value
          if(!requests.empty())
          {
            if(end_ampers != std::string::npos)
            {
              arguments_map.insert(std::make_pair(key, requests.substr(0, end_ampers)));
              requests.remove_prefix(end_ampers + 1); // requests[0] is now first character of next key (or nothing)
            }
            else
            { // no more ampersand, last key
              arguments_map.insert(std::make_pair(key, requests));
              requests = {};
            }
          }
          else
          { // key=
            // no more ampersand
            arguments_map.insert(std::make_pair(key, string_view{}));
            requests = {};
          }
        }
        else
        { // key&...
          arguments_map.insert(std::make_pair(requests.substr(0, end_ampers), string_view{}));
          if(end_ampers != std::string::npos)
            requests.remove_prefix(end_ampers + 1);
          else
            requests = {}; // Was the last key
        }
        key = {};
      }
    }

    return arguments_map;
  }*/

  template<typename Mapper>
  static auto parse_http_request(const std::string& request, Mapper&& mapper)
  {
    using namespace boost;
    using chobo::small_vector;
    using boost::container::static_vector;
    using namespace std;

    // Split on "?"
    ossia::string_view path;
    ossia::string_view queries;

    auto idx = request.find_first_of('?');
    if(idx != std::string::npos)
    {
      path = ossia::string_view(request.data(), idx);
      queries = ossia::string_view(request.data() + idx + 1, request.size() - idx);
    }
    else
    {
      path = request;
    }

    return mapper(path, parse_http_methods(queries));
  }


  template<typename Mapper>
  static auto parse_json_request(const std::string& request, Mapper&& mapper)
  {
    // [ { "/addr/val" : 123 } ]
    rapidjson::Document doc;
    doc.Parse(request); // TODO ParseInsitu

    return mapper(doc);
  }

  static void parse(net::access_mode_attribute, const std::string& data, net::address_data& res) {
    try {
      auto i = boost::lexical_cast<int>(data);
      switch(i)
      {
        case 1: res.access = ossia::access_mode::GET; break;
        case 2: res.access = ossia::access_mode::SET; break;
        case 3: res.access = ossia::access_mode::BI;  break;
        default: break;
      }
    } catch(...) { } }

  static void parse(net::bounding_mode_attribute, const std::string& data, net::address_data& res)
  {
    if(data.size() > 2)
    {
      // we compare with the first unique letter
      switch(data[2])
      {
        case 'n': res.bounding = ossia::bounding_mode::FREE; break;
        case 't': res.bounding = ossia::bounding_mode::CLIP; break;
        case 'w': res.bounding = ossia::bounding_mode::LOW;  break;
        case 'g': res.bounding = ossia::bounding_mode::HIGH; break;
        case 'a': res.bounding = ossia::bounding_mode::WRAP; break;
        case 'l': res.bounding = ossia::bounding_mode::FOLD; break;
        default:
          break;
      }
    }
  }

  static bool parse_bool(const std::string& data)
  {
    if(data.size() > 0)
    {
      switch(data[0])
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

  static void parse(net::repetition_filter_attribute, const std::string& data, net::address_data& res)
  {
    try {
      bool b = parse_bool(data);
      res.repetition_filter = b ? ossia::repetition_filter::ON : ossia::repetition_filter::OFF;
    } catch (...) { }
  }
  static void parse(net::refresh_rate_attribute attr, const std::string& data, net::address_data& res)
  {
    try {
      using type = decltype(attr)::type;
      auto i = boost::lexical_cast<type>(data);
      attr.setter(res, i);
    } catch(...) { }
  }
  static void parse(net::priority_attribute attr, const std::string& data, net::address_data& res)
  {
    try {
      using type = decltype(attr)::type;
      auto i = boost::lexical_cast<type>(data);
      attr.setter(res, i);
    } catch(...) { }
  }
  static void parse(net::value_step_size_attribute attr, const std::string& data, net::address_data& res)
  {
    try {
      using type = decltype(attr)::type;
      auto i = boost::lexical_cast<type>(data);
      attr.setter(res, i);
    } catch(...) { }
  }
  static void parse(net::critical_attribute attr, const std::string& data, net::address_data& res)
  {
    try {
      bool b = parse_bool(data);
      attr.setter(res, b);
    } catch (...) { }
  }
  static void parse(net::description_attribute attr, const std::string& data, net::address_data& res)
  {
    attr.setter(res, data);
  }
  static void parse(net::app_name_attribute attr, const std::string& data, net::address_data& res)
  {
    attr.setter(res, data);
  }
  static void parse(net::app_version_attribute attr, const std::string& data, net::address_data& res)
  {
    attr.setter(res, data);
  }
  static void parse(net::app_creator_attribute attr, const std::string& data, net::address_data& res)
  {
    attr.setter(res, data);
  }
  static void parse(net::value_attribute attr, const std::string& data, net::address_data& res)
  {
    // TODO
    // Note : we can leverage the fact that this is called after the unit, type, etc. have been parsed
  }

  static void parse(net::domain_attribute attr, const std::string& data, net::address_data& res)
  {
    // TODO
  }

  static void parse(net::tags_attribute, const std::string& data, net::address_data& res)
  {
    // TODO
  }
  static void parse(net::instance_bounds_attribute attr, const std::string& data, net::address_data& res)
  {
    // TODO
  }

  static void parse(net::unit_attribute attr, const std::string& data, net::address_data& res)
  {
    res.unit = parse_pretty_unit(data);
  }
  static void parse(net::extended_type_attribute attr, const std::string& data, net::address_data& res)
  {
    attr.setter(res, data);
  }
  static void parse(net::default_value_attribute attr, const std::string& data, net::address_data& res)
  {
    // TODO
  }
  static void parse(oscquery::detail::typetag_attribute attr, const std::string& data, net::address_data& res)
  {
    res.type = get_type_from_osc_typetag(data);
  }

};


/**
 * @brief The Answerer class
 *
 * OSCQuery query-answering logic.
 */
class query_answerer
{
public:
  template<typename Protocol>
  static auto answer_http_request (Protocol& proto, const typename Protocol::connection_handler& hdl)
  {
    return [&] (
        ossia::string_view path,
        string_map<std::string>&& parameters)
    {
      // Here we handle the url elements relative to oscquery
      if(parameters.size() == 0)
      {
        // TODO auto&& lock = dev.map().acquire_read_lock();
        auto& root = proto.getDevice().getRootNode();
        if(path == "/")
          return oscquery::json_writer::query_namespace(root);
        else
        {
          auto node = ossia::net::find_node(root, path);
          if(node)
            return oscquery::json_writer::query_namespace(*node);
          else
            throw node_not_found_error{path.to_string()};
        }
      }
      else
      {
        auto node = ossia::net::find_node(proto.getDevice().getRootNode(), path);
        // First check if we have the path
        if(!node)
          throw node_not_found_error{path.to_string()};

        // Listen
        auto listen_it = parameters.find(detail::listen());
        if(listen_it != parameters.end())
        {
          // First we find for a corresponding client
          auto clt = proto.findClient(hdl);

          if(!clt)
            throw bad_request_error{"Client not found"};
          // Then we enable / disable listening
          if(listen_it->second == detail::text_true())
          {
            clt->start_listen(path.to_string(), node->getAddress());
            return json_writer::string_t{};
          }
          else if(listen_it->second == detail::text_false())
          {
            clt->stop_listen(path.to_string());
            return json_writer::string_t{};
          }
          else
          {
            throw bad_request_error{"Wrong arguments to listen query: " + listen_it->second};
          }
        }

        auto set_osc_port_it = parameters.find(detail::set_port());
        if(set_osc_port_it != parameters.end())
        {
          // First we find for a corresponding client
          auto clt = proto.findBuildingClient(hdl);

          if(!clt)
            throw bad_request_error{"Client not found"};

          // Then we set-up the sender
          clt->openOSCSender(proto.getLogger(), boost::lexical_cast<int>(set_osc_port_it->second));

          proto.enableClient(hdl);
          return json_writer::string_t{};
        }

        auto add_instance_it = parameters.find(detail::add_node());
        if(add_instance_it != parameters.end())
        {
          proto.add_node(path, std::move(parameters));
          return json_writer::string_t{};
        }
        auto rm_instance_it = parameters.find(detail::remove_node());
        if(rm_instance_it != parameters.end())
        {
          // Value is the child to remove
          proto.remove_node(path, rm_instance_it.value());
          return json_writer::string_t{};
        }

        // All the value-less parameters
        chobo::small_vector<std::string, 5> attributes;
        for(const auto& elt : parameters)
        {
          if(elt.second.empty())
          {
            attributes.push_back(elt.first);
          }
        }

        if(!attributes.empty())
        {
          return oscquery::json_writer::query_attributes(
                *node, attributes);
        }
      }
      return json_writer::string_t{};
    };
  }


  template<typename Protocol>
  static auto answer_json_request (Protocol& proto, const typename Protocol::connection_handler& hdl)
  {
    return  [&] (const rapidjson::Document& doc) {

      if(doc.IsArray())
      {
        const auto& arr = doc.GetArray();
        for(const auto& e : arr)
        {
          json_parser::parse_address_value(proto.getDevice().getRootNode(), e);
        }
      }
      else
      {
        json_parser::parse_address_value(proto.getDevice().getRootNode(), doc);
      }
      return json_writer::string_t{};
    };

  }

};

}
}
