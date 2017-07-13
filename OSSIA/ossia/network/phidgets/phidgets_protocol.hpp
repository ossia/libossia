#pragma once
#include <ossia/network/base/protocol.hpp>
#include <ossia/network/phidgets/detail/phidgetspp.hpp>
#include <readerwriterqueue.h>
namespace ossia
{
class phidget_device;
class OSSIA_EXPORT phidget_protocol : public ossia::net::protocol_base
{
  ppp::phidgets_manager m_mgr;
  phidget_device* m_dev{};
  std::function<void()> m_commandCb;
  moodycamel::ReaderWriterQueue<std::function<void()>> m_functionQueue;

public:
  phidget_protocol();

  bool pull(net::address_base&) override;
  std::future<void> pull_async(net::address_base&) override;
  void request(net::address_base&) override;
  bool push(const net::address_base&) override;
  bool observe(net::address_base&, bool) override;
  bool observe_quietly(net::address_base&, bool) override;
  bool update(net::node_base& node_base) override;

  void set_device(net::device_base& dev) override;

  void set_command_callback(std::function<void()>);
  void run_commands();
};
}
