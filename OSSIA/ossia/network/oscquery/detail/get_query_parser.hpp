#pragma once
#include <ossia/detail/string_map.hpp>
#include <ossia/network/exceptions.hpp>
#include <ossia/network/oscquery/detail/json_writer.hpp>
#include <ossia/network/oscquery/oscquery_client.hpp>
#include <boost/lexical_cast.hpp>
#include <ossia/detail/small_vector.hpp>
#include <ossia/network/oscquery/oscquery_server.hpp>
#include <ossia/network/oscquery/detail/html_writer.hpp>
namespace ossia
{
namespace net
{
struct parameter_data;
}
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
  static json_writer::string_t handle_listen(
      oscquery_server_protocol& proto,
      const oscquery_server_protocol::connection_handler& hdl,
      ossia::net::node_base& node, ossia::string_view path,
      const std::string& listen_text)
  {
    // First we find for a corresponding client
    auto clt = proto.find_client(hdl);

    if (clt)
    {
      // Then we enable / disable listening
      if (listen_text == detail::text_true())
      {
        clt->start_listen(std::string(path), node.get_parameter());
        return {};
      }
      else if (listen_text == detail::text_false())
      {
        clt->stop_listen(std::string(path));
        return {};
      }
      else
      {
        throw bad_request_error{"Wrong arguments to listen query: "
                                + listen_text};
        return {};
      }
    }
    else
    {
      throw bad_request_error{"Client not found"};
      return {};
    }
  }

  static json_writer::string_t handle_osc_port(
      oscquery_server_protocol& proto,
      const oscquery_server_protocol::connection_handler& hdl,
      int port,
      int remotePort = 0)
  {
    // First we find for a corresponding client
    auto clt = proto.find_building_client(hdl);

    if (!clt)
    {
      // It's an http-connecting client - we can't open a streaming connection to it
      throw bad_request_error{"Client not found"};
    }

    if(port != 0)
    {
      // Then we set-up the sender
      clt->open_osc_sender(proto.get_logger(), port);
      if(remotePort != 0)
      {
        clt->remote_sender_port = remotePort;
      }
    }

    proto.enable_client(hdl);
    return {};
  }

  auto operator()(oscquery_server_protocol& proto, const oscquery_server_protocol::connection_handler& hdl)
  {
    return [&proto,&hdl] (ossia::string_view path, string_map<std::string>&& parameters) -> server_reply
    {
      // Here we handle the url elements relative to oscquery
      if (parameters.size() == 0)
      {
        auto& root = proto.get_device().get_root_node();
        if (path == "/")
        {
          return oscquery::json_writer::query_namespace(root);
        }
        else
        {
          auto node = ossia::net::find_node(root, path);
          if (node)
            return oscquery::json_writer::query_namespace(*node);
          else
            throw node_not_found_error{std::string(path)};
        }
      }
      else
      {
        auto host_it = parameters.find("HOST_INFO");
        if (host_it == parameters.end())
        {
          auto node = ossia::net::find_node(
              proto.get_device().get_root_node(), path);
          // First check if we have the path
          if (!node)
            throw node_not_found_error{std::string(path)};


          // LISTEN
          auto listen_it = parameters.find(detail::listen());
          if (listen_it != parameters.end())
          {
            return handle_listen(proto, hdl, *node, path, listen_it->second);
          }

          // HTML
          auto html_it = parameters.find("HTML");
          if (html_it != parameters.end())
          {
            return static_html_builder{}.build_tree(*node);
          }

          // SET_PORT
          auto set_osc_port_it = parameters.find(detail::set_port());
          if (set_osc_port_it != parameters.end())
          {
            auto remote_osc_port_it = parameters.find(detail::local_port());
            if(remote_osc_port_it != parameters.end())
            {
              return handle_osc_port(
                  proto,
                    hdl,
                    boost::lexical_cast<int>(set_osc_port_it->second),
                    boost::lexical_cast<int>(remote_osc_port_it->second)
                    );
            }
            else
            {
              return handle_osc_port(
                  proto, hdl, boost::lexical_cast<int>(set_osc_port_it->second));
            }
          }

          // ADD_NODE
          auto add_instance_it = parameters.find(detail::add_node());
          if (add_instance_it != parameters.end())
          {
            proto.add_node(path, std::move(parameters));
            return {};
          }

          // REMOVE_NODE
          auto rm_instance_it = parameters.find(detail::remove_node());
          if (rm_instance_it != parameters.end())
          {
            // Value is the child to remove
            proto.remove_node(path, rm_instance_it.value());
            return {};
          }

          // All the value-less parameters
          ossia::small_vector<std::string, 5> attributes;
          for (const auto& elt : parameters)
          {
            if (elt.second.empty())
            {
              attributes.push_back(elt.first);
            }
          }

          if (!attributes.empty())
          {
            return oscquery::json_writer::query_attributes(*node, attributes);
          }
        }
        else
        {
          return oscquery::json_writer::query_host_info(proto);
        }
      }
      return {};
    };
  }
};
}
}
