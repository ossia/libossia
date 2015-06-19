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
  
public:
  
# pragma mark -
# pragma mark Enumerations
  
  /*! event status */
  enum class Status
  {
    WAITING,
    PENDING,
    HAPPENED,
    DISPOSED
  };
  
private:
  
# pragma mark -
# pragma mark Implementation specific
  
  // Implementation specific
  shared_ptr<TimeNode>    mTimeNode;
  shared_ptr<State>       mState;
  shared_ptr<Expression>  mExpression;
  
  Status                  mStatus;

  uint32_t                mMinReachedConstraintsCounter;  /// how many constraints have reached their minimun duration bound ?
  uint32_t                mEndedConstraintsCounter;       /// how many constraints have ended ?
  uint32_t                mDisposedConstraintsCounter;    /// how many constraints have been disposed ?
  
  bool                    mRequestWait;                   /// an internal flag to know if there is a request to make the event to wait
  bool                    mRequestHappen;                 /// an internal flag to know if there is a request to make the event happen
  bool                    mRequestDispose;                /// an internal flag to know if there is a request to dispose the event
  
public:
  
# pragma mark -
# pragma mark Life cycle
  
  JamomaTimeEvent(shared_ptr<TimeNode> aTimeNode = nullptr,
                  shared_ptr<Expression> anExpression = nullptr);
  
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
  
  const shared_ptr<TimeNode> & getTimeNode() const override;
  
  const shared_ptr<State> & getState() const override;
  
  const shared_ptr<Expression> & getExpression() const override;

# pragma mark -
# pragma mark Implementation specific
  
private:
  
  /*! set status directly
   \details this is usefull to reset event at precise status but it couldn't not be used when our Scenario is running
   \param Status status to set */
  void setStatus(Status);
  
  /*! request to make the event to wait
   \details the request will be apply on next statusUpdate call */
  void wait();
  
  /*! request to make the event happen
   \details the request will be apply on next statusUpdate call */
  void happen();
  
  /*! request to make the event not happen
   \details the request will be apply on next statusUpdate call */
  void dispose();
  
  /*! apply request or update event status depending on attached processes statement
   \details this methods should only be called by our Scenario or the event itself */
  void statusUpdate();
  
  /*! internal method to apply new status and notify observers
   \param Status status to apply */
  void statusApply(Status);
  
};
