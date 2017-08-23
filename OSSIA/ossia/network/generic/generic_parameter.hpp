#pragma once
#include <ossia/detail/callback_container.hpp>
#include <ossia/editor/value/value.hpp>
#include <ossia/network/base/parameter.hpp>
#include <ossia/network/base/node_attributes.hpp>
#include <ossia/network/domain/domain.hpp>
#include <ossia/network/generic/generic_device.hpp>

#include <ossia/detail/mutex.hpp>
#include <ossia/detail/optional.hpp>
#include <thread>

#include <string>
#include <vector>

namespace ossia
{
namespace net
{

struct parameter_data;
class protocol_base;

//! Default implementation for parameter_base, with everything expected and
//! mutable
class OSSIA_EXPORT generic_parameter : public ossia::net::parameter_base
{
protected:
  ossia::net::node_base& m_node;
  ossia::net::protocol_base& m_protocol;

  ossia::val_type m_valueType{};
  ossia::access_mode m_accessMode{};
  ossia::bounding_mode m_boundingMode{};
  ossia::repetition_filter m_repetitionFilter{};
  bool m_muted{};
  bool m_critical{};

  mutable mutex_t m_valueMutex;
  ossia::value m_value;

  ossia::domain m_domain;
  unit_t m_unit;

  ossia::value m_previousValue; //! Used for repetition filter.

public:
  generic_parameter(ossia::net::node_base& node_base);
  generic_parameter(const parameter_data&, ossia::net::node_base& node_base);

  ~generic_parameter();

  ossia::net::node_base& get_node() const final override;

  void pull_value() final override;
  std::future<void> pull_value_async() final override;
  void request_value() final override;

  ossia::net::generic_parameter& push_value(const ossia::value&) final override;
  ossia::net::generic_parameter& push_value(ossia::value&&) final override;
  ossia::net::generic_parameter& push_value() final override;

  const ossia::value& getValue() const;
  ossia::value value() const final override;
  generic_parameter& set_value(const ossia::value&) override;
  generic_parameter& set_value(ossia::value&&) override;

  /** Set a value without sending notifications **/
  void set_value_quiet(const ossia::value&) override;
  void set_value_quiet(ossia::value&&) override;
  void set_value_quiet(const ossia::Destination&);

  ossia::val_type get_value_type() const final override;
  ossia::net::generic_parameter& set_value_type(ossia::val_type) final override;

  ossia::access_mode get_access() const final override;
  ossia::net::generic_parameter& set_access(ossia::access_mode) final override;

  const ossia::domain& get_domain() const final override;
  ossia::net::generic_parameter& set_domain(const ossia::domain&) final override;

  ossia::bounding_mode get_bounding() const final override;
  ossia::net::generic_parameter&
      set_bounding(ossia::bounding_mode) final override;

  ossia::repetition_filter get_repetition_filter() const final override;
  ossia::net::generic_parameter&
      set_repetition_filter(ossia::repetition_filter) final override;
  bool filter_repetition(const ossia::value& val) const final override;

  ossia::unit_t get_unit() const final override;
  generic_parameter& set_unit(const ossia::unit_t& v) final override;

  bool get_muted() const final override;
  generic_parameter& set_muted(bool v) final override;

  bool get_critical() const final override;
  generic_parameter& set_critical(bool v) final override;

  void on_first_callback_added() final override;
  void on_removing_last_callback() final override;

private:
  friend struct update_parameter_visitor;
};
}
}
