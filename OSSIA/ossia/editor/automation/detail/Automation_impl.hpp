/*!
 * \file Automation_impl.h
 *
 * \brief
 *
 * \details
 *
 * \author Théo de la Hogue
 *
 * \copyright This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#pragma once

#include <ossia/editor/automation/automation.hpp>
#include <ossia/editor/curve/curve.hpp>
#include <ossia/editor/state/state_element.hpp>
#include <ossia/editor/scenario/time_constraint.hpp>
#include <ossia/editor/scenario/time_node.hpp>
#include <ossia/editor/scenario/time_value.hpp>
#include <ossia/editor/value/value.hpp>

#include <ossia/network/base/address.hpp>
#include <ossia/editor/scenario/detail/TimeProcess_impl.hpp>

namespace impl
{
class JamomaAutomation final :
    public ossia::automation,
    public JamomaTimeProcess
{

private:
#if 0
# pragma mark -
# pragma mark Implementation specific
#endif

  ossia::net::address& mDrivenAddress;
  ossia::value mDrive;
  ossia::Message mLastMessage;

public:
#if 0
# pragma mark -
# pragma mark Life cycle
#endif

  JamomaAutomation(ossia::net::address&, const ossia::value&);

  JamomaAutomation(const JamomaAutomation&);

  ~JamomaAutomation();
#if 0
# pragma mark -
# pragma mark Execution
#endif

  ossia::StateElement offset(ossia::time_value) override;

  ossia::StateElement state() override;
#if 0
# pragma mark -
# pragma mark Execution - Implementation specific
#endif

  void start() override;
  void stop() override;
  void pause() override;
  void resume() override;
#if 0
# pragma mark -
# pragma mark Accessors
#endif

  const ossia::net::address& getDrivenAddress() const override;

  const ossia::value& getDriving() const override;

#if 0
# pragma mark -
# pragma mark Implementation specific
#endif
  static ossia::value computeValue(double, const ossia::value&);
};
}
