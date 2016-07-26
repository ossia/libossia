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

#include <ossia/network/v1/Address.hpp>
#include <ossia/editor/scenario/detail/TimeProcess_impl.hpp>

namespace impl
{
class JamomaAutomation final :
    public OSSIA::Automation,
    public JamomaTimeProcess
{

private:
#if 0
# pragma mark -
# pragma mark Implementation specific
#endif

  std::shared_ptr<OSSIA::Address> mDrivenAddress;
  OSSIA::Value mDrive;
  OSSIA::Message mLastMessage;

public:
#if 0
# pragma mark -
# pragma mark Life cycle
#endif

  JamomaAutomation(std::shared_ptr<OSSIA::Address>, const OSSIA::Value&);

  JamomaAutomation(const JamomaAutomation&);

  std::shared_ptr<Automation> clone() const override;

  ~JamomaAutomation();
#if 0
# pragma mark -
# pragma mark Execution
#endif

  OSSIA::StateElement offset(OSSIA::TimeValue) override;

  OSSIA::StateElement state() override;
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

  const std::shared_ptr<OSSIA::Address> getDrivenAddress() const override;

  const OSSIA::Value& getDriving() const override;

#if 0
# pragma mark -
# pragma mark Implementation specific
#endif
  static OSSIA::Value computeValue(double, const OSSIA::Value&);
};
}
