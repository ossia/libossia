#pragma once

#include <ossia/network/base/device.hpp>
#include <ossia/network/midi/midi_node.hpp>

namespace ossia
{
namespace net
{
namespace midi
{
class OSSIA_EXPORT midi_device final : public ossia::net::device_base,
                                       public midi_node
{
  std::string m_name;

public:
  midi_device(std::unique_ptr<ossia::net::protocol_base> prot);

  std::string get_name() const override;
  node_base& set_name(std::string n) override;

  bool update_namespace();

  const ossia::net::node_base& get_root_node() const override
  {
    return *this;
  }
  ossia::net::node_base& get_root_node() override
  {
    return *this;
  }
};
}
}
}
