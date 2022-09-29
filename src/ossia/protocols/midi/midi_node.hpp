#pragma once
#include <ossia/detail/ptr_container.hpp>
#include <ossia/network/base/node.hpp>

namespace ossia::net::midi
{
class midi_protocol;
class midi_device;
class OSSIA_EXPORT midi_node : public ossia::net::node_base
{
protected:
  midi_device& m_device;
  node_base* m_parent{};
  std::unique_ptr<parameter_base> m_parameter;

public:
  using iterator = ossia::ptr_container<ossia::net::node_base>::iterator;
  using const_iterator = ossia::ptr_container<ossia::net::node_base>::const_iterator;
  ~midi_node() override;
  midi_node(midi_device& aDevice, ossia::net::node_base& aParent);
  midi_node(midi_device& aDevice);

  device_base& get_device() const final override;
  node_base* get_parent() const final override;

  node_base& set_name(std::string) override;

  parameter_base* get_parameter() const final override;
  parameter_base* create_parameter(val_type) final override;
  bool remove_parameter() final override;

  std::unique_ptr<node_base> make_child(const std::string& name) override;
  void removing_child(node_base& node_base) final override;

  //! Explicitly add a child node (which has to be valid)
  midi_node* add_midi_node(std::unique_ptr<midi_node> n);
};
}
