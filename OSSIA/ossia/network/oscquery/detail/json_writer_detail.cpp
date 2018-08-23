// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "json_writer_detail.hpp"
#include "json_writer.hpp"
#include <ossia/detail/string_view.hpp>
#include <ossia/network/dataspace/dataspace_visitors.hpp>
#include <ossia/network/dataspace/dataspace_variant_visitors.hpp>
#include <ossia/network/value/value.hpp>
#include <ossia/network/base/device.hpp>
#include <ossia/network/domain/domain.hpp>
#include <ossia/network/exceptions.hpp>
#include <ossia/network/oscquery/detail/attributes.hpp>
#include <ossia/network/oscquery/detail/domain_to_json.hpp>
#include <ossia/network/oscquery/detail/outbound_visitor.hpp>
#include <ossia/network/oscquery/oscquery_server.hpp>
#include <ossia/detail/for_each.hpp>
#include <ossia/network/osc/detail/message_generator.hpp>
#include <ossia/network/osc/detail/osc_fwd.hpp>

namespace ossia
{
namespace oscquery
{
namespace detail
{

void json_writer_impl::writeValue(const value& val) const
{
  const auto array = is_array(val);
  if(!array)
  {
    writer.StartArray();
  }
  val.apply(value_to_json{writer});
  if(!array)
  {
    writer.EndArray();
  }
}

void json_writer_impl::writeValue(bounding_mode b) const
{
  switch (b)
  {
    case ossia::bounding_mode::FREE:
      writer.String("none");
      break;
    case ossia::bounding_mode::CLIP:
      writer.String("both");
      break;
    case ossia::bounding_mode::LOW:
      writer.String("low");
      break;
    case ossia::bounding_mode::HIGH:
      writer.String("high");
      break;
    case ossia::bounding_mode::WRAP:
      writer.String("wrap");
      break;
    case ossia::bounding_mode::FOLD:
      writer.String("fold");
      break;
    default:
      throw std::runtime_error("json_writer_impl: bad clip mode");
  }
}

void json_writer_impl::writeValue(access_mode b) const
{
  switch (b)
  {
    case ossia::access_mode::GET:
      writer.Int(1);
      break;
    case ossia::access_mode::SET:
      writer.Int(2);
      break;
    case ossia::access_mode::BI:
      writer.Int(3);
      break;
    default:
      writer.Int(0); // no address case
  }
}

void json_writer_impl::writeValue(const domain& d) const
{
  ossia::apply(domain_to_json{writer}, d);
}

void json_writer_impl::writeValue(const unit_t& d) const
{
  writer.String(ossia::get_pretty_unit_text(d));
}

void json_writer_impl::writeValue(const net::tags& tags) const
{
  writer.StartArray();

  for (const auto& tag : tags)
  {
    writer.String(tag);
  }

  writer.EndArray();
}

void json_writer_impl::writeKey(ossia::string_view k) const
{
  ::write_json_key(writer, k);
}
void json_writer_impl::writeValue(int32_t i) const
{
  writer.Int(i);
}
void json_writer_impl::writeValue(float i) const
{
  if(!writer.Double(i))
      writer.Null();
}
void json_writer_impl::writeValue(double i) const
{
  if(!writer.Double(i))
    writer.Null();
}
void json_writer_impl::writeValue(bool i) const
{
  writer.Bool(i);
}
void json_writer_impl::writeValue(const std::string& i) const
{
  writer.String(i);
}
void json_writer_impl::writeValue(const repetition_filter& i) const
{
  writeValue(i == repetition_filter::ON);
}

void json_writer_impl::writeValue(const net::instance_bounds& i) const
{
  writer.StartArray();
  writer.Int(i.min_instances);
  writer.Int(i.max_instances);
  writer.EndArray();
}

using writer_map_fun
    = void (*)(const json_writer_impl&, const ossia::net::node_base&);
using writer_map_type = string_view_map<writer_map_fun>;

template <typename Attr>
static auto make_fun_pair()
{
  return std::make_pair(
      metadata<Attr>::key(),
      [](const json_writer_impl& self, const ossia::net::node_base& n) {
        self.writeValue(Attr::getter(n));
      });
}

static const auto& attributesMap()
{
  static const writer_map_type attr_map{[] {
    writer_map_type attr_impl;

    attr_impl.insert(make_fun_pair<full_path_attribute>());

    // Add the "writeValue" function to the map for every attribute
    ossia::for_each_tagged(base_attributes{}, [&](auto attr) {
      using type = typename decltype(attr)::type;
      attr_impl.insert(make_fun_pair<type>());
    });
    ossia::for_each_tagged(extended_attributes{}, [&](auto attr) {
      using type = typename decltype(attr)::type;
      attr_impl.insert(make_fun_pair<type>());
    });

    return attr_impl;
  }()};

  return attr_map;
}

void json_writer_impl::writeAttribute(
    const net::node_base& n, ossia::string_view method) const
{
  // We put all our attributes in a map.
  // Look into the map and call writeValue(theAttribute), c.f. make_fun_pair.
  auto& attr_map = attributesMap();

  auto it = attr_map.find(method);
  if (it != attr_map.end())
  {
    it.value()(*this, n);
  }
  else
  {
    throw ossia::bad_request_error{"Attribute not found"};
  }
}

struct node_attribute_writer
{
  const net::node_base& n;
  const json_writer_impl& writer;

