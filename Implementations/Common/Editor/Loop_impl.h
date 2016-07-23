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

#include "Editor/Loop.h"
#include "Editor/TimeConstraint.h"
#include "Editor/TimeEvent.h"
#include "Editor/TimeValue.h"

#include "TimeConstraint_impl.h"
#include "TimeProcess_impl.h"

#include <cmath>

using namespace OSSIA;
using namespace std;
using namespace std::placeholders;

namespace impl
{
class JamomaLoop final : public Loop, public virtual JamomaTimeProcess
{

private:

# pragma mark -
# pragma mark Implementation specific

  shared_ptr<TimeNode>              mPatternStartNode;
  TimeEvent::ExecutionCallback      mPatternStartEventCallback;

  shared_ptr<TimeNode>              mPatternEndNode;
  TimeEvent::ExecutionCallback      mPatternEndEventCallback;

  shared_ptr<TimeConstraint>        mPatternConstraint;
  TimeConstraint::ExecutionCallback mPatternConstraintCallback;

  shared_ptr<State>                 mCurrentState;      // an internal State to return on state call
  shared_ptr<State>                 mOffsetState;       // an internal State built when offset is called

public:

# pragma mark -
# pragma mark Life cycle

  JamomaLoop(TimeValue,
             TimeConstraint::ExecutionCallback,
             TimeEvent::ExecutionCallback,
             TimeEvent::ExecutionCallback);

  JamomaLoop(const JamomaLoop *);

  shared_ptr<Loop> clone() const override;

  ~JamomaLoop();

# pragma mark -
# pragma mark Execution

  shared_ptr<StateElement> offset(TimeValue) override;

  shared_ptr<StateElement> state() override;

# pragma mark -
# pragma mark Execution - Implementation specific

  void start() override;
  void stop() override;
  void pause() override;
  void resume() override;

# pragma mark -
# pragma mark Accessors

  const shared_ptr<TimeConstraint> getPatternTimeConstraint() const override;

  const shared_ptr<TimeNode> getPatternStartTimeNode() const override;

  const shared_ptr<TimeNode> getPatternEndTimeNode() const override;

private:

# pragma mark -
# pragma mark Implementation specific

  void PatternConstraintCallback(
      TimeValue,
      TimeValue,
      const std::shared_ptr<State>&);

  void PatternStartEventCallback(TimeEvent::Status);

  void PatternEndEventCallback(TimeEvent::Status);
};
}
