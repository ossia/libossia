/*!
 * \file TimeConstraint_impl.h
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

#include "Editor/Clock.h"
#include "Editor/State.h"
#include "Editor/TimeConstraint.h"
#include "Editor/TimeEvent.h"
#include "Editor/TimeProcess.h"
#include "Editor/TimeValue.h"

#include "Clock_impl.h"
#include "TimeConstraint_impl.h"
#include "TimeEvent_impl.h"
#include "TimeNode_impl.h"
#include "TimeProcess_impl.h"

using namespace OSSIA;
using namespace std;
using namespace std::placeholders;

namespace impl
{
class JamomaTimeConstraint final :
        public TimeConstraint,
        public JamomaClock,
        public enable_shared_from_this<JamomaTimeConstraint>
{

private:

# pragma mark -
# pragma mark Implementation specific

  TimeConstraint::ExecutionCallback   mCallback;

  State                               mCurrentState;    // an internal State to update at each tick of the clock
  State                               mOffsetState;     // an internal State built when offset is called

  shared_ptr<TimeEvent>               mStartEvent;
  shared_ptr<TimeEvent>               mEndEvent;

  TimeValue                           mDurationNominal{};
  TimeValue                           mDurationMin{};
  TimeValue                           mDurationMax{};

public:

# pragma mark -
# pragma mark Life cycle

  JamomaTimeConstraint(TimeConstraint::ExecutionCallback,
                       shared_ptr<TimeEvent>,
                       shared_ptr<TimeEvent>,
                       TimeValue = Infinite,
                       TimeValue = 0.,
                       TimeValue = Infinite);

  JamomaTimeConstraint(const JamomaTimeConstraint *);

  shared_ptr<TimeConstraint> clone() const override;

  ~JamomaTimeConstraint();

# pragma mark -
# pragma mark Execution

  void start() final override;

  void stop() final override;

  State offset(TimeValue) override;

  State state() override;

  void pause() final override;

  void resume() final override;

# pragma mark -
# pragma mark Accessors

  void setCallback(ExecutionCallback) override;

  const TimeValue & getDurationNominal() const override;

  TimeConstraint & setDurationNominal(TimeValue) override;

  const TimeValue & getDurationMin() const override;

  TimeConstraint & setDurationMin(TimeValue) override;

  const TimeValue & getDurationMax() const override;

  TimeConstraint & setDurationMax(TimeValue) override;

  const shared_ptr<TimeEvent> & getStartEvent() const override;

  const shared_ptr<TimeEvent> & getEndEvent() const override;

# pragma mark -
# pragma mark TimeProcesses

  void addTimeProcess(std::shared_ptr<TimeProcess>) override;

  void removeTimeProcess(std::shared_ptr<TimeProcess>) override;

private:
# pragma mark -
# pragma mark Implementation specific
  void ClockCallback(TimeValue position, TimeValue date, unsigned char droppedTicks);
};
}
