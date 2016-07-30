#pragma once

#include <ossia/network/base/device.hpp>
#include <ossia/network/midi/midi_node.hpp>

namespace ossia
{
namespace net
{
class OSSIA_EXPORT MIDIDevice final :
    public ossia::net::device,
    public MIDINode
{
    std::string mName;

  public:
    MIDIDevice(std::unique_ptr<ossia::net::protocol> prot);

    std::string getName() const override;
    node & setName(std::string n) override;

    bool updateNamespace() override;

    const ossia::net::node& getRootNode() const override{ return *this; }
    ossia::net::node& getRootNode() override { return *this; }
};
}
}
