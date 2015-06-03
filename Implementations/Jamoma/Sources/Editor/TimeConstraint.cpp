#include "Editor/TimeConstraint.h"
#include "Editor/TimeValue.h"

using namespace OSSIA;
using namespace std;

class JamomaTimeConstraint : public TimeConstraint
{
  
private:
  
  // Implementation specific
  TimeValue             mDuration;
  TimeValue             mDurationMin;
  TimeValue             mDurationMax;
  
  shared_ptr<TimeNode>  mStartNode;
  shared_ptr<TimeNode>  mEndNode;
  shared_ptr<State>     mStartState;
  shared_ptr<State>     mEndState;
  
public:
  
# pragma mark -
# pragma mark Life cycle
  JamomaTimeConstraint(TimeValue nominal, TimeValue min /*= nominal*/, TimeValue max /*= nominal*/) :
  mDuration(nominal),
  mDurationMin(min),
  mDurationMax(max)
  {}
  
  JamomaTimeConstraint(const JamomaTimeConstraint * other)
  {}
  
  virtual ~JamomaTimeConstraint()
  {}
  
  virtual shared_ptr<TimeConstraint> clone() const override
  {
    return shared_ptr<TimeConstraint>(new JamomaTimeConstraint(this));
  }

# pragma mark -
# pragma mark Execution
  virtual void play(bool log = false, string name = "") const override
  {}

# pragma mark -
# pragma mark Accessors
  virtual const shared_ptr<TimeNode> & getStartNode() const override
  {
    return mStartNode;
  }
  
  virtual const shared_ptr<TimeNode> & getEndNode() const override
  {
    return mEndNode;
  }

  virtual const shared_ptr<State> & getStartState() const override
  {
    return mStartState;
  }
  
  virtual void setStartState(shared_ptr<State> startState) override
  {
    mStartState = startState;
  }
  
  virtual const shared_ptr<State> & getEndState() const override
  {
    return mEndState;
  }
  
  virtual void setEndState(shared_ptr<State> endState) override
  {
    mEndState = endState;
  }

};

shared_ptr<TimeConstraint> TimeConstraint::create(TimeValue nominal, TimeValue min /*= nominal*/, TimeValue max /*= nominal*/)
{
  return shared_ptr<TimeConstraint>(new JamomaTimeConstraint(nominal, min, max));
}
