#pragma once
#include <ossia/network/base/protocol.hpp>
#include <ossia/network/phidgets/phidgets_parameter_data.hpp>
#include <ossia/detail/hash_map.hpp>
#include <readerwriterqueue.h>
#include <ossia/detail/optional.hpp>

namespace ossia
{
namespace net {
class device_base;
}
class phidget_node;
class OSSIA_EXPORT phidget_protocol : public ossia::net::protocol_base
{
  net::device_base* m_dev{};
  std::function<void()> m_commandCb;
  moodycamel::ReaderWriterQueue<std::function<void()>> m_functionQueue;

  ossia::fast_hash_map<PhidgetHandle, ossia::phidget_node*> m_phidgetMap;

public:
  phidget_protocol();
  ~phidget_protocol() override;

  bool pull(net::parameter_base&) override;
  std::future<void> pull_async(net::parameter_base&) override;
  void request(net::parameter_base&) override;
  bool push(const net::parameter_base&) override;
  bool observe(net::parameter_base&, bool) override;
  bool observe_quietly(net::parameter_base&, bool) override;
  bool update(net::node_base& node_base) override;

  void set_device(net::device_base& dev) override;

  void set_command_callback(std::function<void()>);
  void run_commands();
  void run_command();
  bool push_raw(const ossia::net::full_parameter_data&) override;

private:

  ossia::net::node_base*  get_parent(ossia::phidget_handle_t hdl);
  PhidgetManagerHandle m_hdl{};

  void on_deviceCreated(PhidgetHandle phid);
  void on_deviceRemoved(phidget_id sn);
  void remove_parent_rec(ossia::net::node_base* par);
  void remove_node(ossia::net::node_base* par);
  void deleting_node(const ossia::net::node_base& par);
};
}
