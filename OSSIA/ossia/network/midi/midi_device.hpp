#pragma once

#include <ossia/network/base/device.hpp>
#include <ossia/network/midi/midi_node.hpp>

namespace ossia::net::midi
{
class OSSIA_EXPORT midi_device final : public ossia::net::device_base,
                                       public midi_node
{
public:
  midi_device(std::unique_ptr<ossia::net::protocol_base> prot);

  //! Create a default MIDI tree with all the nodes available
  bool create_full_tree();

  using midi_node::get_name;
  node_base& set_name(std::string n) override;

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
