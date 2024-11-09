#pragma once

#include <ossia/detail/json_fwd.hpp>
#include <ossia/detail/lockfree_queue.hpp>
#include <ossia/network/base/listening.hpp>
#include <ossia/network/base/protocol.hpp>
#include <ossia/network/oscquery/host_info.hpp>
#include <ossia/protocols/oscquery/oscquery_fwd.hpp>

#include <atomic>

namespace osc
{
template <typename T>
class sender;
class receiver;
}

namespace oscpack
{
class ReceivedMessage;
class IpEndpointName;
}

namespace ossia::net
{

class websocket_client;
struct parameter_data;
struct network_context;
using network_context_ptr = std::shared_ptr<network_context>;
}

namespace ossia::oscquery_asio
{
struct osc_outbound_visitor;
struct http_async_client_context;
struct http_responder;

template <typename State>
struct http_async_request;
template <typename State>
struct http_async_value_request;
template <typename State>
struct http_async_answer;
template <typename State>
struct http_async_value_answer;
}
