#pragma once

#include <ossia/editor/curve/curve.hpp>
#include <ossia/editor/mapper/mapper.hpp>
#include <ossia/editor/scenario/time_constraint.hpp>
#include <ossia/editor/scenario/time_node.hpp>
#include <ossia/editor/scenario/time_value.hpp>
#include <ossia/editor/state/message.hpp>
#include <ossia/editor/value/value.hpp>
#include <ossia/network/base/address.hpp>

#include <ossia/editor/scenario/detail/TimeProcess_impl.hpp>

#include <mutex>
#include <thread>

using namespace std::placeholders;

namespace ossia
{
namespace detail
{
class mapper_impl : public ossia::mapper
{

private:
  ossia::net::address_base& mDriverAddress;
  ossia::net::address_base& mDrivenAddress;
  ossia::value mDrive;

  ossia::message mLastMessage;
  ossia::value mValueToMap;
  mutable std::mutex mValueToMapMutex;

  bool mDriverValueObserved{};
  ossia::net::address_base::callback_index mDriverValueCallbackIndex;

public:
  mapper_impl(
      ossia::net::address_base&, ossia::net::address_base&,
      const ossia::value&);

  ~mapper_impl();

private:
  ossia::state_element offset(ossia::time_value) override;

  ossia::state_element state() override;

  void start() override;
  void stop() override;
  void pause() override;
  void resume() override;

  const ossia::net::address_base& getDriverAddress() const override;

  const ossia::net::address_base& getDrivenAddress() const override;

  const ossia::value& getDriving() const override;

  ossia::value computeValue(const ossia::value&, const ossia::value&);

  void driverValueCallback(const ossia::value& value);
};
}
}
