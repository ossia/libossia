#pragma once
#include <ossia/detail/string_map.hpp>
#include <ossia/network/oscquery/detail/json_writer.hpp>
#include <ossia/network/exceptions.hpp>
#include <chobo/small_vector.hpp>
#include <boost/lexical_cast.hpp>

namespace ossia
{
namespace net { struct address_data; }
namespace oscquery
{

/**
 * @brief OSCQuery get query-answering logic.
 *
 * This is used for both http and ws requests.
 */
class get_query_answerer
{
public:
  template<typename Protocol>
  auto operator() (Protocol& proto, const typename Protocol::connection_handler& hdl)
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

};


}
}
