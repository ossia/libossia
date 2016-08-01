#pragma once

#include <ossia/detail/algorithms.hpp>
#include <ossia/network/base/protocol.hpp>
#include <vector>

namespace ossia
{
namespace net
{
class generic_device;
class OSSIA_EXPORT local_protocol final : public ossia::net::protocol_base
{
public:
  local_protocol();

  virtual ~local_protocol();

  bool pull(ossia::net::address_base&) override;
  bool push(const ossia::net::address_base& addr) override;
  bool observe(ossia::net::address_base&, bool) override;
  bool update(ossia::net::node_base& node_base) override;

  void setDevice(ossia::net::device_base& dev) override;

  void exposeTo(std::unique_ptr<ossia::net::protocol_base> p);

  void stopExposeTo(const ossia::net::protocol_base& p);

  const auto& getExposedProtocols() const
  {
    return mExposed;
  }

private:
  std::vector<std::unique_ptr<ossia::net::protocol_base>> mExposed;
  ossia::net::generic_device* mDevice{};
};
}
}
