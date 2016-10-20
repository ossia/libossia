#pragma once
#include <ossia/detail/callback_container.hpp>
#include <ossia/editor/value/value.hpp>
#include <ossia/network/domain/domain.hpp>
#include <ossia/network/base/address.hpp>
#include <ossia/network/generic/generic_device.hpp>

#include <boost/optional.hpp>
#include <mutex>
#include <thread>

namespace ossia
{
namespace net
{

struct generic_address_data
{
  generic_address_data() = default;
  generic_address_data(const generic_address_data&) = default;
  generic_address_data(generic_address_data&&) = default;
  generic_address_data& operator=(const generic_address_data&) = default;
  generic_address_data& operator=(generic_address_data&&) = default;


  generic_address_data(std::string n):
    node_name{std::move(n)}
  {

  }

  std::string node_name;
  boost::optional<ossia::val_type> type;
  boost::optional<ossia::net::domain> domain;
  boost::optional<ossia::access_mode> access;
  boost::optional<ossia::bounding_mode> bounding;
  boost::optional<ossia::repetition_filter> repetition_filter;

  std::string description;
  ossia::value default_value;
  tags_t tags;
  unit_t unit;
};

class protocol_base;
class OSSIA_EXPORT generic_address : public ossia::net::address_base
{
protected:
  const ossia::net::node_base& mNode;
  ossia::net::protocol_base& mProtocol;

  ossia::val_type mValueType{};
  ossia::access_mode mAccessMode{};
  ossia::bounding_mode mBoundingMode{};
  ossia::repetition_filter mRepetitionFilter{};

  mutable std::mutex mValueMutex;
  ossia::value mValue;

  ossia::net::domain mDomain;
  std::string mDescription;
  ossia::value mDefault;
  tags_t mTags;
  unit_t mUnit;

  ossia::net::value_callback mCallback;

public:
  ossia::value PreviousValue;
  generic_address(
      const ossia::net::node_base& node_base);
  generic_address(
      const generic_address_data&,
      const ossia::net::node_base& node_base);

  ~generic_address();

  const ossia::net::node_base& getNode() const final override;

  void pullValue() final override;

  ossia::net::generic_address& pushValue(const ossia::value&) final override;
  ossia::net::generic_address& pushValue() final override;

  const ossia::value& getValue() const;
  ossia::value cloneValue() const final override;
  generic_address& setValue(const ossia::value&) final override;

  ossia::val_type getValueType() const final override;
  ossia::net::generic_address& setValueType(ossia::val_type) final override;

  ossia::access_mode getAccessMode() const final override;
  ossia::net::generic_address& setAccessMode(ossia::access_mode) final override;

  const ossia::net::domain& getDomain() const final override;
  ossia::net::generic_address& setDomain(const ossia::net::domain&) final override;

  ossia::bounding_mode getBoundingMode() const final override;
  ossia::net::generic_address& setBoundingMode(ossia::bounding_mode) final override;

  ossia::repetition_filter getRepetitionFilter() const final override;
  ossia::net::generic_address&
      setRepetitionFilter(ossia::repetition_filter) final override;

  std::vector<std::string> getTags() const final override;
  generic_address& setTags(const std::vector<std::string>& v) final override;

  std::string getDescription() const final override;
  generic_address& setDescription(const std::string& v) final override;

  ossia::value getDefaultValue() const final override;
  generic_address& setDefaultValue(const ossia::value& v) final override;

  ossia::unit_t getUnit() const final override;
  generic_address& setUnit(const ossia::unit_t& v) final override;

  void onFirstCallbackAdded() final override;
  void onRemovingLastCallback() final override;

private:
};
}
}
