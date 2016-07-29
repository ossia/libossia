#pragma once

#include <ossia/network/base/device.hpp>
#include <ossia/network/midi/midi_node.hpp>

namespace ossia
{
namespace net
{
class OSSIA_EXPORT MIDIDevice final :
    public ossia::net::Device,
    public MIDINode
{
    std::string mName;

  public:
    MIDIDevice(std::unique_ptr<ossia::net::Protocol> prot);

    std::string getName() const override;
    Node & setName(std::string n) override;

    bool updateNamespace() override;

    const ossia::net::Node& getRootNode() const override{ return *this; }
    ossia::net::Node& getRootNode() override { return *this; }
};
}
}
