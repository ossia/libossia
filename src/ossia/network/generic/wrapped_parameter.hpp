#pragma once
#include <ossia/network/base/osc_address.hpp>
#include <ossia/network/base/protocol.hpp>
#include <ossia/network/generic/generic_parameter.hpp>

namespace ossia::net
{

template <typename T, typename Parameter_T>
class wrapped_node : public ossia::net::node_base
{
  ossia::net::device_base& m_device;
  ossia::net::node_base* m_parent{};
  std::unique_ptr<Parameter_T> m_parameter;

public:
  using data_type = T;
  wrapped_node(
      T&& data, ossia::net::device_base& aDevice, ossia::net::node_base& aParent)
      : m_device{aDevice}
      , m_parent{&aParent}
  {
    m_name = data.name;
    m_extended = data.extended;
    m_oscAddressCache = ossia::net::osc_parameter_string(*this);
    if(data.valid())
      m_parameter.reset(new Parameter_T(std::move(data), *this));
  }

  wrapped_node(T&& data, ossia::net::device_base& aDevice)
      : m_device{aDevice}
  {
    m_name = data.name;
    m_extended = data.extended;
    m_oscAddressCache = ossia::net::osc_parameter_string(*this);
    if(data.valid())
      m_parameter.reset(new Parameter_T(std::move(data), *this));
  }

  ~wrapped_node() override { clear(); }

  void clear()
  {
    about_to_be_deleted(*this);

    write_lock_t lock{m_mutex};
    m_children.clear();
    m_parameter.reset();
  }

  device_base& get_device() const final override { return m_device; }

  node_base* get_parent() const final override { return m_parent; }

  node_base& set_name(std::string) final override { return *this; }

  parameter_base* get_parameter() const final override { return m_parameter.get(); }

  parameter_base* create_parameter(val_type type = val_type::IMPULSE) final override
  {
    // Same contract as generic_node: creating a parameter on a node that
    // already has one only retypes it.
    if(m_parameter)
    {
      m_parameter->set_value_type(type);
      return m_parameter.get();
    }

    // The protocols that use this class downcast every parameter of their tree
    // to Parameter_T without checking, so a parameter created from the generic
    // APIs has to be a Parameter_T too - see make_child below.
    T data{m_name};
    data.type = type;
    data.value = ossia::init_value(type);
    m_parameter = std::make_unique<Parameter_T>(std::move(data), *this);

    m_device.on_parameter_created(*m_parameter);
    return m_parameter.get();
  }

  bool remove_parameter() final override { return false; }

  void add_child(std::unique_ptr<ossia::net::node_base> p)
  {
    if(p)
    {
      write_lock_t lock{m_mutex};
      m_children.push_back(std::move(p));
    }
  }

private:
  std::unique_ptr<node_base> make_child(const std::string& name) final override
  {
    // Every node of a wrapped device has to be a wrapped_node: the protocols
    // built on top of this (serial, http, websocket, score's mapper) recover
    // their own node and parameter types from the tree with an unchecked
    // static_cast. Returning a generic_node here - as this used to - meant that
    // any node created through the generic APIs, e.g. the `Device.addNode()`
    // binding exposed to the protocols' own QML scripts, carried a
    // generic_parameter that those casts would then reinterpret.
    return std::make_unique<wrapped_node>(T{name}, m_device, *this);
  }

  void removing_child(node_base& node_base) final override { }
};

template <typename T>
class wrapped_parameter : public ossia::net::generic_parameter
{
public:
  using base_data_type = typename T::base_data_type;

  wrapped_parameter(T&& data, ossia::net::node_base& node_base)
      : generic_parameter{data, node_base}
      , m_data(std::move(data))
  {
  }

  wrapped_parameter() = delete;
  wrapped_parameter(const wrapped_parameter& other) = delete;
  wrapped_parameter(wrapped_parameter&& other) = delete;
  wrapped_parameter& operator=(const wrapped_parameter& other) = delete;
  wrapped_parameter& operator=(wrapped_parameter&& other) = delete;
  ~wrapped_parameter() { callback_container<value_callback>::callbacks_clear(); }

  const base_data_type& data() const { return m_data; }

  base_data_type& data() { return m_data; }

private:
  base_data_type m_data;
};

template <typename Node_T, typename Protocol_T>
class wrapped_device final
    : public ossia::net::device_base
    , public Node_T
{
public:
  wrapped_device() = delete;
  wrapped_device(const wrapped_device&) = delete;
  wrapped_device(wrapped_device&&) = delete;
  wrapped_device& operator=(const wrapped_device&) = delete;
  wrapped_device& operator=(wrapped_device&&) = delete;

  wrapped_device(std::unique_ptr<Protocol_T> protocol_base, std::string name)
      : device_base(std::move(protocol_base))
      , Node_T{typename Node_T::data_type{name}, *this}
  {
    m_capabilities.change_tree = true;
    m_protocol->set_device(*this);
  }

  const ossia::net::node_base& get_root_node() const override { return *this; }
  ossia::net::node_base& get_root_node() override { return *this; }

  using Node_T::get_name;
  using Node_T::set_name;

  ~wrapped_device()
  {
    m_protocol->stop();

    this->remove_parameter();

    {
      write_lock_t lock{this->m_mutex};
      this->m_children.clear();
    }

    // Parameters, etc of the device's own node must also be cleared
    // before removing the protocol:
    Node_T::clear();

    m_protocol.reset();
  }
};
}
