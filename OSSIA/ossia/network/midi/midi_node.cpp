// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/network/midi/midi_parameter.hpp>
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
    for (int i = 0; i < 128; i++)
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
    : m_device{aDevice}, m_parent{aParent}
{
}

device_base& midi_node::get_device() const
{
  return m_device;
}

node_base* midi_node::get_parent() const
{
  return &m_parent;
}

node_base& midi_node::set_name(std::string)
{
  return *this;
}

parameter_base* midi_node::get_parameter() const
{
  return m_parameter.get();
}

parameter_base* midi_node::create_parameter(val_type)
{
  return m_parameter.get();
}

bool midi_node::remove_parameter()
{
  return false;
}

std::unique_ptr<node_base> midi_node::make_child(const std::string& name)
{
  return nullptr;
}

void midi_node::removing_child(node_base& node)
{
}
}
}
}
