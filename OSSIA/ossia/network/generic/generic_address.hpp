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

  std::string node_name;
  boost::optional<ossia::val_type> type;
  boost::optional<ossia::net::domain> domain;
  boost::optional<ossia::access_mode> access;
  boost::optional<ossia::bounding_mode> bounding;
  boost::optional<ossia::repetition_filter> repetition_filter;
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

  ossia::net::value_callback mCallback;
  std::string mTextualAddress;

public:
  ossia::value PreviousValue;
  generic_address(
      const ossia::net::node_base& node_base);
  generic_address(
      const generic_address_data&,
      const ossia::net::node_base& node_base);

  ~generic_address();

  const ossia::net::node_base& getNode() const override;

  void pullValue() override;

  ossia::net::address_base& pushValue(const ossia::value&) override;
  ossia::net::address_base& pushValue() override;

  const ossia::value& getValue() const;
  ossia::value cloneValue(ossia::destination_index = {}) const override;
  address_base& setValue(const ossia::value&) override;

  ossia::val_type getValueType() const override;
  ossia::net::address_base& setValueType(ossia::val_type) override;

  ossia::access_mode getAccessMode() const override;
  ossia::net::address_base& setAccessMode(ossia::access_mode) override;

  const ossia::net::domain& getDomain() const override;
  ossia::net::address_base& setDomain(const ossia::net::domain&) override;

  ossia::bounding_mode getBoundingMode() const override;
  ossia::net::address_base& setBoundingMode(ossia::bounding_mode) override;

  ossia::repetition_filter getRepetitionFilter() const override;
  ossia::net::address_base&
      setRepetitionFilter(ossia::repetition_filter) override;

  void onFirstCallbackAdded() override;
  void onRemovingLastCallback() override;

  const std::string& getTextualAddress() const override
  {
    return mTextualAddress;
  }

private:
};
}
}
