#pragma once
#include <ossia/network/v2/minuit/detail/minuit_common.hpp>
#include <oscpack/osc/OscReceivedElements.h>
#include <ossia/network/v2/osc/detail/osc.hpp>
#include <ossia/network/v2/generic/generic_address.hpp>
#include <ossia/network/v2/generic/generic_device.hpp>
#include <ossia/network/v2/generic/generic_node.hpp>
#include <ossia/network/v2/minuit/minuit.hpp>
namespace OSSIA
{
namespace minuit
{


template<minuit_command Req, minuit_operation Op>
struct minuit_remote_behaviour
{
        auto operator()(
                impl::Minuit2& proto,
                impl::BasicDevice& dev,
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
        OSSIA::v2::Domain get_domain(
                oscpack::ReceivedMessageArgumentIterator beg_it,
                oscpack::ReceivedMessageArgumentIterator end_it)
        {
            /*
      Values v;
      values_reader<
          lax_error_handler,
          conversion_mode::ConvertAndReplace>{}(
      beg_it,
          end_it,
          v);*/

            OSSIA::v2::Domain r;
            /*
      if(v.variants.size() == 2)
      {
        r.min = v.variants[0];
        r.max = v.variants[1];
      }
      */

            return r;
        }

        void operator()(
                impl::Minuit2& proto,
                impl::BasicDevice& dev,
                const oscpack::ReceivedMessage& mess)
        {
            auto mess_it = mess.ArgumentsBegin();
            boost::string_ref full_address{mess_it->AsString()};
            auto idx = full_address.find_first_of(":");


            if(idx == std::string::npos)
            {
                // The OSC message is a standard OSC one, carrying a value.
                auto node = impl::find_node(dev, full_address);
                if(node)
                {
                    if(auto addr = node->getAddress())
                    {
                        impl::updateValue(*addr, ++mess_it,
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

                auto node = impl::find_node(dev, address);
                if(!node)
                    return;
                auto addr = node->getAddress();
                if(!addr)
                    return;

                switch(attr)
                {
                    case minuit_attribute::Value:
                    {
                        impl::updateValue(*addr, mess_it,
                                          mess.ArgumentsEnd(),
                                          mess.ArgumentCount() - 1);
                        break;
                    }
                    case minuit_attribute::Type:
                        break;
                    case minuit_attribute::RangeBounds:
                        break;
                    case minuit_attribute::RangeClipMode:
                        break;
                    case minuit_attribute::RepetitionFilter:
                        break;
                    case minuit_attribute::Service:
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
struct minuit_remote_behaviour<
        minuit_command::Answer,
        minuit_operation::Listen>
{
        auto operator()(
                impl::Minuit2& proto,
                impl::BasicDevice& dev,
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
                impl::Minuit2& proto,
                impl::BasicDevice& dev,
                boost::string_ref address,
                oscpack::ReceivedMessageArgumentIterator beg_it,
                oscpack::ReceivedMessageArgumentIterator end_it)
        {
            using namespace oscpack;
            auto sub_request = proto.mNameTable.get_action(minuit_action::NamespaceRequest);

            // Get the sub-nodes
            for(auto child : get_nodes(beg_it, end_it))
            {
                // Address of the node to create
                std::string child_address = address.to_string();
                if(address.back() != '/')
                    child_address += '/';
                child_address.append(child.begin(), child.end());
                std::cerr << "new_node_address: " << child_address << "\n";

                // Create the actual node
                impl::find_or_create_node(dev, address);

                // request children
                proto.refresh(sub_request, child_address);
            }
        }

        static auto handle_data(
                impl::Minuit2& proto,
                impl::BasicDevice& dev,
                boost::string_ref address,
                oscpack::ReceivedMessageArgumentIterator beg_it,
                oscpack::ReceivedMessageArgumentIterator end_it)
        {
            using namespace oscpack;

            // First find the node
            if(impl::find_node(dev, address))
            {
                auto sub_request = proto.mNameTable.get_action(minuit_action::GetRequest);

                // Request all the attributes provided by the node
                for(auto attrib : get_attributes(beg_it, end_it))
                {
                    // name?get address:attribute
                    auto str = address.to_string();
                    str += ':';
                    str.append(attrib.begin(), attrib.end());
                    proto.mSender.send(sub_request, boost::string_ref(str));
                }
            }
        }

        static auto handle_minuit(
                impl::Minuit2& proto,
                impl::BasicDevice& dev,
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
                case minuit_type::None:
                    // just add the node ?
                    break;
            }

        }

        auto operator()(
                impl::Minuit2& proto,
                impl::BasicDevice& dev,
                const oscpack::ReceivedMessage& mess)
        {
            auto it = mess.ArgumentsBegin();
            boost::string_ref address = it->AsString();
            auto type = get_type((++it)->AsString()[0]);

            handle_minuit(proto, dev, address, type, it, mess.ArgumentsEnd());

            proto.refreshed(address);
        }
};

/*
struct get_promise
{
        get_promise(const std::string& addr):
            address{addr} { }
        std::string address;
        std::promise<coppa::minuit::Parameter> promise;
};


template<
        template<
          minuit_command,
          minuit_operation>
        class Handler>
struct minuit_callback_behaviour_wrapper<Handler, minuit_command::Answer, minuit_operation::Get>
{
    auto operator()(impl::BasicDevice& dev, const oscpack::ReceivedMessage& mess)
    {
        auto res_it = Handler<minuit_command::Answer, minuit_operation::Get>{}(dev, map, mess);

        if(res_it != map.end())
        {
            // TODO for all if instead ?
            auto prom_it = std::find_if(
                        dev.m_getPromises.begin(),
                        dev.m_getPromises.end(),
                        [=] (const auto& prom) {
               return prom.address == res_it->destination;
            });

            if(prom_it != dev.m_getPromises.end())
            {
              prom_it->promise.set_value(*res_it);
              dev.m_getPromises.erase(prom_it);
            }
        }
        // TODO here : - boost.lockfree queue for namespaces : we add the requested paths to a queue
        // when they are received we remove them
        // when the queue is empty, the namespace querying is finished (else we retry three times or something)

        // For "get" we set our promise as soon as we're done ? where shall we store them ?

        //dev.m_getPromises
    }
};

template<
  minuit_command c,
  minuit_operation op>
using minuit_callback_behaviour_wrapper_t = minuit_callback_behaviour_wrapper<minuit_remote_behaviour, c, op>;
template<
        template<
          minuit_command,
          minuit_operation>
        class Handler>
struct minuit_callback_behaviour_wrapper<Handler, minuit_command::Answer, minuit_operation::Listen>
{
    auto operator()(impl::BasicDevice& dev, const oscpack::ReceivedMessage& mess)
    {
      Handler<minuit_command::Answer, minuit_operation::Listen>{}(dev, map, mess);
    }
};
*/


// Namespace request :
// app?namespace addr

// Namespace answer :
// app:namespace addr minuit_type nodes={ n1 n2 n3 } attributes={ foo bar baz }

class minuit_message_handler
{
    public:
        static void handleMinuitMessage(
                impl::Minuit2& proto,
                impl::BasicDevice& dev,
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
