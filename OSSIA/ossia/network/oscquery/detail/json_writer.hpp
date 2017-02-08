#pragma once
#include <ossia/network/oscquery/detail/json_writer_detail.hpp>
#include <ossia/network/oscquery/detail/attributes.hpp>
#include <brigand/algorithms/for_each.hpp>
namespace ossia
{
namespace oscquery
{

//! Creates the JSON message to send through OSCQuery
class json_writer
{

public:
  // Initialisation
  using string_t = rapidjson::StringBuffer;
  using writer_t = rapidjson::Writer<string_t>;

  //! Sends the port at which a server opens its OSC port
  static string_t device_info(int port);

  // Format interface
  // Queries
  //! Reply to the namespace query : /foo/bar
  static string_t query_namespace(
      const ossia::net::node_base& node);

  //! Reply to a query of attributes : /foo/bar?VALUE&RANGE
  template<typename StringVec_T>
  static string_t query_attributes(
      const ossia::net::node_base& node,
      const StringVec_T& methods)
  {
    string_t buf;
    writer_t wr(buf);

    detail::json_writer_impl p{wr};

    // Here we just write the attributes in the object directly
    wr.StartObject();
    for(auto& method : methods)
    {
      writeKey(wr, method);
      p.writeAttribute(node, method);
    }
    wr.EndObject();

    return buf;
  }

  // Update messages
  //! Sent when a new node is added
  static string_t path_added(const ossia::net::node_base& n);

  //! Sent when the content of a node has changed
  static string_t path_changed(const ossia::net::node_base& n);

  //! Sent when a node is being removed
  static string_t path_removed(
      const std::string& path);


  static string_t attributes_changed(
      const ossia::net::node_base& n,
      const std::vector<ossia::string_view>& attributes);

  static string_t paths_added(
      const std::vector<const ossia::net::node_base*>& vec);

  static string_t paths_changed(
      const std::vector<const ossia::net::node_base*>& vec);

  static string_t paths_removed(
      const std::vector<std::string>& vec);

  static string_t attributes_changed_array(
      const std::vector<
      std::pair<
      const ossia::net::node_base*,
      std::vector<ossia::string_view>
      >
      >& vec );
private:
  static void path_added_impl(detail::json_writer_impl& p, const ossia::net::node_base& n);
  static void path_changed_impl(detail::json_writer_impl& p, const ossia::net::node_base& n);
  static void path_removed_impl(writer_t& wr, const std::string& path);
  static void attributes_changed_impl(
      detail::json_writer_impl& p,
      const ossia::net::node_base& n,
      const std::vector<ossia::string_view>& attributes);
};


}
}
