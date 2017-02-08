#pragma once
#include <ossia/network/exceptions.hpp>
#include <boost/container/static_vector.hpp>
#include <chobo/small_vector.hpp>
#include <boost/algorithm/string.hpp>
#include <ossia/network/oscquery/detail/json_writer.hpp>
#include <ossia/network/base/node.hpp>
#include <hopscotch_map.h>
#include <boost/lexical_cast.hpp>
#include <websocketpp/connection.hpp>
#include <string>
#include <vector>


namespace ossia
{
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
   static auto parse(const std::string& request, Mapper&& mapper)
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
     tsl::hopscotch_map<std::string, std::string> arguments_map;
     if(uri_tokens.size() > 1)
     {
       // Then, split the &-separated arguments
       std::vector<string> argument_tokens;
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

     return mapper(uri_tokens.at(0), arguments_map);
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
    static auto answer (Protocol& proto, const typename Protocol::connection_handler& hdl)
    {
      return [&] (
          const std::string& path,
          const tsl::hopscotch_map<std::string, std::string>& parameters)
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
          auto listen_it = parameters.find("listen");
          if(listen_it != parameters.end())
          {
            // First we find for a corresponding client
            auto clt = proto.findClient(hdl);

            if(!clt)
              throw bad_request_error{"Client not found"};

            // Then we enable / disable listening
            if(listen_it->second == "true")
            {
              clt->start_listen(path, node->getAddress());
            }
            else if(listen_it->second == "false")
            {
              clt->stop_listen(path);
            }
            else
            {
              throw bad_request_error{"Wrong arguments to listen query"};
            }
          }

          auto set_osc_port_it = parameters.find("set_port");
          if(set_osc_port_it != parameters.end())
          {
            // First we find for a corresponding client
            auto clt = proto.findClient(hdl);

            if(!clt)
              throw bad_request_error{"Client not found"};

            // Then we set-up the sender
            clt->openOSCSender(proto.getLogger(), boost::lexical_cast<int>(set_osc_port_it->second));
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

};

}
}
