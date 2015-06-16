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
#include "Editor/State.h"

#include "JamomaTimeNode.h"

using namespace OSSIA;
using namespace std;

class JamomaTimeNode;

class JamomaTimeEvent : public TimeEvent
{
  
private:
  
  // Implementation specific
  shared_ptr<State>         mState;
  shared_ptr<Expression>    mExpression;
  weak_ptr<JamomaTimeNode>  mTimeNode;
  
public:
  
# pragma mark -
# pragma mark Life cycle
  
  JamomaTimeEvent(shared_ptr<State> aState = nullptr, shared_ptr<Expression> anExpression = nullptr, shared_ptr<JamomaTimeNode> aTimeNode = nullptr);
  
  virtual ~JamomaTimeEvent();

# pragma mark -
# pragma mark Execution
  
  virtual void play(bool log = false, string name = "") const override;

# pragma mark -
# pragma mark Accessors
  
  virtual const shared_ptr<State> & getState() const override;
  
  virtual const shared_ptr<Expression> & getExpression() const override;
  
  virtual void setExpression(shared_ptr<Expression> expression) override;
};
