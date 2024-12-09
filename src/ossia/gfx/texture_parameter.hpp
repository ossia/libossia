#pragma once
#include <ossia/gfx/port_index.hpp>
#include <ossia/network/base/parameter.hpp>

namespace ossia::gfx
{
class OSSIA_EXPORT texture_parameter : public ossia::net::parameter_base
{
public:
  explicit texture_parameter(ossia::net::node_base& n)
      : parameter_base{n}
  {
    m_type = parameter_type::TEXTURE;
  }

  virtual ~texture_parameter();

  // For input
  virtual void pull_texture(ossia::gfx::port_index idx);

  void pull_value() override;
  ossia::net::parameter_base& push_value(const ossia::value&) override;
  ossia::net::parameter_base& push_value(ossia::value&&) override;
  ossia::net::parameter_base& push_value() override;
  ossia::value value() const override;
  ossia::value set_value(const ossia::value&) override;
  ossia::value set_value(ossia::value&&) override;
  ossia::val_type get_value_type() const noexcept override;
  ossia::net::parameter_base& set_value_type(ossia::val_type) override;
  ossia::access_mode get_access() const noexcept override;
  ossia::net::parameter_base& set_access(ossia::access_mode) override;
  const ossia::domain& get_domain() const noexcept override;
  ossia::net::parameter_base& set_domain(const ossia::domain&) override;
  ossia::bounding_mode get_bounding() const noexcept override;
  ossia::net::parameter_base& set_bounding(ossia::bounding_mode) override;
};
class OSSIA_EXPORT geometry_parameter : public ossia::net::parameter_base
{
public:
  explicit geometry_parameter(ossia::net::node_base& n)
      : parameter_base{n}
  {
    m_type = parameter_type::GEOMETRY;
  }

  virtual ~geometry_parameter();

  // For input
  virtual void pull_geometry(ossia::gfx::port_index idx);

  void pull_value() override;
  ossia::net::parameter_base& push_value(const ossia::value&) override;
  ossia::net::parameter_base& push_value(ossia::value&&) override;
  ossia::net::parameter_base& push_value() override;
  ossia::value value() const override;
  ossia::value set_value(const ossia::value&) override;
  ossia::value set_value(ossia::value&&) override;
  ossia::val_type get_value_type() const noexcept override;
  ossia::net::parameter_base& set_value_type(ossia::val_type) override;
  ossia::access_mode get_access() const noexcept override;
  ossia::net::parameter_base& set_access(ossia::access_mode) override;
  const ossia::domain& get_domain() const noexcept override;
  ossia::net::parameter_base& set_domain(const ossia::domain&) override;
  ossia::bounding_mode get_bounding() const noexcept override;
  ossia::net::parameter_base& set_bounding(ossia::bounding_mode) override;
};
}
