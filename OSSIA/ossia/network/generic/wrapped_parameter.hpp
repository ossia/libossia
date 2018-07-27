#pragma once
#include <ossia/network/generic/generic_parameter.hpp>
#include <ossia/network/base/protocol.hpp>

namespace ossia::net
{

template<typename T, typename Parameter_T>
class wrapped_node
    : public ossia::net::node_base
{
  ossia::net::device_base& m_device;
  ossia::net::node_base* m_parent{};
  std::unique_ptr<Parameter_T> m_parameter;

public:
  using data_type = T;
  wrapped_node(
      const T& data, ossia::net::device_base& aDevice,
      ossia::net::node_base& aParent)
    : m_device{aDevice}, m_parent{&aParent}
  {
    m_name = data.name;
    if (data.valid())
      m_parameter = std::make_unique<Parameter_T>(data, *this);
  }

  wrapped_node(const T& data, ossia::net::device_base& aDevice)
    : m_device{aDevice}
  {
    m_name = data.name;
    if (data.valid())
      m_parameter = std::make_unique<Parameter_T>(data, *this);
  }

  ~wrapped_node() override
  {
    about_to_be_deleted(*this);

    write_lock_t lock{m_mutex};
    m_children.clear();
    m_parameter.reset();
  }

  device_base& get_device() const final override
  {
    return m_device;
  }

  node_base* get_parent() const final override
  {
    return m_parent;
  }

  node_base& set_name(std::string) final override
  {
    return *this;
  }

  parameter_base* get_parameter() const final override
  {
    return m_parameter.get();
  }

  parameter_base* create_parameter(val_type = val_type::IMPULSE) final override
  {
    return nullptr;
  }

  bool remove_parameter() final override
  {
    return false;
  }

  void add_child(std::unique_ptr<ossia::net::node_base> p)
  {
    if (p)
    {
      write_lock_t lock{m_mutex};
      m_children.push_back(std::move(p));
    }
  }

private:
  std::unique_ptr<node_base> make_child(const std::string& name) final override
  {
    return nullptr;
  }

  void removing_child(node_base& node_base) final override
  {
  }
};

template<typename T>
class wrapped_parameter final
    : public ossia::net::generic_parameter
{
public:
  using base_data_type = typename T::base_data_type;
  wrapped_parameter(
      const T& data,
      ossia::net::node_base& node_base)
    : generic_parameter{data, node_base}, m_data{data}
  {
  }

  wrapped_parameter() = delete;
  wrapped_parameter(const wrapped_parameter& other) = delete;
  wrapped_parameter(wrapped_parameter&& other) = delete;
  wrapped_parameter& operator=(const wrapped_parameter& other)
      = delete;
  wrapped_parameter& operator=(wrapped_parameter&& other)
      = delete;
  ~wrapped_parameter() = default;

  const base_data_type& data() const
  { return m_data; }

  base_data_type& data()
  { return m_data; }

private:
  base_data_type m_data;
};


template<typename Node_T, typename Protocol_T>
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

  wrapped_device(
      std::unique_ptr<Protocol_T> protocol_base,
      std::string name)
    : device_base(std::move(protocol_base))
    , Node_T{typename Node_T::data_type{name}, *this}
  {
    m_protocol->set_device(*this);
  }

  const ossia::net::node_base& get_root_node() const override
  {
    return *this;
  }
  ossia::net::node_base& get_root_node() override
  {
    return *this;
  }

  using Node_T::get_name;
  using Node_T::set_name;

  ~wrapped_device()
  {
    // TODO c.f. generic_device
  }
};
}
