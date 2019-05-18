#pragma once
#include <ossia/detail/for_each.hpp>
#include <ossia/network/oscquery/detail/attributes.hpp>
#include <ossia/network/oscquery/detail/json_writer_detail.hpp>

#include <oscpack/ip/UdpSocket.h>
namespace ossia
{
namespace net
{
class network_logger;
}
namespace oscquery
{
class oscquery_server_protocol;
//! Creates the JSON message to send through OSCQuery
class OSSIA_EXPORT json_writer
{
public:
  // Initialisation
  using string_t = rapidjson::StringBuffer;
  using writer_t = rapidjson::Writer<string_t>;

  //! Sends the port at which a server opens its OSC port
  static string_t device_info(int port);

  static string_t query_host_info(const oscquery_server_protocol&);

  // Format interface
  // Queries
  //! Reply to the namespace query : /foo/bar
  static string_t query_namespace(const ossia::net::node_base& node);

  //! Reply to a query of attributes : /foo/bar?VALUE&RANGE
  template <typename StringVec_T>
  static string_t query_attributes(
      const ossia::net::node_base& node, const StringVec_T& methods)
  {
    string_t buf;
    writer_t wr(buf);

    detail::json_writer_impl p{wr};

    // Here we just write the attributes in the object directly
    wr.StartObject();
    for (auto& method : methods)
    {
      // Here we reply to the query which already has
      // the key in the "oscquery" format so no need to convert
      write_json_key(wr, method);
      p.writeAttribute(node, method);
    }
    wr.EndObject();

    return buf;
  }

  // Listen messages
  static string_t listen(ossia::string_view address);
  static string_t ignore(ossia::string_view address);

  // Extensions
  static string_t
  start_osc_streaming(int local_server_port, int local_sender_port);

  // Update messages
  //! Sent when a new node is added
  static string_t path_added(const ossia::net::node_base& n);

  //! Sent when the content of a node has changed
  static string_t path_changed(const ossia::net::node_base& n);

  //! Sent when a node is being removed
  static string_t path_removed(const std::string& path);

  //! Sent when a node is renamed
  static string_t path_renamed(const std::string& old_path, const std::string& new_path);

  static string_t attributes_changed(
      const ossia::net::node_base& n, ossia::string_view attribute);

  static string_t attributes_changed(
      const ossia::net::node_base& n,
      const std::vector<ossia::string_view>& attributes);

  static string_t
  paths_added(const std::vector<const ossia::net::node_base*>& vec);

  static string_t
  paths_changed(const std::vector<const ossia::net::node_base*>& vec);

  static string_t paths_removed(const std::vector<std::string>& vec);

  static string_t attributes_changed_array(
      const std::vector<std::pair<
          const ossia::net::node_base*, std::vector<ossia::string_view>>>&
          vec);

private:
  static void
  path_added_impl(detail::json_writer_impl& p, const ossia::net::node_base& n);
  static void path_changed_impl(
      detail::json_writer_impl& p, const ossia::net::node_base& n);
  static void path_removed_impl(writer_t& wr, const std::string& path);
  static void path_renamed_impl(
      json_writer::writer_t& wr, const std::string& path, const std::string& old);
  static void attribute_changed_impl(
      detail::json_writer_impl& p, const ossia::net::node_base& n,
      ossia::string_view attribute);
  static void attributes_changed_impl(
      detail::json_writer_impl& p, const ossia::net::node_base& n,
      const std::vector<ossia::string_view>& attributes);
};

// TODO this export is only needed for tests...
struct OSSIA_EXPORT osc_writer
{
  static std::string send_message(
      const ossia::net::parameter_base&, const ossia::value&,
      const ossia::net::network_logger&);
  static std::string send_message(
      const ossia::net::full_parameter_data&, const ossia::value&,
      const ossia::net::network_logger&);
  static void send_message(
      const ossia::net::parameter_base&, const ossia::value&,
      const ossia::net::network_logger&, oscpack::UdpTransmitSocket&);
  static void send_message(
      const ossia::net::full_parameter_data&, const ossia::value&,
      const ossia::net::network_logger&, oscpack::UdpTransmitSocket&);
};
}
}
