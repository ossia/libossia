#pragma once

#include <ossia/editor/automation/automation.hpp>
#include <ossia/editor/curve/curve.hpp>
#include <ossia/editor/scenario/time_constraint.hpp>
#include <ossia/editor/scenario/time_node.hpp>
#include <ossia/editor/scenario/time_value.hpp>
#include <ossia/editor/state/state_element.hpp>
#include <ossia/editor/value/value.hpp>

#include <ossia/editor/scenario/detail/TimeProcess_impl.hpp>
#include <ossia/network/base/address.hpp>

namespace detail
{
class automation_impl final : public ossia::automation,
                              public time_process_impl
{

private:
  ossia::net::address_base& mDrivenAddress;
  ossia::value mDrive;
  ossia::message mLastMessage;

public:
  automation_impl(ossia::net::address_base&, const ossia::value&);
  automation_impl(const automation_impl&);

  ~automation_impl();

  static ossia::value computeValue(double, const ossia::value&);

private:
  ossia::state_element offset(ossia::time_value) override;
  ossia::state_element state() override;

  void start() override;
  void stop() override;
  void pause() override;
  void resume() override;

  const ossia::net::address_base& getDrivenAddress() const override;
  const ossia::value& getDriving() const override;
};
}
