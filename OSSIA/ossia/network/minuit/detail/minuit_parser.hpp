#pragma once
#include <ossia/network/minuit/detail/minuit_common.hpp>
#include <oscpack/osc/OscReceivedElements.h>
#include <ossia/network/osc/detail/osc.hpp>
#include <ossia/network/generic/generic_address.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/generic/generic_node.hpp>
#include <ossia/network/minuit/minuit.hpp>
namespace ossia
{
namespace minuit
{


template<minuit_command Req, minuit_operation Op>
struct minuit_remote_behaviour
{
    auto operator()(
        ossia::net::minuit_protocol& proto,
        ossia::net::generic_device& dev,
        const oscpack::ReceivedMessage& mess)
    {
      // By default do nothing
    }
};

// Get
template<>
struct minuit_remote_behaviour<
    minuit_command::Answer,
    minuit_operation::Get>
{
    ossia::net::domain get_domain(
        ossia::net::address_base& addr,
        oscpack::ReceivedMessageArgumentIterator beg_it,
        oscpack::ReceivedMessageArgumentIterator end_it)
    {
      std::vector<ossia::value> val;
      auto cur = addr.cloneValue();

      // We read all the values one by one
      while(beg_it != end_it)
      {
        auto cur_it = beg_it;
        ++beg_it;
        val.push_back(ossia::net::toValue(cur, cur_it, beg_it, 1));
      }

      if(val.size() == 2)
        return ossia::net::makeDomain(val[0], val[1]);
      return {};
    }

    void operator()(
        ossia::net::minuit_protocol& proto,
        ossia::net::generic_device& dev,
        const oscpack::ReceivedMessage& mess)
    {
      auto mess_it = mess.ArgumentsBegin();
      boost::string_ref full_address{mess_it->AsString()};
      auto idx = full_address.find_first_of(":");

      if(idx == std::string::npos)
      {
        // The OSC message is a standard OSC one, carrying a value.
        auto node = ossia::net::find_node(dev, full_address);
        if(node)
        {
          if(auto addr = node->getAddress())
          {
            ossia::net::updateValue(*addr, ++mess_it,
                              mess.ArgumentsEnd(),
                              mess.ArgumentCount() - 1);
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
                        address.data() + idx + 1,
                        full_address.size() - idx - 1));

        ++mess_it;
        // mess_it is now at the first argument after the address:attribute

        auto node = ossia::net::find_node(dev, address);
        if(!node)
          return;
        auto addr = node->getAddress();
        if(!addr)
          return;

        switch(attr)
        {
          case minuit_attribute::Value:
          {
            ossia::net::updateValue(*addr, mess_it,
                              mess.ArgumentsEnd(),
                              mess.ArgumentCount() - 1);
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
                  this->get_domain(*addr, mess_it, mess.ArgumentsEnd()));
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
            addr->setAccessMode(
                  from_minuit_service_text(mess_it->AsString()));
            break;
          }
          default:
            break;
        }
      }

      proto.get_promise.set_value();
    }
};

// Listen
template<>
struct minuit_remote_behaviour<
    minuit_command::Answer,
    minuit_operation::Listen>
{
    auto operator()(
        ossia::net::minuit_protocol& proto,
        ossia::net::generic_device& dev,
        const oscpack::ReceivedMessage& mess)
    {

    }
};

template<typename It, typename Fun>
auto find_if(It begin, It end, Fun f)
{
  for(auto it = begin; it != end; ++it)
  {
    if(f(*it))
      return it;
  }
  return end;
}

// Namespace
template<>
struct minuit_remote_behaviour<
    minuit_command::Answer,
    minuit_operation::Namespace>
{
    template<typename Str>
    static auto get_container(
        Str s,
        oscpack::ReceivedMessageArgumentIterator beg_it,
        oscpack::ReceivedMessageArgumentIterator end_it)
    {
      std::vector<boost::string_ref> elements;
      auto nodes_beg_it = find_if(beg_it, end_it, [=] (const auto& mess) {
        return mess.IsString() && boost::string_ref(mess.AsStringUnchecked()) == s;
      });

      ++nodes_beg_it; // It will point on the first past "nodes={".
      if(nodes_beg_it == end_it)
        return elements;

      auto nodes_end_it = find_if(nodes_beg_it, end_it, [] (const auto& mess) {
        return mess.IsString() && boost::string_ref(mess.AsStringUnchecked()) == "}";
      });

      if(nodes_end_it == end_it)
        return elements;

      for(auto it = nodes_beg_it; it != nodes_end_it; ++it)
      {
        elements.push_back(it->AsString());
      }

      return elements;
    }

    static auto get_nodes(
        oscpack::ReceivedMessageArgumentIterator beg_it,
        oscpack::ReceivedMessageArgumentIterator end_it
        )
    {
      return get_container("nodes={", beg_it, end_it);
    }

    static auto get_attributes(
        oscpack::ReceivedMessageArgumentIterator beg_it,
        oscpack::ReceivedMessageArgumentIterator end_it
        )
    {
      return get_container("attributes={", beg_it, end_it);
    }

    static auto handle_container(
        ossia::net::minuit_protocol& proto,
        ossia::net::generic_device& dev,
        boost::string_ref address,
        oscpack::ReceivedMessageArgumentIterator beg_it,
        oscpack::ReceivedMessageArgumentIterator end_it)
    {
      using namespace oscpack;
      auto sub_request = proto.name_table.get_action(minuit_action::NamespaceRequest);

      // Get the sub-nodes
      for(auto child : get_nodes(beg_it, end_it))
      {
        // Address of the node to create
        std::string child_address = address.to_string();
        if(address.back() != '/')
          child_address += '/';
        child_address.append(child.begin(), child.end());

        // Create the actual node
        ossia::net::find_or_create_node(dev, address);

        // request children
        proto.refresh(sub_request, child_address);
      }
    }

    static auto handle_data(
        ossia::net::minuit_protocol& proto,
        ossia::net::generic_device& dev,
        boost::string_ref address,
        oscpack::ReceivedMessageArgumentIterator beg_it,
        oscpack::ReceivedMessageArgumentIterator end_it)
    {
      using namespace oscpack;

      // Find or create the node
      auto& n = ossia::net::find_or_create_node(dev, address);
      n.createAddress(ossia::val_type::IMPULSE);

      auto sub_request = proto.name_table.get_action(minuit_action::GetRequest);

      // Request all the attributes provided by the node
      for(auto attrib : get_attributes(beg_it, end_it))
      {
        // name?get address:attribute
        auto str = address.to_string();
        str += ':';
        str.append(attrib.begin(), attrib.end());
        proto.sender.send(sub_request, boost::string_ref(str));
      }

    }

    static auto handle_minuit(
        ossia::net::minuit_protocol& proto,
        ossia::net::generic_device& dev,
        boost::string_ref address,
        minuit_type type,
        oscpack::ReceivedMessageArgumentIterator beg_it,
        oscpack::ReceivedMessageArgumentIterator end_it)
    {
      switch(type)
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
          // just add the node ?
          break;
      }
    }

