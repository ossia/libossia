#pragma once
#include <ossia/network/base/parameter.hpp>
#include <ossia/gfx/port_index.hpp>

namespace ossia::gfx
{
class OSSIA_EXPORT texture_parameter : public ossia::net::parameter_base
{
public:
  using parameter_base::parameter_base;
  virtual void pull_texture(ossia::gfx::port_index idx) = 0;
  virtual ~texture_parameter();

  void pull_value() override;
  ossia::net::parameter_base& push_value(const ossia::value&) override;
  ossia::net::parameter_base& push_value(ossia::value&&) override;
  ossia::net::parameter_base& push_value() override;
  ossia::value value() const override;
  ossia::net::parameter_base& set_value(const ossia::value&) override;
  ossia::net::parameter_base& set_value(ossia::value&&) override;
  ossia::val_type get_value_type() const override;
  ossia::net::parameter_base& set_value_type(ossia::val_type) override;
  ossia::access_mode get_access() const override;
  ossia::net::parameter_base& set_access(ossia::access_mode) override;
  const ossia::domain& get_domain() const override;
  ossia::net::parameter_base& set_domain(const ossia::domain&) override;
  ossia::bounding_mode get_bounding() const override;
  ossia::net::parameter_base& set_bounding(ossia::bounding_mode) override;
};
}
