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
    // Do nothing
    if(proto.pending_get_requests > 0)
    {
      proto.pending_get_requests--;
      try {
      proto.get_promise.set_value();
      }
      catch ( ... )  { }
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
    boost::string_ref full_address{mess.ArgumentsBegin()->AsString()};
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
      boost::string_ref address{full_address.data(), idx};

      // Note : bug if address == "foo:"
      auto attr = get_attribute(
            boost::string_ref(
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
                          full_address, addr->cloneValue());
          break;
        case minuit_attribute::Type:
          proto.sender().send(proto.name_table.get_action(minuit_action::GetReply),
                          full_address,
                          to_minuit_type_text(addr->getValueType())
                          );
          break;
        case minuit_attribute::RangeBounds:
          proto.sender().send(proto.name_table.get_action(minuit_action::GetReply),
                          full_address, addr->getDomain());
          break;
        case minuit_attribute::RangeClipMode:
          proto.sender().send(proto.name_table.get_action(minuit_action::GetReply),
                          full_address,
                          to_minuit_bounding_text(addr->getBoundingMode())
                          );
          break;
        case minuit_attribute::RepetitionFilter:
          proto.sender().send(proto.name_table.get_action(minuit_action::GetReply),
                          full_address, (int32_t)addr->getRepetitionFilter());
          break;
        case minuit_attribute::Service:
          proto.sender().send(proto.name_table.get_action(minuit_action::GetReply),
                          full_address, to_minuit_service_text(addr->getAccessMode()));
          break;
        case minuit_attribute::Priority:
        case minuit_attribute::Description:
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
    // Add the address to the listeners
  }
};

// Namespace
template<>
struct minuit_behavior<
    minuit_command::Request,
    minuit_operation::Namespace>
{
  template<typename Children>
  void handle_root(
      ossia::net::minuit_protocol& proto,
      Children&& c)
  {
    proto.sender().send(proto.name_table.get_action(minuit_action::NamespaceReply),
                    "/",
                    "Application",
                    "nodes={",
                    c,
                    "}",
                    "attributes={",
                    "}");

  }

  template<typename Children>
  void handle_container(
      ossia::net::minuit_protocol& proto,
      boost::string_ref address,
      Children&& c)
  {
    proto.sender().send(proto.name_table.get_action(minuit_action::NamespaceReply),
                    address,
                    "Container",
                    "nodes={",
                    c,
                    "}",
                    "attributes={",
                    "}");

  }

  void handle_data(
      ossia::net::minuit_protocol& proto,
      boost::string_ref address)
  {
    proto.sender().send(proto.name_table.get_action(minuit_action::NamespaceReply),
                    address,
                    "Data",
                    "attributes={",
                    "type"             ,
                    "repetitionsFilter",
                    "service"          ,
                    "priority"         ,
                    "value"            ,
                    "rangeBounds"      ,
                    "rangeClipmode"    ,
                    "}");

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
      boost::string_ref address)
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
    boost::string_ref address{mess.ArgumentsBegin()->AsString()};

    if(address == boost::string_ref("/"))
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
        handle_container(proto, address, get_children_names(dev, address));
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
  auto cur = addr.cloneValue();

  // We read all the values one by one
  while (beg_it != end_it)
  {
    auto cur_it = beg_it;
    ++beg_it;
    val.push_back(ossia::net::to_value(cur, cur_it, beg_it, 1));
  }

  if (val.size() == 2)
  {
    if(cur.getType() != val_type::TUPLE)
      return ossia::net::make_domain(val[0], val[1]);
    else
    {
      if(val[0].valid() && val[1].valid())
        return ossia::net::domain_base<Tuple>{val[0], val[1]};
      else
        return {};
    }
  }
  return {};
}

// Get
template <>
struct minuit_behavior<minuit_command::Answer, minuit_operation::Get>
{

  void operator()(
      ossia::net::minuit_protocol& proto, ossia::net::generic_device& dev,
      const oscpack::ReceivedMessage& mess)
  {
    auto mess_it = mess.ArgumentsBegin();
    boost::string_ref full_address{mess_it->AsString()};
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
      boost::string_ref address{full_address.data(), idx};

      // Note : bug if address == "foo:"
      auto attr = get_attribute(
            boost::string_ref(
              address.data() + idx + 1, full_address.size() - idx - 1));

      ++mess_it;
      // mess_it is now at the first argument after the address:attribute

      auto node = ossia::net::find_node(dev, address);
      if (!node)
        return;
      auto addr = node->getAddress();
      if (!addr)
        return;

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
                ossia::minuit::type_from_minuit_type_text(mess_it->AsString()));

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
                from_minuit_bounding_text(mess_it->AsString()));
          break;
        }
        case minuit_attribute::RepetitionFilter:
        {
          addr->setRepetitionFilter(
                static_cast<repetition_filter>(mess_it->AsInt32()));
          break;
        }
        case minuit_attribute::Service:
        {
          addr->setAccessMode(from_minuit_service_text(mess_it->AsString()));
          break;
        }
        default:
          break;
      }
    }

    proto.pending_get_requests--;
    try {
    proto.get_promise.set_value();
    } catch(...) { }
  }
};

