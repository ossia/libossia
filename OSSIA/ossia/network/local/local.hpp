#pragma once

#include <ossia/detail/algorithms.hpp>
#include <ossia/network/base/protocol.hpp>
#include <vector>

namespace ossia
{
namespace net
{
class generic_device;
/**
 * \brief A protocol used to expose a local application's data through multiple other protocols.
 *
 * For instance use this when developing an artistic application with parameters
 * that you want to be able to control from another software.
 */
class OSSIA_EXPORT local_protocol final : public ossia::net::protocol_base
{
public:
  local_protocol();
  local_protocol(const local_protocol&) = delete;
  local_protocol(local_protocol&&) = delete;
  local_protocol& operator=(const local_protocol&) = delete;
  local_protocol& operator=(local_protocol&&) = delete;

  virtual ~local_protocol();

  bool pull(ossia::net::address_base&) override;
  bool push(const ossia::net::address_base& addr) override;
  bool observe(ossia::net::address_base&, bool) override;
  bool update(ossia::net::node_base& node_base) override;

  void setDevice(ossia::net::device_base& dev) override;
  
  //! Use this to add protocols through which you will expose the device. For instance OSC, Minuit, etc.
  void exposeTo(std::unique_ptr<ossia::net::protocol_base> p);
  
  //! Stop exposition to a protocol. It will be deleted.
  void stopExposeTo(const ossia::net::protocol_base& p);

  //! The protocols we are currently exposing this device through.
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
