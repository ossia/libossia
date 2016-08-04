#pragma once
#include <ossia/detail/ptr_container.hpp>
#include <ossia/editor/scenario/clock.hpp>
#include <ossia/editor/scenario/time_process.hpp>
#include <ossia/editor/state/state.hpp>
#include <ossia/network/base/address.hpp>
#include <ossia_export.h>

#include <mutex>

namespace ossia
{
namespace net
{
class address_base;
}
class value;
class time_value;

/**
 * @brief The mapper class
 *
 * Allows to map a value to another following a transfer function.
 * The driver address is where the input value is taken from;
 * The driven address is where the output value is sent to.
 */
class OSSIA_EXPORT mapper final :
    public ossia::time_process
{
public:
  mapper(
      ossia::net::address_base&, ossia::net::address_base&,
      const ossia::value&);

  ~mapper();

  const ossia::net::address_base& getDriverAddress() const;
  const ossia::net::address_base& getDrivenAddress() const;

  const ossia::value& getDriving() const;

private:
  ossia::state_element offset(ossia::time_value) override;

  ossia::state_element state() override;

  void start() override;
  void stop() override;
  void pause() override;
  void resume() override;

  ossia::value computeValue(const ossia::value&, const ossia::value&);

  void driverValueCallback(const ossia::value& value);

    ossia::net::address_base& mDriverAddress;
    ossia::net::address_base& mDrivenAddress;
    ossia::value mDrive;

    ossia::message mLastMessage;
    ossia::value mValueToMap;
    mutable std::mutex mValueToMapMutex;

    ossia::net::address_base::callback_index mDriverValueCallbackIndex;
    bool mDriverValueObserved{};
};
}
