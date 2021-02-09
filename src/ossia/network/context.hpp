#pragma once
#include <asio/io_context.hpp>
#include <memory>
namespace ossia::net
{
struct network_context
{
  asio::io_context context;
};
using network_context_ptr = std::shared_ptr<network_context>;
}
