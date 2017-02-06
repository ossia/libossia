#pragma once
#include <ossia/network/exceptions.hpp>
#include <boost/container/static_vector.hpp>
#include <chobo/small_vector.hpp>
#include <boost/algorithm/string.hpp>
#include <ossia/network/oscquery/detail/json_writer.hpp>
#include <hopscotch_map.h>
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
   static std::string parse(const std::string& request, Mapper&& mapper)
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
    template<typename Device>
    static auto answer (Device& dev, typename Device::connection_handler& hdl)
    {
      return [&] (
          const std::string& path,
          const tsl::hopscotch_map<std::string, std::string>& parameters)
      {
        // Here we handle the url elements relative to oscquery
        if(parameters.size() == 0)
        {
          // TODO auto&& lock = dev.map().acquire_read_lock();
          return oscquery::writer::query_namespace(dev.getDevice().getRootNode());
        }
        else
        {
          /*
          // First check if we have the path
          if(!dev.map().has(path))
            throw node_not_found_error{path};

          // Listen
          auto listen_it = parameters.find("listen");
          if(listen_it != end(parameters))
          {
            // First we find for a corresponding client
            auto it = find(begin(dev.clients()), end(dev.clients()), hdl);
            if(it == end(dev.clients()))
              throw bad_request_error{"Client not found"};

            // Then we enable / disable listening
            if(listen_it->second == "true")
            {
              it->addListenedPath(path);
            }
            else if(listen_it->second == "false")
            {
              it->removeListenedPath(path);
            }
            else
            {
              throw bad_request_error{"Wrong arguments to listen query"};
            }
          }

          // All the value-less parameters
          std::vector<std::string> attributes;
          attributes.reserve(5);
          for(const auto& elt : parameters)
          {
            if(elt.second.empty())
            {
              attributes.push_back(elt.first);
            }
          }

          if(!attributes.empty())
          {
            return json::writer::query_attributes(
                  dev.map().get(path),
                  attributes);
          }
*/
        }
        return std::string{};
      };
    }

};

}
}
