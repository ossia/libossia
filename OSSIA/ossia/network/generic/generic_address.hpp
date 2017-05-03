#pragma once
#include <ossia/detail/callback_container.hpp>
#include <ossia/editor/value/value.hpp>
#include <ossia/network/domain/domain.hpp>
#include <ossia/network/base/address.hpp>
#include <ossia/network/base/address_data.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/base/node_attributes.hpp>

#include <ossia/detail/optional.hpp>
#include <ossia/detail/mutex.hpp>
#include <thread>

#include <vector>
#include <string>

namespace ossia
{
namespace net
{


class protocol_base;

//! Default implementation for address_base, with everything expected and mutable
class OSSIA_EXPORT generic_address : public ossia::net::address_base
{
protected:
  ossia::net::node_base& m_node;
  ossia::net::protocol_base& m_protocol;

  ossia::val_type m_valueType{};
  ossia::access_mode m_accessMode{};
  ossia::bounding_mode m_boundingMode{};
  ossia::repetition_filter m_repetitionFilter{};
  bool m_muted{};

  mutable mutex_t m_valueMutex;
  ossia::value m_value;

  ossia::domain m_domain;
  unit_t m_unit;

  ossia::value m_previousValue; //! Used for repetition filter.

public:
  generic_address(
      ossia::net::node_base& node_base);
  generic_address(
      const address_data&,
      ossia::net::node_base& node_base);

  ~generic_address();

  ossia::net::node_base& getNode() const final override;

  void pull_value() final override;
  std::future<void> pull_value_async() final override;
  void request_value() final override;

  ossia::net::generic_address& push_value(const ossia::value&) final override;
  ossia::net::generic_address& push_value() final override;

  const ossia::value& getValue() const;
  ossia::value value() const final override;
  generic_address& set_value(const ossia::value&) override;

  /** Set a value without sending notifications **/
  void set_value_quiet(const ossia::value&) override;
  void set_value_quiet(const ossia::Destination&);

  ossia::val_type get_value_type() const final override;
  ossia::net::generic_address& set_value_type(ossia::val_type) final override;

  ossia::access_mode get_access() const final override;
  ossia::net::generic_address& set_access(ossia::access_mode) final override;

  const ossia::domain& get_domain() const final override;
  ossia::net::generic_address& set_domain(const ossia::domain&) final override;

  ossia::bounding_mode get_bounding() const final override;
  ossia::net::generic_address& set_bounding(ossia::bounding_mode) final override;

  ossia::repetition_filter get_repetition_filter() const final override;
  ossia::net::generic_address&
      set_repetition_filter(ossia::repetition_filter) final override;
  bool filter_repetition(const ossia::value& val) const final override;

  ossia::unit_t get_unit() const final override;
  generic_address& set_unit(const ossia::unit_t& v) final override;

  bool get_muted() const final override;
  generic_address& set_muted(bool v) final override;

  void on_first_callback_added() final override;
  void on_removing_last_callback() final override;

private:
  friend struct update_address_visitor;
};
}
}
