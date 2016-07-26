/*!
 * \file Loop_impl.h
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

#include <ossia/editor/loop/loop.hpp>
#include <ossia/editor/scenario/time_constraint.hpp>
#include <ossia/editor/scenario/time_event.hpp>
#include <ossia/editor/scenario/time_value.hpp>

#include <ossia/editor/scenario/detail/TimeConstraint_impl.hpp>
#include <ossia/editor/scenario/detail/TimeProcess_impl.hpp>

#include <cmath>

using namespace OSSIA;

using namespace std::placeholders;

namespace impl
{
class JamomaLoop final : public Loop, public virtual JamomaTimeProcess
{

private:

# pragma mark -
# pragma mark Implementation specific

  std::shared_ptr<TimeNode>              mPatternStartNode;
  TimeEvent::ExecutionCallback      mPatternStartEventCallback;

  std::shared_ptr<TimeNode>              mPatternEndNode;
  TimeEvent::ExecutionCallback      mPatternEndEventCallback;

  std::shared_ptr<TimeConstraint>        mPatternConstraint;
  TimeConstraint::ExecutionCallback mPatternConstraintCallback;

  State                             mCurrentState;      // an internal State to return on state call
  State                             mOffsetState;       // an internal State built when offset is called

public:

# pragma mark -
# pragma mark Life cycle

  JamomaLoop(TimeValue,
             TimeConstraint::ExecutionCallback,
             TimeEvent::ExecutionCallback,
             TimeEvent::ExecutionCallback);

  JamomaLoop(const JamomaLoop *);

  std::shared_ptr<Loop> clone() const override;

  ~JamomaLoop();

# pragma mark -
# pragma mark Execution

  StateElement offset(TimeValue) override;

  StateElement state() override;

# pragma mark -
# pragma mark Execution - Implementation specific

  void start() override;
  void stop() override;
  void pause() override;
  void resume() override;

# pragma mark -
# pragma mark Accessors

  const std::shared_ptr<TimeConstraint> getPatternTimeConstraint() const override;

  const std::shared_ptr<TimeNode> getPatternStartTimeNode() const override;

  const std::shared_ptr<TimeNode> getPatternEndTimeNode() const override;

private:

# pragma mark -
# pragma mark Implementation specific

  void PatternConstraintCallback(
      TimeValue,
      TimeValue,
      const State&);

  void PatternStartEventCallback(TimeEvent::Status);

  void PatternEndEventCallback(TimeEvent::Status);
};
}
