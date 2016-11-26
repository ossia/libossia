#pragma once
#include <ossia/detail/ptr_container.hpp>
#include <ossia/editor/scenario/clock.hpp>
#include <ossia/editor/scenario/time_process.hpp>
#include <ossia/editor/state/state_element.hpp>
#include <ossia/editor/state/state.hpp>
#include <ossia/network/base/address.hpp>
#include <ossia/editor/curve/behavior.hpp>
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
      ossia::Destination,
      ossia::Destination,
      ossia::behavior);

  ~mapper();

  const ossia::Destination& getDriverAddress() const;
  const ossia::Destination& getDrivenAddress() const;

  const ossia::behavior& getDriving() const;

private:
  ossia::state_element offset(ossia::time_value) override;

  ossia::state_element state() override;

  void start() override;
  void stop() override;
  void pause() override;
  void resume() override;

  static ossia::value computeValue(const ossia::value&, const ossia::behavior&);

  void driverValueCallback(ossia::value value);

  ossia::Destination mDriverAddress;
  ossia::Destination mDrivenAddress;
  ossia::behavior mDrive;

  ossia::message mLastMessage;
  ossia::value mValueToMap;
  mutable std::mutex mValueToMapMutex;

  ossia::net::address_base::callback_index mDriverValueCallbackIndex;
  bool mDriverValueObserved{};

  friend struct mapper_compute_visitor;
};
}