// Listen
template <>
struct minuit_behavior<minuit_command::Answer,
                               minuit_operation::Listen>
{
  auto operator()(
      ossia::net::minuit_protocol& proto, ossia::net::generic_device& dev,
      const oscpack::ReceivedMessage& mess)
  {
    // TODO refactor with get answer
    auto mess_it = mess.ArgumentsBegin();
    boost::string_ref full_address{mess_it->AsString()};
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
      boost::string_ref address{full_address.data(), idx};

      // Note : bug if address == "foo:"
      auto attr = get_attribute(
            boost::string_ref(
              address.data() + idx + 1, full_address.size() - idx - 1));

      ++mess_it;
      // mess_it is now at the first argument after the address:attribute

      auto node = ossia::net::find_node(dev, address);
      if (!node)
        return;
      auto addr = node->getAddress();
      if (!addr)
        return;

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
                ossia::minuit::type_from_minuit_type_text(mess_it->AsString()));

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
                from_minuit_bounding_text(mess_it->AsString()));
          break;
        }
        case minuit_attribute::RepetitionFilter:
        {
          addr->setRepetitionFilter(
                static_cast<repetition_filter>(mess_it->AsInt32()));
          break;
        }
        case minuit_attribute::Service:
        {
          addr->setAccessMode(from_minuit_service_text(mess_it->AsString()));
          break;
        }
        default:
          break;
      }
    }
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
    std::vector<boost::string_ref> elements;
    auto nodes_beg_it = find_if(beg_it, end_it, [=](const auto& mess) {
      return mess.IsString()
             && boost::string_ref(mess.AsStringUnchecked()) == s;
    });

    ++nodes_beg_it; // It will point on the first past "nodes={".
    if (nodes_beg_it == end_it)
      return elements;

    auto nodes_end_it = find_if(nodes_beg_it, end_it, [](const auto& mess) {
      return mess.IsString()
             && boost::string_ref(mess.AsStringUnchecked()) == "}";
    });

    if (nodes_end_it == end_it)
      return elements;

    for (auto it = nodes_beg_it; it != nodes_end_it; ++it)
    {
      elements.push_back(it->AsString());
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
      boost::string_ref address,
      oscpack::ReceivedMessageArgumentIterator beg_it,
      oscpack::ReceivedMessageArgumentIterator end_it)
  {
    using namespace oscpack;
    auto sub_request
        = proto.name_table.get_action(minuit_action::NamespaceRequest);

    // Get the sub-nodes
    for (auto child : get_nodes(beg_it, end_it))
    {
      // Address of the node to create
      std::string child_address = address.to_string();
      if (address.back() != '/')
        child_address += '/';
      child_address.append(child.begin(), child.end());

      // Create the actual node
      ossia::net::find_or_create_node(dev, address);

      // request children
      proto.refresh(sub_request, child_address);
    }
  }

  static auto handle_data(
      ossia::net::minuit_protocol& proto, ossia::net::generic_device& dev,
      boost::string_ref address,
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
    auto attribs = get_attributes(beg_it, end_it);
    for(auto it = attribs.begin(); it != attribs.end(); )
    {
      if(*it == "type")
      {
        auto str = address.to_string() + ":type";
        proto.pending_get_requests++;
        proto.sender().send(sub_request, boost::string_ref(str));

        it = attribs.erase(it);
      }
      else
      {
        ++it;
      }
    }
    for (auto attrib : get_attributes(beg_it, end_it))
    {
      auto attr = get_attribute(attrib);
      switch(attr)
      {
        case minuit_attribute::Value:
        case minuit_attribute::Type:
        case minuit_attribute::RangeBounds:
        case minuit_attribute::RangeClipMode:
        case minuit_attribute::RepetitionFilter:
        case minuit_attribute::Service:
        {
          // name?get address:attribute
          auto str = address.to_string();
          str += ':';
          str.append(attrib.begin(), attrib.end());
          proto.pending_get_requests++;
          proto.sender().send(sub_request, boost::string_ref(str));
        }
      default:
        break;
      }
    }
  }

  static auto handle_minuit(
      ossia::net::minuit_protocol& proto, ossia::net::generic_device& dev,
      boost::string_ref address, minuit_type type,
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
    boost::string_ref address = it->AsString();
    auto type = get_type((++it)->AsString()[0]);

    handle_minuit(proto, dev, address, type, it, mess.ArgumentsEnd());

    proto.refreshed(address);
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
      boost::string_ref address, const oscpack::ReceivedMessage& m)
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
