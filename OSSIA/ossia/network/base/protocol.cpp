// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/network/base/protocol.hpp>
#include <ossia/network/common/network_logger.hpp>
namespace ossia
{
namespace net
{
protocol_base::~protocol_base() = default;

network_logger::network_logger() = default;
network_logger::~network_logger() = default;

void protocol_base::request(address_base&)
{
}

std::future<void> protocol_base::pull_async(address_base&)
{
  return {};
}
}
}
