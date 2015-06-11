#include "Editor/TimeProcess.h"
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
#include "Editor/TimeValue.h"
#include "Editor/Clock.h"

using namespace OSSIA;
using namespace std;

class JamomaTimeProcess : public TimeProcess
{
  
private:
  
  // Implementation specific
  shared_ptr<TimeEvent> event;
  shared_ptr<State> state;
  shared_ptr<Clock> clock;
  
public:
  
# pragma mark -
# pragma mark Navigation
  
  virtual const shared_ptr<TimeEvent> & getStartEvent() const override;
  
  virtual const shared_ptr<TimeEvent> & getEndEvent() const override;

# pragma mark -
# pragma mark Accessors
  
  virtual TimeValue getLength() const override;
  
  virtual void setLength(TimeValue) override;
  
  virtual const shared_ptr<State> & getStartState() const override;
  
  virtual void setStartState(shared_ptr<State>) override;
  
  virtual const shared_ptr<State> & getEndState() const override;
  
  virtual void setEndState(shared_ptr<State>) override;
  
  virtual const shared_ptr<Clock> & getClock() const override;
  
  virtual void setClock(shared_ptr<Clock>) override;
};
