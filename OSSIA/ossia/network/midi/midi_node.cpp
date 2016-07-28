#include <ossia/network/midi/midi_node.hpp>
#include <ossia/network/midi/midi_device.hpp>
#include <ossia/network/midi/midi_address.hpp>

namespace OSSIA
{
namespace net
{
MIDINode::MIDINode(MIDIDevice& aDevice, Node& aParent):
  mDevice{aDevice},
  mParent{aParent}
{

}

Device& MIDINode::getDevice() const
{
  return mDevice;
}

Node* MIDINode::getParent() const
{
  return &mParent;
}

Node& MIDINode::setName(std::string)
{
  return *this;
}

Address* MIDINode::getAddress() const
{
  return mAddress.get();
}

Address* MIDINode::createAddress(Type)
{
  return mAddress.get();
}

bool MIDINode::removeAddress()
{
  return false;
}

std::unique_ptr<Node> MIDINode::makeChild(const std::string& name)
{
  return nullptr;
}

void MIDINode::removingChild(Node& node)
{

}
}
}
