/*!
 * \file JamomaTimeProcess.h
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
#include "Editor/TimeProcess.h"

using namespace OSSIA;
using namespace std;

class JamomaTimeProcess : public virtual TimeProcess
{

protected:

# pragma mark -
# pragma mark Implementation specific

  std::shared_ptr<TimeConstraint> mParent;

  shared_ptr<State>               mStartState;
  shared_ptr<State>               mEndState;

  TimeValue                       mLastPosition;  // used to filter multiple state calls at the same time

public:

# pragma mark -
# pragma mark Life cycle

  JamomaTimeProcess(shared_ptr<State>,
                    shared_ptr<State>);

# pragma mark -
# pragma mark Execution - Implementation specific

  virtual void start() = 0;
  virtual void stop() = 0;
  virtual void pause() = 0;
  virtual void resume() = 0;

# pragma mark -
# pragma mark Accessors

  const shared_ptr<State> & getStartState() const override;

  const shared_ptr<State> & getEndState() const override;

  const shared_ptr<TimeConstraint> & getParentTimeConstraint() const override;

# pragma mark -
# pragma mark Implementation specific

  void setParentTimeConstraint(const shared_ptr<TimeConstraint>);
};
