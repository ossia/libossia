#include <ossia/gfx/texture_parameter.hpp>

namespace ossia::gfx
{
texture_input_parameter::~texture_input_parameter() { }

texture_parameter::~texture_parameter() { }

void texture_parameter::pull_value() { }

net::parameter_base& texture_parameter::push_value(const ossia::value&) { return *this; }

net::parameter_base& texture_parameter::push_value(ossia::value&&) { return *this; }

net::parameter_base& texture_parameter::push_value() { return *this; }

value texture_parameter::value() const { return {}; }

ossia::value texture_parameter::set_value(const ossia::value&) { return {}; }

ossia::value texture_parameter::set_value(ossia::value&&) { return {}; }

val_type texture_parameter::get_value_type() const { return {}; }

net::parameter_base& texture_parameter::set_value_type(val_type) { return *this; }

access_mode texture_parameter::get_access() const { return {}; }

net::parameter_base& texture_parameter::set_access(access_mode) { return *this; }

const domain& texture_parameter::get_domain() const { throw; }

net::parameter_base& texture_parameter::set_domain(const domain&) { return *this; }

bounding_mode texture_parameter::get_bounding() const { return {}; }

net::parameter_base& texture_parameter::set_bounding(bounding_mode) { return *this; }
}
