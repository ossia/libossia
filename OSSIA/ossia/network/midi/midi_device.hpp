#pragma once

#include <ossia/network/base/device.hpp>
#include <ossia/network/midi/midi_node.hpp>

namespace ossia
{
namespace net
{
namespace midi
{
class OSSIA_EXPORT midi_device final :
    public ossia::net::device_base,
    public midi_node
{
    std::string mName;

  public:
    midi_device(std::unique_ptr<ossia::net::protocol_base> prot);

    std::string getName() const override;
    node_base & setName(std::string n) override;

    bool updateNamespace();

    const ossia::net::node_base& getRootNode() const override{ return *this; }
    ossia::net::node_base& getRootNode() override { return *this; }
};
}
}
}