    auto operator()(
        ossia::net::minuit_protocol& proto,
        ossia::net::generic_device& dev,
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
        ossia::net::minuit_protocol& proto,
        ossia::net::generic_device& dev,
        boost::string_ref address,
        const oscpack::ReceivedMessage& m)
    {
      // Look for either ':' or '?'
      auto idx = address.find_first_of(":?!");

      if(idx != std::string::npos)
      {
        auto req = get_command(address[idx]);
        auto op = get_operation(*(address.data() + idx + 1));
        switch(req)
        {
          case minuit_command::Answer: // Receiving an answer
          {
            switch(op)
            {
              case minuit_operation::Listen:
                minuit_remote_behaviour<minuit_command::Answer, minuit_operation::Listen>{}(proto, dev, m);
                break;
              case minuit_operation::Get:
                minuit_remote_behaviour<minuit_command::Answer, minuit_operation::Get>{}(proto, dev, m);
                break;
              case minuit_operation::Namespace:
                minuit_remote_behaviour<
                    minuit_command::Answer,
                    minuit_operation::Namespace>{}(proto, dev, m);
                break;
              default:
                break;
            }
            break;
          }
          case minuit_command::Request: // Receiving a request
          {
            switch(op)
            {
              case minuit_operation::Listen:
                minuit_remote_behaviour<minuit_command::Request, minuit_operation::Listen>{}(proto, dev, m);
                break;
              case minuit_operation::Get:
                minuit_remote_behaviour<minuit_command::Request, minuit_operation::Get>{}(proto, dev, m);
                break;
              case minuit_operation::Namespace:
                minuit_remote_behaviour<minuit_command::Request, minuit_operation::Namespace>{}(proto, dev, m);
                break;
              default:
                break;
            }
            break;
          }
          case minuit_command::Error: // Receiving an error
          {
            switch(op)
            {
              case minuit_operation::Listen:
                minuit_remote_behaviour<minuit_command::Error, minuit_operation::Listen>{}(proto, dev, m);
                break;
              case minuit_operation::Get:
                minuit_remote_behaviour<minuit_command::Error, minuit_operation::Get>{}(proto, dev, m);
                break;
              case minuit_operation::Namespace:
                minuit_remote_behaviour<minuit_command::Error, minuit_operation::Namespace>{}(proto, dev, m);
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
