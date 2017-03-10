#include <ossia/network/midi/midi_address.hpp>
#include <ossia/network/midi/midi_device.hpp>
#include <ossia/network/midi/midi_node.hpp>

namespace ossia
{
namespace net
{
namespace midi
{

struct midi_name_table
{
  midi_name_table()
  {
    for(int i = 0; i < 128; i++)
      names[i] = boost::lexical_cast<std::string>(i);
  }

  std::array<std::string, 128> names;
};

const std::string& midi_node_name(midi_size_t i)
{
  static const midi_name_table tbl;
  return tbl.names[i];
}

midi_node::~midi_node()
{

}

midi_node::midi_node(midi_device& aDevice, node_base& aParent)
    : mDevice{aDevice}, mParent{aParent}
{
}

device_base& midi_node::getDevice() const
{
  return mDevice;
}

node_base* midi_node::getParent() const
{
  return &mParent;
}

node_base& midi_node::setName(std::string)
{
  return *this;
}

address_base* midi_node::getAddress() const
{
  return mAddress.get();
}

address_base* midi_node::createAddress(val_type)
{
  return mAddress.get();
}

bool midi_node::removeAddress()
{
  return false;
}

std::unique_ptr<node_base> midi_node::makeChild(const std::string& name)
{
  return nullptr;
}

void midi_node::removingChild(node_base& node)
{
}
}
}
}
