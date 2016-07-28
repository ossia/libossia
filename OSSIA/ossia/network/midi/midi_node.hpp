#pragma once

#include <ossia/network/base/node.hpp>

namespace OSSIA
{
namespace net
{
class MIDI;
class MIDIDevice;
class OSSIA_EXPORT MIDINode :
    public OSSIA::net::Node
{
  protected:
    MIDIDevice& mDevice;
    Node& mParent;
    std::unique_ptr<Address> mAddress;

  public:
    using iterator = OSSIA::Container<OSSIA::net::Node>::iterator;
    using const_iterator = OSSIA::Container<OSSIA::net::Node>::const_iterator;
    MIDINode() = default;
    MIDINode(
        MIDIDevice& aDevice,
        OSSIA::net::Node& aParent);

    Device& getDevice() const final override;
    Node* getParent() const final override;

    Node& setName(std::string) override;

    Address* getAddress() const final override;
    Address* createAddress(Type) final override;
    bool removeAddress() final override;

    std::unique_ptr<Node> makeChild(const std::string& name) final override;
    void removingChild(Node& node) final override;
};

}
}
