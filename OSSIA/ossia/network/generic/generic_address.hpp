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

  void pullValue() final override;
  std::future<void> pullValueAsync() final override;
  void requestValue() final override;

  ossia::net::generic_address& pushValue(const ossia::value&) final override;
  ossia::net::generic_address& pushValue() final override;

  const ossia::value& getValue() const;
  ossia::value cloneValue() const final override;
  generic_address& setValue(const ossia::value&) override;

  /** Set a value without sending notifications **/
  void setValueQuiet(const ossia::value&) override;

  ossia::val_type getValueType() const final override;
  ossia::net::generic_address& setValueType(ossia::val_type) final override;

  ossia::access_mode getAccessMode() const final override;
  ossia::net::generic_address& setAccessMode(ossia::access_mode) final override;

  const ossia::domain& getDomain() const final override;
  ossia::net::generic_address& setDomain(const ossia::domain&) final override;

  ossia::bounding_mode getBoundingMode() const final override;
  ossia::net::generic_address& setBoundingMode(ossia::bounding_mode) final override;

  ossia::repetition_filter getRepetitionFilter() const final override;
  ossia::net::generic_address&
      setRepetitionFilter(ossia::repetition_filter) final override;
  bool filterRepetition(const ossia::value& val) const final override;

  ossia::unit_t getUnit() const final override;
  generic_address& setUnit(const ossia::unit_t& v) final override;

  bool getMuted() const final override;
  generic_address& setMuted(bool v) final override;

  void onFirstCallbackAdded() final override;
  void onRemovingLastCallback() final override;

private:
  friend struct update_address_visitor;
};
}
}
