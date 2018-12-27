#pragma once
#include <ossia/network/base/parameter.hpp>

namespace ossia
{

class OSSIA_EXPORT texture_generic_parameter
    : public ossia::net::parameter_base
{
  uint32_t m_tex{};

public:
  using tex_id = uint32_t;
  texture_generic_parameter(ossia::net::node_base& n)
      : ossia::net::parameter_base{n}
  {
  }

  virtual ~texture_generic_parameter();

  void clone_value() const
  {
  }

  void push_value(uint32_t)
  {
  }

  void pull_value() override
  {
  }
  net::parameter_base& push_value(const ossia::value&) override
  {
    return *this;
  }
  net::parameter_base& push_value(ossia::value&&) override
  {
    return *this;
  }
  net::parameter_base& push_value() override
  {
    return *this;
  }
  ossia::value value() const override
  {
    return {};
  }
  net::parameter_base& set_value(const ossia::value&) override
  {
    return *this;
  }
  net::parameter_base& set_value(ossia::value&&) override
  {
    return *this;
  }
  val_type get_value_type() const override
  {
    return {};
  }
  net::parameter_base& set_value_type(val_type) override
  {
    return *this;
  }
  access_mode get_access() const override
  {
    return {};
  }
  net::parameter_base& set_access(access_mode) override
  {
    return *this;
  }
  const domain& get_domain() const override
  {
    throw;
  }
  net::parameter_base& set_domain(const domain&) override
  {
    return *this;
  }
  bounding_mode get_bounding() const override
  {
    return {};
  }
  net::parameter_base& set_bounding(bounding_mode) override
  {
    return *this;
  }
};


}
