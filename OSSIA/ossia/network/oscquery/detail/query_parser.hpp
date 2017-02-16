#pragma once
#include <ossia/network/exceptions.hpp>
#include <ossia/network/oscquery/detail/attributes.hpp>
#include <boost/container/static_vector.hpp>
#include <chobo/small_vector.hpp>
#include <boost/algorithm/string.hpp>
#include <ossia/network/oscquery/detail/json_writer.hpp>
#include <ossia/network/base/address_data.hpp>
#include <ossia/network/base/node.hpp>
#include <hopscotch_map.h>
#include <boost/lexical_cast.hpp>
#include <websocketpp/connection.hpp>
#include <string>
#include <vector>


namespace ossia
{
namespace net { struct address_data; }
namespace oscquery
{

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
  template<typename Mapper>
  static auto parse_http_request(const std::string& request, Mapper&& mapper)
  {
    using namespace boost;
    using chobo::small_vector;
    using boost::container::static_vector;
    using namespace std;

    // Split on "?"
    static_vector<std::string, 3> uri_tokens;
    try {
      boost::split(uri_tokens, request, is_any_of("?"));
    }
    catch(...) {
      throw bad_request_error{"Too much '?'"};
    }

    // Parse the methods
    string_map<std::string> arguments_map;
    if(uri_tokens.size() > 1)
    {
      // Then, split the &-separated arguments
      std::vector<string> argument_tokens; // TODO small_vector
      boost::split(argument_tokens, uri_tokens.at(1), is_any_of("&"));

      // Finally, split these arguments at '=' and put them in a map
      for(const auto& arg : argument_tokens)
      {
        static_vector<string, 3> map_tokens;

        try {
          boost::split(map_tokens, arg, is_any_of("="));
        }
        catch(...) {
          throw bad_request_error{"Too much '='"};
        }

        switch(map_tokens.size())
        {
          case 1: // &value
            arguments_map.insert({map_tokens.front(), {}});
            break;
          case 2: // &listen=true
            arguments_map.insert({map_tokens.front(), map_tokens.back()});
            break;
          default:
            throw bad_request_error{"Too much '='"};
            break;
        }
      }
    }

    return mapper(uri_tokens.at(0), std::move(arguments_map));
  }


  template<typename Mapper>
  static auto parse_json_request(const std::string& request, Mapper&& mapper)
  {
    // rapidjson::Document doc;
    // doc.ParseInsitu(request);

    return rapidjson::StringBuffer{};
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
        const std::string& path,
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
            throw node_not_found_error{path};
        }
      }
      else
      {
        auto node = ossia::net::find_node(proto.getDevice().getRootNode(), path);
        // First check if we have the path
        if(!node)
          throw node_not_found_error{path};

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
            clt->start_listen(path, node->getAddress());
          }
          else if(listen_it->second == detail::text_false())
          {
            clt->stop_listen(path);
          }
          else
          {
            throw bad_request_error{"Wrong arguments to listen query"};
          }
        }

        auto set_osc_port_it = parameters.find(detail::set_port());
        if(set_osc_port_it != parameters.end())
        {
          // First we find for a corresponding client
          auto clt = proto.findClient(hdl);

          if(!clt)
            throw bad_request_error{"Client not found"};

          // Then we set-up the sender
          clt->openOSCSender(proto.getLogger(), boost::lexical_cast<int>(set_osc_port_it->second));
        }

        auto add_instance_it = parameters.find(detail::add_node());
        if(add_instance_it != parameters.end())
        {
          proto.add_node(std::move(parameters));
          return json_writer::string_t{};
        }
        auto rm_instance_it = parameters.find(detail::remove_node());
        if(rm_instance_it != parameters.end())
        {
          // Value is the path to remove
          proto.remove_node(rm_instance_it.value());
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
    return [] {};

  }

};

}
}
