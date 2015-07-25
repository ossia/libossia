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

class JamomaTimeConstraint;

class JamomaTimeProcess
{
  
  friend JamomaTimeConstraint;
  
protected:
  
# pragma mark -
# pragma mark Implementation specific
  
  std::shared_ptr<TimeConstraint> mParent;
  
  shared_ptr<State>               mStartState;
  shared_ptr<State>               mEndState;
  shared_ptr<State>               mCurrentState;  // an internal State to return on state call

public:
  
# pragma mark -
# pragma mark Life cycle
  
  JamomaTimeProcess(shared_ptr<State>,
                    shared_ptr<State>);
};
