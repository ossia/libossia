#include <ossia/gfx/texture_parameter.hpp>

namespace ossia::gfx
{
static const ossia::domain dummy_domain;
texture_parameter::~texture_parameter() = default;

void texture_parameter::pull_texture(ossia::gfx::port_index idx) { }
void texture_parameter::pull_value() { }

net::parameter_base& texture_parameter::push_value(const ossia::value&)
{
  return *this;
}

net::parameter_base& texture_parameter::push_value(ossia::value&&)
{
  return *this;
}

net::parameter_base& texture_parameter::push_value()
{
  return *this;
}

value texture_parameter::value() const
{
  return {};
}

ossia::value texture_parameter::set_value(const ossia::value&)
{
  return {};
}

ossia::value texture_parameter::set_value(ossia::value&&)
{
  return {};
}

val_type texture_parameter::get_value_type() const noexcept
{
  return {};
}

net::parameter_base& texture_parameter::set_value_type(val_type)
{
  return *this;
}

access_mode texture_parameter::get_access() const noexcept
{
  return {};
}

net::parameter_base& texture_parameter::set_access(access_mode)
{
  return *this;
}

const domain& texture_parameter::get_domain() const noexcept
{
  return dummy_domain;
}

net::parameter_base& texture_parameter::set_domain(const domain&)
{
  return *this;
}

bounding_mode texture_parameter::get_bounding() const noexcept
{
  return {};
}

net::parameter_base& texture_parameter::set_bounding(bounding_mode)
{
  return *this;
}

geometry_parameter::~geometry_parameter() = default;
void geometry_parameter::pull_geometry(ossia::gfx::port_index idx) { }
void geometry_parameter::pull_value() { }

net::parameter_base& geometry_parameter::push_value(const ossia::value&)
{
  return *this;
}

net::parameter_base& geometry_parameter::push_value(ossia::value&&)
{
  return *this;
}

net::parameter_base& geometry_parameter::push_value()
{
  return *this;
}

value geometry_parameter::value() const
{
  return {};
}

ossia::value geometry_parameter::set_value(const ossia::value&)
{
  return {};
}

ossia::value geometry_parameter::set_value(ossia::value&&)
{
  return {};
}

val_type geometry_parameter::get_value_type() const noexcept
{
  return {};
}

net::parameter_base& geometry_parameter::set_value_type(val_type)
{
  return *this;
}

access_mode geometry_parameter::get_access() const noexcept
{
  return {};
}

net::parameter_base& geometry_parameter::set_access(access_mode)
{
  return *this;
}

const domain& geometry_parameter::get_domain() const noexcept
{
  return dummy_domain;
}

net::parameter_base& geometry_parameter::set_domain(const domain&)
{
  return *this;
}

bounding_mode geometry_parameter::get_bounding() const noexcept
{
  return {};
}

net::parameter_base& geometry_parameter::set_bounding(bounding_mode)
{
  return *this;
}
}
