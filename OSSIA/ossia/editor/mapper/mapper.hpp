#pragma once
#include <ossia/detail/optional.hpp>
#include <ossia/detail/ptr_container.hpp>
#include <ossia/editor/curve/behavior.hpp>
#include <ossia/editor/scenario/clock.hpp>
#include <ossia/editor/scenario/time_process.hpp>
#include <ossia/editor/state/state.hpp>
#include <ossia/editor/state/state_element.hpp>
#include <ossia/network/base/address.hpp>
#include <ossia_export.h>

#include <ossia/detail/mutex.hpp>

namespace ossia
{
namespace net
{
class address_base;
}
class value;
struct time_value;

/**
 * @brief The mapper class
 *
 * Allows to map a value to another following a transfer function.
 * The driver address is where the input value is taken from;
 * The driven address is where the output value is sent to.
 */
class OSSIA_EXPORT mapper final : public ossia::time_process
{
public:
  mapper();
  mapper(ossia::Destination, ossia::Destination, ossia::behavior);

  ~mapper();

  void set_driver(ossia::Destination);
  void set_driven(ossia::Destination);
  void set_behavior(ossia::behavior b);

  void clean();

private:
  ossia::state_element offset(ossia::time_value, double pos) override;

  ossia::state_element state(ossia::time_value date, double pos) override;

  void start() override;
  void stop() override;
  void pause() override;
  void resume() override;

  static ossia::value
  compute_value(const ossia::value&, const ossia::behavior&);

  void driver_value_callback(ossia::value value);

  optional<ossia::Destination> m_driverAddress;
  ossia::behavior m_drive;

  optional<ossia::message> m_lastMessage;
  ossia::value m_valueToMap;
  mutable mutex_t m_valueToMapMutex;
  mutable mutex_t m_driverAddressMutex;

  optional<ossia::net::address_base::callback_index> m_callback;

  friend struct mapper_compute_visitor;
};
}
