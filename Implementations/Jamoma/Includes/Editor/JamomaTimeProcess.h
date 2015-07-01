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

#include "Editor/State.h"
#include "Editor/TimeProcess.h"

using namespace OSSIA;
using namespace std;

class JamomaTimeConstraint;

class JamomaTimeProcess
{
  
  friend JamomaTimeConstraint;
  
protected:
  
# pragma mark -
# pragma mark Implementation specific
  
  std::shared_ptr<TimeConstraint> mParent;
  
  TimeProcess::ExecutionCallback  mCallback;
  
  shared_ptr<State>               mStartState;
  shared_ptr<State>               mEndState;
  shared_ptr<State>               mCurrentState;  // an internal State to update at each tick of the clock
  
  shared_ptr<Clock>               mClock;

public:
  
# pragma mark -
# pragma mark Life cycle
  
  /*! constructor
  \param #TimeProcess::ExecutionCallback the function to use to be notified at each step
  \param std::shared_ptr<#State> the state at start
  \param std::shared_ptr<#State> the state at end
  \param std::shared_ptr<#Clock> the clock use to run */
  JamomaTimeProcess(TimeProcess::ExecutionCallback,
                    shared_ptr<State>,
                    shared_ptr<State>,
                    shared_ptr<Clock>);
};