  template<typename T>
  void operator()(const T&)
  {
    using Attr = typename T::type;
    auto res = Attr::getter(n);
    if (ossia::net::valid(res))
    {
      writer.writeKey(metadata<Attr>::key());
      writer.writeValue(res);
    }
  }
};

void json_writer_impl::writeNodeAttributes(const net::node_base& n) const
{
  using namespace std;
  using namespace eggs::variants;

  auto addr = n.get_parameter();

  // We are already in an object
  // These attributes are always here
  writeKey(detail::attribute_full_path());

  writer.String(n.osc_address());

  // Handling of the types / values
  if (addr)
  {
    // TODO it could be nice to have versions that take a parameter or a value
    // directly
    ossia::for_each_tagged(base_attributes{}, node_attribute_writer{n, *this});
  }
  else
  {
    writeKey(detail::attribute_accessmode());
    writer.Int(0);
  }

  ossia::for_each_tagged(extended_attributes{}, [&](auto attr) {
    using Attr = typename decltype(attr)::type;
    auto res = Attr::getter(n);
    if (ossia::net::valid(res))
    {
      this->writeKey(metadata<Attr>::key());
      this->writeValue(res);
    }
  });
}

void json_writer_impl::writeNode(const net::node_base& n)
{
  writer.StartObject();
  writeNodeAttributes(n);

  const auto& cld = n.children();

  if (!cld.empty())
  {
    writeKey(detail::contents());
    writer.StartObject();
    for (const auto& child : cld)
    {
      writer.String(child->get_name());
      writeNode(*child);
    }
    writer.EndObject();
  }

  writer.EndObject();
}
}

void json_writer::path_added_impl(
    detail::json_writer_impl& p, const net::node_base& n)
{
  auto& wr = p.writer;
  wr.StartObject();

  write_json_key(wr, detail::command());
  write_json(wr, detail::path_added());

  write_json_key(wr, detail::data());
  p.writeNode(n);

  wr.EndObject();
}

void json_writer::path_changed_impl(
    detail::json_writer_impl& p, const net::node_base& n)
{
  auto& wr = p.writer;
  wr.StartObject();

  write_json_key(wr, detail::command());
  write_json(wr, detail::path_changed());

  write_json_key(wr, detail::data());
  p.writeNode(n);

  wr.EndObject();
}

void json_writer::path_removed_impl(
    json_writer::writer_t& wr, const std::string& path)
{
  wr.StartObject();

  write_json_key(wr, detail::command());
  write_json(wr, detail::path_removed());

  write_json_key(wr, detail::data());
  wr.String(path);

  wr.EndObject();
}

void json_writer::attribute_changed_impl(
    detail::json_writer_impl& p, const net::node_base& n,
    ossia::string_view attr)
{
  auto& wr = p.writer;
  wr.StartObject();

  write_json_key(wr, detail::command());
  write_json(wr, detail::attributes_changed());

  write_json_key(wr, detail::data());
  {
    wr.StartObject();

    write_json_key(wr, detail::attribute_full_path());
    wr.String(n.osc_address());

    auto& map = ossia_to_oscquery_key();
    auto it = map.find(attr);
    if (it != map.end())
    {
      write_json_key(wr, it.value());
      p.writeAttribute(n, it.value());
    }

    wr.EndObject();
  }

  wr.EndObject();
}

void json_writer::attributes_changed_impl(
    detail::json_writer_impl& p, const net::node_base& n,
    const std::vector<ossia::string_view>& attributes)
{
  auto& wr = p.writer;
  wr.StartObject();

  write_json_key(wr, detail::command());
  write_json(wr, detail::attributes_changed());

  write_json_key(wr, detail::data());
  auto& map = ossia_to_oscquery_key();
  {
    wr.StartObject();

    write_json_key(wr, detail::attribute_full_path());
    wr.String(n.osc_address());

    for (auto& attr : attributes)
    {
      auto it = map.find(attr);
      if (it != map.end())
      {
        write_json_key(wr, it.value());
        p.writeAttribute(n, it.value());
      }
    }
    wr.EndObject();
  }

  wr.EndObject();
}

void json_writer::send_message_impl(
    detail::json_writer_impl& p, const net::parameter_base& n, const value& val)
{
  auto& wr = p.writer;
  wr.StartObject();

  write_json_key(wr, n.get_node().osc_address());
  p.writeValue(val);

  wr.EndObject();
}
void json_writer::send_message_impl(
    detail::json_writer_impl& p, const net::full_parameter_data& n, const value& val)
{
  auto& wr = p.writer;
  wr.StartObject();

  write_json_key(wr, n.address);
  p.writeValue(val);

  wr.EndObject();
}

json_writer::string_t json_writer::device_info(int port)
{
  string_t buf;
  writer_t wr(buf);

  wr.StartObject();
  write_json_key(wr, detail::osc_port());
  wr.Int(port);
  wr.EndObject();

  return buf;
}

json_writer::string_t
json_writer::query_host_info(const oscquery_server_protocol& proto)
{

  string_t buf;
  writer_t wr(buf);

  wr.StartObject();
  wr.Key("NAME");
  wr.String(proto.get_device().get_name());
  wr.Key("OSC_PORT");
  wr.Int(proto.get_osc_port());
  wr.Key("OSC_TRANSPORT");
  wr.String("UDP");

  wr.Key("EXTENSIONS");
  wr.StartObject();

  wr.Key("TYPE");
  wr.Bool(true);
  wr.Key("ACCESS");
  wr.Bool(true);
  wr.Key("VALUE");
  wr.Bool(true);
  wr.Key("RANGE");
  wr.Bool(true);
  wr.Key("TAGS");
  wr.Bool(true);
  wr.Key("CLIPMODE");
  wr.Bool(true);
  wr.Key("UNIT");
  wr.Bool(true);
  wr.Key("CRITICAL");
  wr.Bool(true);
  wr.Key("DESCRIPTION");
  wr.Bool(true);

  wr.Key("HTML");
  wr.Bool(true);

  wr.Key("OSC_STREAMING");
  wr.Bool(true);

  wr.Key("LISTEN");
  wr.Bool(true);

  wr.Key("ECHO");
  wr.Bool(true);

  wr.Key("PATH_CHANGED");
  wr.Bool(false);
  wr.Key("PATH_RENAMED");
  wr.Bool(false);
  wr.Key("PATH_ADDED");
  wr.Bool(true);
  wr.Key("PATH_REMOVED");
  wr.Bool(true);

  wr.EndObject();

  wr.EndObject();

  return buf;
}

json_writer::string_t json_writer::query_namespace(const net::node_base& node)
{
  string_t buf;
  writer_t wr(buf);

  detail::json_writer_impl p{wr};
  p.writeNode(node);

  return buf;
}

json_writer::string_t json_writer::listen(string_view address)
{
  string_t buf;
  writer_t wr(buf);

  wr.StartObject();

  write_json_key(wr, detail::command());
  write_json(wr, detail::listen());

  write_json_key(wr, detail::data());
  write_json(wr, address);

  wr.EndObject();

  return buf;
}

json_writer::string_t json_writer::ignore(string_view address)
{
  string_t buf;
  writer_t wr(buf);

  wr.StartObject();

  write_json_key(wr, detail::command());
  write_json(wr, detail::ignore());

  write_json_key(wr, detail::data());
  write_json(wr, address);

  wr.EndObject();

  return buf;
}

json_writer::string_t json_writer::start_osc_streaming(int local_server_port, int local_sender_port)
{
  string_t buf;
  writer_t wr(buf);

  wr.StartObject();

  write_json_key(wr, detail::command());
  write_json(wr, detail::start_osc_streaming());

  write_json_key(wr, detail::data());
  {
    wr.StartObject();
    write_json_key(wr, detail::local_server_port());
    wr.Int(local_server_port);
    write_json_key(wr, detail::local_sender_port());
    wr.Int(local_sender_port);
    wr.EndObject();
  }

  wr.EndObject();

  return buf;
}


json_writer::string_t json_writer::path_added(const net::node_base& n)
{
  string_t buf;
  writer_t wr(buf);

  detail::json_writer_impl p{wr};

  path_added_impl(p, n);

  return buf;
}

json_writer::string_t json_writer::path_changed(const net::node_base& n)
{
  string_t buf;
  writer_t wr(buf);

  detail::json_writer_impl p{wr};

  path_changed_impl(p, n);

  return buf;
}

json_writer::string_t json_writer::path_removed(const std::string& path)
{
  string_t buf;
  writer_t wr(buf);

  path_removed_impl(wr, path);

  return buf;
}

json_writer::string_t json_writer::attributes_changed(
    const net::node_base& n, ossia::string_view attribute)
{
  string_t buf;
  writer_t wr(buf);

  detail::json_writer_impl p{wr};

  attribute_changed_impl(p, n, attribute);

  return buf;
}

json_writer::string_t
json_writer::send_message(const net::parameter_base& addr, const value& val)
{
  string_t buf;
  writer_t wr(buf);

  detail::json_writer_impl p{wr};

  send_message_impl(p, addr, val);

  return buf;
}

json_writer::string_t
json_writer::send_message(const net::full_parameter_data& addr, const value& val)
{
  string_t buf;
  writer_t wr(buf);

  detail::json_writer_impl p{wr};

  send_message_impl(p, addr, val);

  return buf;
}

json_writer::string_t json_writer::attributes_changed(
    const net::node_base& n, const std::vector<ossia::string_view>& attributes)
{
  string_t buf;
  writer_t wr(buf);

  detail::json_writer_impl p{wr};

  attributes_changed_impl(p, n, attributes);

  return buf;
}

json_writer::string_t
json_writer::paths_added(const std::vector<const net::node_base*>& vec)
{
  string_t buf;
  writer_t wr(buf);

  detail::json_writer_impl p{wr};

  wr.StartArray();
  for (auto node : vec)
    path_added_impl(p, *node);
  wr.EndArray();

  return buf;
}

json_writer::string_t
json_writer::paths_changed(const std::vector<const net::node_base*>& vec)
{
  string_t buf;
  writer_t wr(buf);

  detail::json_writer_impl p{wr};

  wr.StartArray();
  for (auto node : vec)
    path_changed_impl(p, *node);
  wr.EndArray();

  return buf;
}

json_writer::string_t
json_writer::paths_removed(const std::vector<std::string>& vec)
{
  string_t buf;
  writer_t wr(buf);

  wr.StartArray();
  for (const auto& str : vec)
    path_removed_impl(wr, str);
  wr.EndArray();

  return buf;
}

json_writer::string_t json_writer::attributes_changed_array(
    const std::
        vector<std::
                   pair<const net::node_base*, std::vector<ossia::string_view>>>&
            vec)
{
  string_t buf;
  writer_t wr(buf);

  detail::json_writer_impl p{wr};

  wr.StartArray();
  for (const auto& val : vec)
    attributes_changed_impl(p, *val.first, val.second);
  wr.EndArray();

  return buf;
}

std::string write_value(std::string_view address, const value& v, const unit_t& u)
{
  std::string buffer;
  buffer.resize(1024);
  oscpack::OutboundPacketStream p{buffer.data(), buffer.size()};

  while(true)
  {
    try
    {
      p << oscpack::BeginMessageN(address);
      if(!u)
      {
        v.apply(oscquery::osc_outbound_visitor{p});
      }
      else
      {
        ossia::apply_nonnull([&] (const auto& dataspace) {
          ossia::apply(oscquery::osc_outbound_visitor{p}, v.v, dataspace);
        }, u.v);
      }
      p << oscpack::EndMessage();
      break;
    }
    catch(const oscpack::OutOfBufferMemoryException&)
    {
      buffer.resize(buffer.size() * 2);
      p.Clear();
    }
  }

  return buffer;
}

std::string osc_writer::send_message(const net::parameter_base& p, const value& v)
{
  return write_value(p.get_node().osc_address(), v, p.get_unit());
}

std::string osc_writer::send_message(const net::full_parameter_data& p, const value& v)
{
  return write_value(p.address, v, p.unit);
}


}
}
