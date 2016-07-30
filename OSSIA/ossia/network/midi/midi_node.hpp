#pragma once

#include <ossia/network/base/node.hpp>

namespace ossia
{
namespace net
{
class MIDI;
class MIDIDevice;
class OSSIA_EXPORT MIDINode :
    public ossia::net::node
{
  protected:
    MIDIDevice& mDevice;
    node& mParent;
    std::unique_ptr<address> mAddress;

  public:
    using iterator = ossia::ptr_container<ossia::net::node>::iterator;
    using const_iterator = ossia::ptr_container<ossia::net::node>::const_iterator;
    MIDINode() = default;
    MIDINode(
        MIDIDevice& aDevice,
        ossia::net::node& aParent);

    device& getDevice() const final override;
    node* getParent() const final override;

    node& setName(std::string) override;

    address* getAddress() const final override;
    address* createAddress(Type) final override;
    bool removeAddress() final override;

    std::unique_ptr<node> makeChild(const std::string& name) final override;
    void removingChild(node& node) final override;
};

}
}
