#pragma once

#include <ossia/network/generic/generic_parameter.hpp>
#include <ossia-qt/http/http_parameter_data.hpp>

namespace ossia
{
namespace net
{

class OSSIA_EXPORT http_parameter final
    : public ossia::net::generic_parameter
{
private:
  http_parameter_data_base mData;

public:
  http_parameter(
      const http_parameter_data& data, ossia::net::node_base& node_base);

  http_parameter() = delete;
  http_parameter(const http_parameter& other) = delete;
  http_parameter(http_parameter&& other) = delete;
  http_parameter& operator=(const http_parameter& other) = delete;
  http_parameter& operator=(http_parameter&& other) = delete;
  ~http_parameter();

  const http_parameter_data_base& data() const
  {
    return mData;
  }

private:
};
}
}
