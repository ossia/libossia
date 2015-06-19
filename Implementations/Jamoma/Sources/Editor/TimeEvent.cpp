#include "Editor/JamomaTimeEvent.h"
#include "Editor/State.h"

# pragma mark -
# pragma mark Life cycle

JamomaTimeEvent::JamomaTimeEvent(shared_ptr<TimeNode> aTimeNode,
                                 shared_ptr<Expression> anExpression) :
mTimeNode(aTimeNode),
mExpression(anExpression)
{
  mState = State::create();
}

JamomaTimeEvent::~JamomaTimeEvent()
{}

# pragma mark -
# pragma mark Execution

void JamomaTimeEvent::play(bool log, string name) const
{
  mState->launch();
  // note : what else ?
}

# pragma mark -
# pragma mark Edition

void JamomaTimeEvent::addState(const std::shared_ptr<State> state)
{
  // store a state if it is not already stored
  if (find(mState->stateElements().begin(),
           mState->stateElements().end(),
           state) == mState->stateElements().end())
  {
    mState->stateElements().push_back(state);
  }
}

void JamomaTimeEvent::removeState(const std::shared_ptr<State> state)
{
  mState->stateElements().erase(find(mState->stateElements().begin(), mState->stateElements().end(), state));
}

# pragma mark -
# pragma mark Accessors

const shared_ptr<TimeNode> & JamomaTimeEvent::getTimeNode() const
{
  return mTimeNode;
}
  
const shared_ptr<State> & JamomaTimeEvent::getState() const
{
  return mState;
}

const shared_ptr<Expression> & JamomaTimeEvent::getExpression() const
{
  return mExpression;
}

# pragma mark -
# pragma mark Implementation specific

void JamomaTimeEvent::setStatus(Status status)
{
  //! \todo check if our Scenario is running
  
  return statusApply(status);
}

void JamomaTimeEvent::wait()
{
  // filter repetitions here because the wait request is sent many times to reset a score
  if (mStatus == Status::WAITING)
    return;
  
  if (mRequestWait)
  {
    // don't log error in this case because if many processes shared the same end event
    // they can request end event waiting at the same time when start event waits (see in TTTimeProcess::EventStatusChanged)
    return;
  }
  
  mRequestWait = true;
  
  //! \todo if the event have no Scenario, update the status our self
  // return statusUpdate();
}

void JamomaTimeEvent::happen()
{
  if (mStatus != Status::WAITING && mStatus != Status::PENDING)
  {
    //! \todo log "TimeEvent::happen : is not waiting or pending (mStatus)");
    return;
  }
  
  if (mRequestHappen)
  {
    //! \todo log "TimeEvent::happen : this resquest is already registered");
    return;
  }
  
  mRequestHappen = true;
  
  //! \todo if the event have no Scenario, update the status our self
  // return statusUpdate();
}

void JamomaTimeEvent::dispose()
{
  // if the event is already happened or disposed : do nothing
  if (mStatus == Status::HAPPENED)
  {
    //! \todo log "TimeEvent::Dispose : is already happened"
    return;
  }
  
  if (mStatus == Status::DISPOSED)
  {
    //! \todo log "TimeEvent::Dispose : is already disposed"
    return;
  }
  
  if (mRequestDispose)
  {
    //! \todo log "TimeEvent::Dispose : this resquest is already registered"
    return;
  }
  
  mRequestDispose = true;
  
  //! \todo if the event have no Scenario, update the status our self
  // return statusUpdate();
}

void JamomaTimeEvent::statusUpdate()
{
  // if there is a request to make the event to wait
  if (mRequestWait)
  {
    mRequestWait = false;
    return statusApply(Status::WAITING);
  }
  
  // if there is a request to make the event happen
  if (mRequestHappen)
  {
    mRequestHappen = false;
    
    mState->launch();
    
    return statusApply(Status::HAPPENED);
  }
  
  // if there is a request to dispose the event
  if (mRequestDispose)
  {
    mRequestDispose = false;
    return statusApply(Status::DISPOSED);
  }
  
  // any event with attached constraints
  if (previousTimeConstraints().size() != 0)
  {
    // a conditioned event becomes pending when all attached constraints have reached their minimal duration bound
    if (mExpression &&
        mStatus == Status::WAITING &&
        mMinReachedConstraintsCounter == previousTimeConstraints().size())
    {
      return statusApply(Status::PENDING);
    }
    
    // an event is disposed when all attached processes are disposed
    if (mStatus == Status::WAITING &&
        mDisposedConstraintsCounter == previousTimeConstraints().size())
    {
      return statusApply(Status::DISPOSED);
    }
    
    // when all attached constraints are ended or disposed
    if (mEndedConstraintsCounter + mDisposedConstraintsCounter == previousTimeConstraints().size())
    {
      // a conditioned pending event forces its condition to apply its default case
      if (mExpression &&
          mStatus == Status::PENDING)
      {
        return; //! \todo but this is not the way it works : mCondition.send("Default");
      }
      // a non conditioned waiting event happens
      else if (!mExpression &&
               mStatus == Status::WAITING)
      {
        mState->launch();
        
        return statusApply(Status::HAPPENED);
      }
    }
  }
  
  // waiting event with no attached constraint
  else if (mStatus == Status::WAITING)
  {
    // set conditionned event as pending
    if (mExpression)
    {
      return statusApply(Status::PENDING);
    }
    // or make none conditioned event to happen at its date
    //! \todo if the event have Scenario
    /*
    else if (mContainer.valid())
    {
      TTValue v;
      mContainer.get("date", v);
      TTUInt32 containerDate = v[0];
      
      if (mDate <= containerDate)
      {
        mState->launch();
        return statusApply(HAPPENED);
      }
    }
     */
  }
  
  return;
}

void JamomaTimeEvent::statusApply(Status status)
{
  Status lastStatus = mStatus;
  
  // set status
  mStatus = status;
  
  // log error if conflicted request are detected
  if (mRequestWait + mRequestHappen + mRequestDispose > 1)
    ;//! \todo log "TimeEvent::statusApply : at least mRequestWait + mRequestHappen + mRequestDispose requests have been received"
  
  // reset requests
  mRequestWait = false;
  mRequestHappen = false;
  mRequestDispose = false;
  
  // reset counts if the event is not pending
  if (mStatus != Status::PENDING)
  {
    mMinReachedConstraintsCounter = 0;
    mEndedConstraintsCounter = 0;
    mDisposedConstraintsCounter = 0;
  }
  
  // filter repetitions
  if (lastStatus == mStatus)
  {
    // log error only for non waiting status repetition
    if (mStatus != Status::WAITING)
       ;//! \todo log "TimeEvent::applyStatus : new status equals last status (mStatus)"
    
    return;
  }

  //! \todo send notification
  /*
  TTValue v = TTObject(this);
  v.append(mStatus);
  v.append(lastStatus);
  sendNotification(kTTSym_EventStatusChanged, v);
  */
}
