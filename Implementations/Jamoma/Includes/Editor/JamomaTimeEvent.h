/*!
 * \file JamomaTimeEvent.h
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

#include "Editor/TimeEvent.h"
#include "Editor/Expression.h"
#include "Editor/State.h"

using namespace OSSIA;
using namespace std;

class JamomaTimeEvent : public TimeEvent
{
  
private:
  
  // Implementation specific
  shared_ptr<State>           mState;
  shared_ptr<Expression>      mExpression;

public:
  
# pragma mark -
# pragma mark Life cycle
  
  JamomaTimeEvent(shared_ptr<Expression> anExpression = nullptr);
  
  ~JamomaTimeEvent();

# pragma mark -
# pragma mark Execution
  
  void play(bool log = false, string name = "") const override;
  
# pragma mark -
# pragma mark Edition
  
  void addState(const std::shared_ptr<State>) override;
  
  void removeState(const std::shared_ptr<State>) override;

# pragma mark -
# pragma mark Accessors
  
  const shared_ptr<State> & getState() const override;
  
  const shared_ptr<Expression> & getExpression() const override;
};
