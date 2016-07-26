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
#include <ossia/editor/state/message.hpp>
#include <ossia/editor/scenario/time_constraint.hpp>
#include <ossia/editor/scenario/time_node.hpp>
#include <ossia/editor/scenario/time_value.hpp>
#include <ossia/editor/value/value.hpp>

#include <ossia/network/v1/Address.hpp>
#include <ossia/editor/scenario/detail/TimeProcess_impl.hpp>

namespace impl
{
class JamomaAutomation final : public Automation, public JamomaTimeProcess
{

private:
#if 0
# pragma mark -
# pragma mark Implementation specific
#endif

  shared_ptr<Address>    mDrivenAddress;
  Value mDrive;
  Message mLastMessage;

public:
#if 0
# pragma mark -
# pragma mark Life cycle
#endif

  JamomaAutomation(shared_ptr<Address>, const Value&);

  JamomaAutomation(const JamomaAutomation&);

  shared_ptr<Automation> clone() const override;

  ~JamomaAutomation();
#if 0
# pragma mark -
# pragma mark Execution
#endif

  StateElement offset(TimeValue) override;

  StateElement state() override;
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

  const shared_ptr<Address> getDrivenAddress() const override;

  const Value& getDriving() const override;

#if 0
# pragma mark -
# pragma mark Implementation specific
#endif
  static Value computeValue(double, const Value&);
};
}
