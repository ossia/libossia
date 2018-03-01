#pragma once
#include <ossia/network/base/parameter.hpp>
#include <ossia/network/base/node.hpp>
#include <ossia/network/domain/domain.hpp>
#include <ossia/network/phidgets/detail/phidgetspp.hpp>
namespace ossia
{
class phidget_protocol;
class phidget_parameter final : public ossia::net::parameter_base
{
  phidget_protocol& m_protocol;
  ppp::phidget_ptr m_phidget;
  //ppp::interface_kit* m_ik{};
  ossia::domain m_domain;

public:
  phidget_parameter(
      ppp::phidget_ptr p, phidget_protocol& proto, ossia::net::node_base& par);
  ~phidget_parameter();

  ppp::phidget_ptr phidget() const
  {
    return m_phidget;
  }

  void pull_value() override;
  ossia::value value() const override;
  net::parameter_base& push_value(const ossia::value&) override;
  net::parameter_base& push_value(ossia::value&&) override;
  net::parameter_base& push_value() override;
  net::parameter_base& set_value(const ossia::value&) override;
  net::parameter_base& set_value(ossia::value&&) override;
  val_type get_value_type() const override;
  net::parameter_base& set_value_type(val_type) override;
  access_mode get_access() const override;
  net::parameter_base& set_access(access_mode) override;
  const domain& get_domain() const override;
  net::parameter_base& set_domain(const domain&) override;
  bounding_mode get_bounding() const override;
  net::parameter_base& set_bounding(bounding_mode) override;
};
}
