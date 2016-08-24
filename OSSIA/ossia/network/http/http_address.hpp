#pragma once

#include <ossia/network/generic/generic_address.hpp>
#include <ossia/network/http/http_address_data.hpp>

namespace ossia
{
namespace net
{

class OSSIA_EXPORT http_address final :
    public ossia::net::generic_address
{
private:
  http_address_data_base mData;

public:
  http_address(
      const http_address_data& data,
      const ossia::net::node_base& node_base);

  http_address() = delete;
  http_address(const http_address& other) = delete;
  http_address(http_address&& other) = delete;
  http_address& operator=(const http_address& other) = delete;
  http_address& operator=(http_address&& other) = delete;
  ~http_address();

  const http_address_data_base& data() const
  { return mData; }

private:
};
}
}
