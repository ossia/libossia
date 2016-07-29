#pragma once

#include <ossia/network/base/node.hpp>

namespace ossia
{
namespace net
{
class MIDI;
class MIDIDevice;
class OSSIA_EXPORT MIDINode :
    public ossia::net::Node
{
  protected:
    MIDIDevice& mDevice;
    Node& mParent;
    std::unique_ptr<address> mAddress;

  public:
    using iterator = ossia::ptr_container<ossia::net::Node>::iterator;
    using const_iterator = ossia::ptr_container<ossia::net::Node>::const_iterator;
    MIDINode() = default;
    MIDINode(
        MIDIDevice& aDevice,
        ossia::net::Node& aParent);

    Device& getDevice() const final override;
    Node* getParent() const final override;

    Node& setName(std::string) override;

    address* getAddress() const final override;
    address* createAddress(Type) final override;
    bool removeAddress() final override;

    std::unique_ptr<Node> makeChild(const std::string& name) final override;
    void removingChild(Node& node) final override;
};

}
}
