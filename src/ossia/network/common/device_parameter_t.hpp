#pragma once

#include <ossia/network/base/node.hpp>
#include <ossia/network/base/protocol.hpp>
#include <ossia/network/common/complex_type.hpp>
#include <ossia/network/domain/domain.hpp>
#include <ossia/network/oscquery/oscquery_server.hpp>
#include <ossia/network/value/value.hpp>

namespace ossia::net
{
template <typename T>
class device_parameter_t : public ossia::net::parameter_base
{
public:
  explicit device_parameter_t(net::node_base& node)
      : net::parameter_base{node}
      , m_current_value{}
  {
    set_repetition_filter(repetition_filter::ON);
  }

  ~device_parameter_t() = default;

  void device_value_change_event(const ossia::value& val)
  {
    if(val.valid())
    {
      m_current_value = val;
      get_protocol().push(*this);
    }
  }

  void pull_value() override { get_protocol().pull(*this); }

  ossia::value value() const override { return m_current_value; }

  net::parameter_base& push_value(const ossia::value& val) override
  {
    set_value(val);
    get_protocol().push(*this);
    return *this;
  }

  net::parameter_base& push_value(ossia::value&& val) override
  {
    return push_value(val);
  }

  net::parameter_base& push_value() override
  {
    get_protocol().push(*this);
    return *this;
  }

  ossia::value set_value(const ossia::value& val) override
  {
    if(val.valid())
    {
      m_current_value = ossia::convert<T>(val);
      send(val);
      device_update_value();
    }

    return m_current_value;
  }

  ossia::value set_value(ossia::value&& val) override { return set_value(val); }

  ossia::val_type get_value_type() const noexcept override
  {
    return ossia::value_trait<T>::ossia_enum;
  }

  ossia::net::parameter_base& set_value_type(ossia::val_type) override { return *this; }

  ossia::access_mode get_access() const noexcept override
  {
    return ossia::access_mode::SET;
  }
  ossia::net::parameter_base& set_access(ossia::access_mode) override { return *this; }

  const domain& get_domain() const noexcept override
  {
    static const thread_local ossia::domain dom;
    return dom;
  }

  ossia::bounding_mode get_bounding() const noexcept override
  {
    return ossia::bounding_mode::CLIP;
  }
  ossia::net::parameter_base& set_bounding(ossia::bounding_mode) override
  {
    return *this;
  }

  ossia::net::parameter_base& set_domain(const ossia::domain&) override { return *this; }

protected:
  virtual void device_update_value()
  {
    //  Here should be the code that actually make the hardware update to
    //  current value
  }

  T m_current_value;
};
}
