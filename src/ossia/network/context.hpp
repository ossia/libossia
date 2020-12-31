#pragma once
#include <asio/io_context.hpp>
namespace ossia::net
{
struct network_context
{
  asio::io_context context;
};
}
