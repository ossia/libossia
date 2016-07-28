#pragma once

#include <ossia/network/base/device.hpp>
#include <ossia/network/midi/midi_node.hpp>

namespace OSSIA
{
namespace net
{
class OSSIA_EXPORT MIDIDevice final :
    public OSSIA::net::Device,
    public MIDINode
{
    std::string mName;

  public:
    MIDIDevice(std::unique_ptr<OSSIA::net::Protocol> prot);

    std::string getName() const override;
    Node & setName(std::string n) override;

    bool updateNamespace() override;

    const OSSIA::net::Node& getRootNode() const override{ return *this; }
    OSSIA::net::Node& getRootNode() override { return *this; }
};
}
}
