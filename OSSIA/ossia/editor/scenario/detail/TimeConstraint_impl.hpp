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

#include <ossia/editor/scenario/clock.hpp>
#include <ossia/editor/state/state.hpp>
#include <ossia/editor/scenario/time_constraint.hpp>
#include <ossia/editor/scenario/time_event.hpp>
#include <ossia/editor/scenario/time_process.hpp>
#include <ossia/editor/scenario/time_value.hpp>

#include "Clock_impl.hpp"
#include <ossia/editor/scenario/detail/TimeConstraint_impl.hpp>
#include "TimeEvent_impl.hpp"
#include "TimeNode_impl.hpp"
#include <ossia/editor/scenario/detail/TimeProcess_impl.hpp>

using namespace OSSIA;

using namespace std::placeholders;

namespace impl
{
class JamomaTimeConstraint final :
        public TimeConstraint,
        public JamomaClock,
        public std::enable_shared_from_this<JamomaTimeConstraint>
{

private:

# pragma mark -
# pragma mark Implementation specific

  TimeConstraint::ExecutionCallback   mCallback;

  std::shared_ptr<TimeEvent>               mStartEvent;
  std::shared_ptr<TimeEvent>               mEndEvent;

  TimeValue                           mDurationNominal{};
  TimeValue                           mDurationMin{};
  TimeValue                           mDurationMax{};

public:

# pragma mark -
# pragma mark Life cycle

  JamomaTimeConstraint(TimeConstraint::ExecutionCallback,
                       std::shared_ptr<TimeEvent>,
                       std::shared_ptr<TimeEvent>,
                       TimeValue = Infinite,
                       TimeValue = 0.,
                       TimeValue = Infinite);

  JamomaTimeConstraint(const JamomaTimeConstraint *);

  std::shared_ptr<TimeConstraint> clone() const override;

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

  const std::shared_ptr<TimeEvent> & getStartEvent() const override;

  const std::shared_ptr<TimeEvent> & getEndEvent() const override;

# pragma mark -
# pragma mark TimeProcesses

  void addTimeProcess(std::shared_ptr<TimeProcess>) override;

  void removeTimeProcess(std::shared_ptr<TimeProcess>) override;

private:
# pragma mark -
# pragma mark Implementation specific
  void ClockCallback(TimeValue position, TimeValue date, unsigned char droppedTicks);
  State make_state();
};
}
