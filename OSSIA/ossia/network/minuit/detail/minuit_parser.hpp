#pragma once
#include <ossia/network/generic/generic_address.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/generic/generic_node.hpp>
#include <ossia/network/minuit/detail/minuit_common.hpp>
#include <ossia/network/minuit/minuit.hpp>
#include <ossia/network/osc/detail/osc.hpp>
#include <oscpack/osc/OscReceivedElements.h>
#include <oscpack/osc/OscPrintReceivedElements.h>
#include <ossia/network/osc/detail/sender.hpp>
#include <boost/container/small_vector.hpp>
#include <ossia/editor/dataspace/dataspace_visitors.hpp>
#define MINUIT_WAIT_TIME
namespace ossia
{
namespace minuit
{
template <minuit_command Req, minuit_operation Op>
struct minuit_behavior
{
  void operator()(
      ossia::net::minuit_protocol& proto, ossia::net::generic_device& dev,
      const oscpack::ReceivedMessage& mess);
};

template <minuit_operation Op>
struct minuit_behavior<minuit_command::Error, Op>
{
  void operator()(
      ossia::net::minuit_protocol& proto, ossia::net::generic_device& dev,
      const oscpack::ReceivedMessage& mess)
  {
  }
};

template <>
struct minuit_behavior<
    minuit_command::Error,
    minuit_operation::Get>
{
  void operator()(
      ossia::net::minuit_protocol& proto, ossia::net::generic_device& dev,
      const oscpack::ReceivedMessage& mess)
  {
    // Do nothing
    if(mess.ArgumentCount() > 0)
    {
      auto beg = mess.ArgumentsBegin();
      if(beg->IsString())
      {
        proto.get_refreshed(beg->AsStringUnchecked());
      }
    }
  }
};

// Get
template<>
struct minuit_behavior<
    minuit_command::Request,
    minuit_operation::Get>
{
  auto operator()(
      ossia::net::minuit_protocol& proto,
      ossia::net::generic_device& dev,
      const oscpack::ReceivedMessage& mess)
  {
    boost::string_view full_address{mess.ArgumentsBegin()->AsString()};
    auto idx = full_address.find_first_of(":");

    if(idx == std::string::npos)
    {
      // Value
      auto node = ossia::net::find_node(dev, full_address);
      if (!node)
        return;
      auto addr = node->getAddress();
      if (!addr)
        return;

      proto.sender().send(proto.name_table.get_action(minuit_action::GetReply),
                        full_address, addr->cloneValue());
    }
    else
    {
      boost::string_view address{full_address.data(), idx};

      // Note : bug if address == "foo:"
      auto attr = get_attribute(
            boost::string_view(
              address.data() + idx + 1,
              full_address.size() - idx - 1));

      auto node = ossia::net::find_node(dev, address);
      if (!node)
        return;
      auto addr = node->getAddress();
      if (!addr)
        return;

      switch(attr)
      {
        case minuit_attribute::Value:
          proto.sender().send(proto.name_table.get_action(minuit_action::GetReply),
                              full_address,
                              addr->cloneValue());
          break;
        case minuit_attribute::Type:
          proto.sender().send(proto.name_table.get_action(minuit_action::GetReply),
                              full_address,
                              to_minuit_type_text(addr->getValueType()));
          break;
        case minuit_attribute::RangeBounds:
          proto.sender().send(proto.name_table.get_action(minuit_action::GetReply),
                              full_address,
                              addr->getDomain());
          break;
        case minuit_attribute::RangeClipMode:
          proto.sender().send(proto.name_table.get_action(minuit_action::GetReply),
                              full_address,
                              to_minuit_bounding_text(addr->getBoundingMode()));
          break;
        case minuit_attribute::Dataspace:
          proto.sender().send(proto.name_table.get_action(minuit_action::GetReply),
                              full_address,
                              ossia::get_dataspace_text(addr->getUnit()));
          break;
        case minuit_attribute::DataspaceUnit:
          proto.sender().send(proto.name_table.get_action(minuit_action::GetReply),
                              full_address,
                              ossia::get_unit_text(addr->getUnit()));
          break;
        case minuit_attribute::RepetitionFilter:
          proto.sender().send(proto.name_table.get_action(minuit_action::GetReply),
                              full_address,
                              (int32_t)addr->getRepetitionFilter());
          break;
        case minuit_attribute::Service:
          proto.sender().send(proto.name_table.get_action(minuit_action::GetReply),
                              full_address,
                              to_minuit_service_text(addr->getAccessMode()));
          break;
        case minuit_attribute::Description:
          proto.sender().send(proto.name_table.get_action(minuit_action::GetReply),
                              full_address,
                              addr->getDescription());
          break;
        case minuit_attribute::Priority:
        default:
          break;
      }
    }
  }
};

// Listen
template<>
struct minuit_behavior<
    minuit_command::Request,
    minuit_operation::Listen>
{
  auto operator()(
      ossia::net::minuit_protocol& proto,
      ossia::net::generic_device& dev,
      const oscpack::ReceivedMessage& mess)
  {
    // TODO FIXME Add the address to the listeners
  }
};

// Namespace
template<>
struct minuit_behavior<
    minuit_command::Request,
    minuit_operation::Namespace>
{
  void handle_root(
      ossia::net::minuit_protocol& proto,
      const std::vector<std::string>& c)
  {
      proto.sender().send(proto.name_table.get_action(minuit_action::NamespaceReply),
                      "/",
                      "Application",
                      "nodes={",
                      c,
                      "}",
                      "attributes={",
                      "}");
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  void handle_container(
      ossia::net::minuit_protocol& proto,
      boost::string_view address,
      const std::vector<std::string>& c)
  {
    proto.sender().send(proto.name_table.get_action(minuit_action::NamespaceReply),
                    address,
                    "Container",
                    "nodes={",
                    c,
                    "}",
                    "attributes={",
                    "}");
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  void handle_data(
      ossia::net::minuit_protocol& proto,
      boost::string_view address)
  {
    proto.sender().send(proto.name_table.get_action(minuit_action::NamespaceReply),
                    address,
                    "Data",
                    "attributes={",
                    "type"             ,
                    "dataspace"        ,
                    "repetitionsFilter",
                    "service"          ,
                    "priority"         ,
                    "value"            ,
                    "rangeBounds"      ,
                    "rangeClipmode"    ,
                    "dataspaceUnit"    ,
                    "}");
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  void handle_data_container(
      ossia::net::minuit_protocol& proto,
      boost::string_view address,
      const std::vector<std::string>& c)
  {
    proto.sender().send(proto.name_table.get_action(minuit_action::NamespaceReply),
                    address,
                    "Data",
                    "attributes={",
                    "type"             ,
                    "dataspace"        ,
                    "repetitionsFilter",
                    "service"          ,
                    "priority"         ,
                    "value"            ,
                    "rangeBounds"      ,
                    "rangeClipmode"    ,
                    "dataspaceUnit"    ,
                    "}");
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  auto get_children_names(const ossia::net::node_base& node)
  {
    std::vector<std::string> v;
    v.reserve(node.children().size());

    for(const auto& child : node.children())
      v.push_back(child->getName());

    return v;
  }

  std::vector<std::string> get_children_names(
      ossia::net::generic_device& dev,
      boost::string_view address)
  {
    auto node = ossia::net::find_node(dev, address);
    if (!node)
      return {};

    return get_children_names(*node);
  }

  auto operator()(
      ossia::net::minuit_protocol& proto,
      ossia::net::generic_device& dev,
      const oscpack::ReceivedMessage& mess)
  {
    boost::string_view address{mess.ArgumentsBegin()->AsString()};

    if(address == boost::string_view("/"))
    {
      handle_root(proto, get_children_names(dev, address));
    }
    else
    {
      auto node = ossia::net::find_node(dev, address);
      if (!node)
        return;

      if(node->children().empty())
      {
        handle_data(proto, address);
      }
      else
      {
        if(node->getAddress())
        {
          handle_data_container(proto, address, get_children_names(dev, address));
        }
        else
        {
          handle_container(proto, address, get_children_names(dev, address));
        }
      }
    }
  }
};


inline ossia::net::domain get_domain(
    ossia::net::address_base& addr,
    oscpack::ReceivedMessageArgumentIterator beg_it,
    oscpack::ReceivedMessageArgumentIterator end_it)
{
  boost::container::small_vector<ossia::value, 2> val;
  const auto cur = addr.cloneValue();

  // We read all the values one by one
  while (beg_it != end_it)
  {
    auto cur_it = beg_it;
    ++beg_it;
    val.push_back(ossia::net::to_numeric_value(cur, cur_it, beg_it));
  }

  return ossia::net::make_domain(val, cur);
}

// Listen
template <>
struct minuit_behavior<
    minuit_command::Answer,
    minuit_operation::Listen>
{
  auto operator()(
      ossia::net::minuit_protocol& proto,
      ossia::net::generic_device& dev,
      const oscpack::ReceivedMessage& mess)
  {
    boost::string_view full_address;
    auto mess_it = mess.ArgumentsBegin();
    full_address = mess_it->AsStringUnchecked();
    auto idx = full_address.find_first_of(":");

    if (idx == std::string::npos)
    {
      // The OSC message is a standard OSC one, carrying a value.
      auto node = ossia::net::find_node(dev, full_address);
      if (node)
      {
        if (auto addr = node->getAddress())
        {
          ossia::net::update_value(
                *addr, ++mess_it, mess.ArgumentsEnd(), mess.ArgumentCount() - 1);
        }
      }
    }
    else
    {
      // The OSC message is a Minuit one.
      // address contains the "sanitized" OSC-like address.
      boost::string_view address{full_address.data(), idx};

      // Note : bug if address == "foo:"
      auto attr = get_attribute(
            boost::string_view(
              address.data() + idx + 1, full_address.size() - idx - 1));

      ++mess_it;
      // mess_it is now at the first argument after the address:attribute

      if (auto node = ossia::net::find_node(dev, address))
      if (auto addr = node->getAddress())
      switch (attr)
      {
        case minuit_attribute::Value:
        {
          ossia::net::update_value(
                *addr, mess_it, mess.ArgumentsEnd(), mess.ArgumentCount() - 1);
          break;
        }
        case minuit_attribute::Type:
        {
          addr->setValueType(
                ossia::minuit::type_from_minuit_type_text(mess_it->AsStringUnchecked()));

          break;
        }
        case minuit_attribute::RangeBounds:
        {
          addr->setDomain(
                get_domain(*addr, mess_it, mess.ArgumentsEnd()));
          break;
        }
        case minuit_attribute::RangeClipMode:
        {
          addr->setBoundingMode(
                from_minuit_bounding_text(mess_it->AsStringUnchecked()));
          break;
        }
        case minuit_attribute::RepetitionFilter:
        {
          addr->setRepetitionFilter(
                static_cast<repetition_filter>(mess_it->AsInt32Unchecked()));
          break;
        }
        case minuit_attribute::Dataspace:
        {
          addr->setUnit(ossia::parse_dataspace(mess_it->AsStringUnchecked()));
          break;
        }
        case minuit_attribute::DataspaceUnit:
        {
          addr->setUnit(ossia::parse_unit(mess_it->AsStringUnchecked(), addr->getUnit()));
          break;
        }
        case minuit_attribute::Service:
        {
          addr->setAccessMode(from_minuit_service_text(mess_it->AsStringUnchecked()));
          break;
        }
        case minuit_attribute::Description:
        {
          addr->setDescription(mess_it->AsStringUnchecked());
          break;
        }
        default:
          break;
      }
    }
    return full_address;
  }
};

// Get
template <>
struct minuit_behavior<minuit_command::Answer, minuit_operation::Get>
{

  void operator()(
      ossia::net::minuit_protocol& proto, ossia::net::generic_device& dev,
      const oscpack::ReceivedMessage& mess)
  {
    auto addr = minuit_behavior<minuit_command::Answer, minuit_operation::Listen>{}(proto, dev, mess);
    proto.get_refreshed(addr);
  }
};

template <typename It, typename Fun>
auto find_if(It begin, It end, Fun f)
{
  for (auto it = begin; it != end; ++it)
  {
    if (f(*it))
      return it;
  }
  return end;
}

// Namespace
template <>
struct minuit_behavior<minuit_command::Answer,
                               minuit_operation::Namespace>
{
  template <typename Str>
  static auto get_container(
      Str s, oscpack::ReceivedMessageArgumentIterator beg_it,
      oscpack::ReceivedMessageArgumentIterator end_it)
  {
    std::vector<boost::string_view> elements;
    auto nodes_beg_it = find_if(beg_it, end_it, [=](const auto& mess) {
      return mess.IsString()
             && boost::string_view(mess.AsStringUnchecked()) == s;
    });

    ++nodes_beg_it; // It will point on the first past "nodes={".
    if (nodes_beg_it != end_it)
    {
      auto nodes_end_it = find_if(nodes_beg_it, end_it, [](const auto& mess)
      {
        return
            mess.IsString() &&
            mess.AsStringUnchecked()[0] == '}' &&
            mess.AsStringUnchecked()[1] == '\0';
      });

      if (nodes_end_it != end_it)
      {
        for (auto it = nodes_beg_it; it != nodes_end_it; ++it)
        {
          elements.push_back(it->AsStringUnchecked());
        }
      }
    }

    return elements;
  }

  static auto get_nodes(
      oscpack::ReceivedMessageArgumentIterator beg_it,
      oscpack::ReceivedMessageArgumentIterator end_it)
  {
    return get_container("nodes={", beg_it, end_it);
  }

  static auto get_attributes(
      oscpack::ReceivedMessageArgumentIterator beg_it,
      oscpack::ReceivedMessageArgumentIterator end_it)
  {
    return get_container("attributes={", beg_it, end_it);
  }

  static auto handle_container(
      ossia::net::minuit_protocol& proto, ossia::net::generic_device& dev,
      boost::string_view address,
      oscpack::ReceivedMessageArgumentIterator beg_it,
      oscpack::ReceivedMessageArgumentIterator end_it)
  {
    using namespace oscpack;
    auto sub_request
        = proto.name_table.get_action(minuit_action::NamespaceRequest);

    // Get the sub-nodes
    std::string child_address = address.to_string();
    if (child_address.back() != '/')
      child_address += '/';
    const auto child_address_size = child_address.size();

    for (auto child : get_nodes(beg_it, end_it))
    {
      child_address.resize(child_address_size);

      // Address of the node to create
      child_address.append(child.begin(), child.end());

      // Create the actual node
      ossia::net::find_or_create_node(dev, address);

      // request children
      proto.namespace_refresh(sub_request, child_address);
    }
  }

  static auto handle_data(
      ossia::net::minuit_protocol& proto, ossia::net::generic_device& dev,
      boost::string_view address,
      oscpack::ReceivedMessageArgumentIterator beg_it,
      oscpack::ReceivedMessageArgumentIterator end_it)
  {
    using namespace oscpack;

    // Find or create the node
    auto& n = ossia::net::find_or_create_node(dev, address);
    n.createAddress(ossia::val_type::IMPULSE);

    // A data can also have child nodes :
    handle_container(proto, dev, address, beg_it, end_it);

    auto sub_request = proto.name_table.get_action(minuit_action::GetRequest);

    // Request all the attributes provided by the node
    // type and dataspace have to be queried before the others since
    // value and dataspace_unit correct parsing depends on correct values prior.
    // note : we should really use TCP for this since there could be a reordering...
    // or a queue of things to query.

    auto attribs = get_attributes(beg_it, end_it);
    for(auto it = attribs.begin(); it != attribs.end(); )
    {
      switch(get_attribute(*it))
      {
        case minuit_attribute::Type:
        case minuit_attribute::Dataspace:
        {
          // name?get address:attribute
          auto str = address.to_string();
          str += ':';
          str.append(it->begin(), it->end());
          proto.get_refresh(sub_request, str);

          it = attribs.erase(it);
        }
          break;
        default:
          ++it;
          break;
      }
    }

    for (auto attrib : attribs)
    {
      switch(get_attribute(attrib))
      {
        case minuit_attribute::Value:
        case minuit_attribute::RangeBounds:
        case minuit_attribute::RangeClipMode:
        case minuit_attribute::RepetitionFilter:
        case minuit_attribute::DataspaceUnit:
        case minuit_attribute::Service:
        case minuit_attribute::Description:
        {
          // name?get address:attribute
          auto str = address.to_string();
          str += ':';
          str.append(attrib.begin(), attrib.end());
          proto.get_refresh(sub_request, str);
        }
      default:
        break;
      }
    }
  }

  static auto handle_minuit(
      ossia::net::minuit_protocol& proto, ossia::net::generic_device& dev,
      boost::string_view address, minuit_type type,
      oscpack::ReceivedMessageArgumentIterator beg_it,
      oscpack::ReceivedMessageArgumentIterator end_it)
  {
    switch (type)
    {
      case minuit_type::Application:
      case minuit_type::Container:
      {
        handle_container(proto, dev, address, beg_it, end_it);
        break;
      }
      case minuit_type::Data:
      {
        handle_data(proto, dev, address, beg_it, end_it);
        break;
      }
      case minuit_type::ModelInfo:
      case minuit_type::UiInfo:
      case minuit_type::PresetManager:
      case minuit_type::None:
        handle_container(proto, dev, address, beg_it, end_it);
        break;
    }
  }

  auto operator()(
      ossia::net::minuit_protocol& proto, ossia::net::generic_device& dev,
      const oscpack::ReceivedMessage& mess)
  {
    auto it = mess.ArgumentsBegin();
    boost::string_view address = it->AsString();
    auto type = get_type((++it)->AsString()[0]);

    handle_minuit(proto, dev, address, type, it, mess.ArgumentsEnd());

    proto.namespace_refreshed(address);
  }
};

// Namespace request :
// app?namespace addr

// Namespace answer :
// app:namespace addr minuit_type nodes={ n1 n2 n3 } attributes={ foo bar baz }

class minuit_message_handler
{
public:
  static void handleMinuitMessage(
      ossia::net::minuit_protocol& proto, ossia::net::generic_device& dev,
      boost::string_view address, const oscpack::ReceivedMessage& m)
  {
    // Look for either ':' or '?'
    auto idx = address.find_first_of(":?!");

    if (idx != std::string::npos)
    {
      auto req = get_command(address[idx]);
      auto op = get_operation(*(address.data() + idx + 1));
      switch (req)
      {
        case minuit_command::Answer: // Receiving an answer
        {
          switch (op)
          {
            case minuit_operation::Listen:
              minuit_behavior<minuit_command::Answer,
                                      minuit_operation::Listen>{}(
                  proto, dev, m);
              break;
            case minuit_operation::Get:
              minuit_behavior<minuit_command::Answer,
                                      minuit_operation::Get>{}(proto, dev, m);
              break;
            case minuit_operation::Namespace:
              minuit_behavior<minuit_command::Answer,
                                      minuit_operation::Namespace>{}(
                  proto, dev, m);
              break;
            default:
              break;
          }
          break;
        }
        case minuit_command::Request: // Receiving a request
        {
          switch (op)
          {
            case minuit_operation::Listen:
              minuit_behavior<minuit_command::Request,
                                      minuit_operation::Listen>{}(
                  proto, dev, m);
              break;
            case minuit_operation::Get:
              minuit_behavior<minuit_command::Request,
                                      minuit_operation::Get>{}(proto, dev, m);
              break;
            case minuit_operation::Namespace:
              minuit_behavior<minuit_command::Request,
                                      minuit_operation::Namespace>{}(
                  proto, dev, m);
              break;
            default:
              break;
          }
          break;
        }
        case minuit_command::Error: // Receiving an error
        {
          switch (op)
          {
            case minuit_operation::Listen:
              minuit_behavior<minuit_command::Error,
                                      minuit_operation::Listen>{}(
                  proto, dev, m);
              break;
            case minuit_operation::Get:
              minuit_behavior<minuit_command::Error,
                                      minuit_operation::Get>{}(proto, dev, m);
              break;
            case minuit_operation::Namespace:
              minuit_behavior<minuit_command::Error,
                                      minuit_operation::Namespace>{}(
                  proto, dev, m);
              break;
            default:
              break;
          }
          break;
        }
        default:
          break;
      }
    }
  }
};
}
}
