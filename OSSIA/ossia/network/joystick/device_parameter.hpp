
#pragma once

#include <ossia/network/base/node.hpp>
#include <ossia/network/base/protocol.hpp>
#include <ossia/network/common/complex_type.hpp>
#include <ossia/network/domain/domain.hpp>
#include <ossia/network/oscquery/oscquery_server.hpp>
#include <ossia/network/value/value.hpp>

namespace ossia::net
{

class device_parameter : public ossia::net::parameter_base
{

public:
  device_parameter(
      ossia::net::node_base& node, const ossia::val_type type,
      const ossia::bounding_mode bounding, const ossia::access_mode access,
      const ossia::domain domain);

  virtual ~device_parameter();

  void device_value_change_event(const ossia::value& value);

  void pull_value() override;
  ossia::value value() const override;

  ossia::net::parameter_base& push_value(const ossia::value& val) override;
  ossia::net::parameter_base& push_value(ossia::value&& val) override;
  ossia::net::parameter_base& push_value() override;

  ossia::net::parameter_base& set_value(const ossia::value& val) override;
  ossia::net::parameter_base& set_value(ossia::value&& v) override;

  ossia::val_type get_value_type() const override;
  ossia::net::parameter_base& set_value_type(ossia::val_type) override
  {
    return *this;
  }

  ossia::access_mode get_access() const override;
  ossia::net::parameter_base& set_access(ossia::access_mode) override
  {
    return *this;
  }

  const ossia::domain& get_domain() const override;
  ossia::net::parameter_base& set_domain(const ossia::domain&) override
  {
    return *this;
  }

  ossia::bounding_mode get_bounding() const override;
  ossia::net::parameter_base& set_bounding(ossia::bounding_mode) override
  {
    return *this;
  }

  template <class ParamType = device_parameter, class... T>
  static ParamType* create_device_parameter(
      ossia::net::node_base& root_node, const std::string& path,
      const ossia::value& initial_value, const T&... ctor_args)
  {
    static_assert(std::is_base_of<device_parameter, ParamType>::value);

    auto& param_node = ossia::net::create_node(root_node, path);
    auto param = std::make_unique<ParamType>(param_node, ctor_args...);

    ParamType* ret = param.get();

    param->set_value(initial_value);
    param_node.set_parameter(std::move(param));

    return ret;
  }

protected:
  virtual void device_update_value()
  {
  }

  auto& get_protocol()
  {
    return get_node().get_device().get_protocol();
  }

  ossia::value m_current_value;

  const ossia::val_type m_type;
  const ossia::bounding_mode m_bounding;
  const ossia::access_mode m_access;
  const ossia::domain m_domain;
};
}
