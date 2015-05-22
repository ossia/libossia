#include "Editor/TimeConstraint.h"
#include "Editor/TimeValue.h"

#include "TTScore.h"

using namespace OSSIA;
using namespace std;

class JamomaTimeConstraint : public TimeConstraint
{
  
private:
  
  // Implementation specific
  shared_ptr<TimeNode> tnode;
  shared_ptr<State> st;
  shared_ptr<TimeProcess> element;
  
public:
  // Constructors, destructor, cloning
  JamomaTimeConstraint(TimeValue nominal, TimeValue min /*= nominal*/, TimeValue max /*= nominal*/)
  {
    // todo : we shouldn't init each time we create an object ...
    TTFoundationInit("/usr/local/jamoma/");
    TTModularInit("/usr/local/jamoma/");
    TTScoreInit("/usr/local/jamoma/");
  }
  
  JamomaTimeConstraint(const JamomaTimeConstraint * other)
  {}
  
  virtual ~JamomaTimeConstraint()
  {}
  
  virtual shared_ptr<TimeConstraint> clone() const override
  {
    return shared_ptr<TimeConstraint>(new JamomaTimeConstraint(this));
  }

  // Lecture
  virtual void play(bool log = false, string name = "") const override
  {}

  // Navigation
  virtual const shared_ptr<TimeNode> & getStartNode() const override
  {
    return tnode;
  }
  
  virtual const shared_ptr<TimeNode> & getEndNode() const override
  {
    return tnode;
  }

  // Accessors
  virtual const shared_ptr<State> & getStartState() const override
  {
    return st;
  }
  
  virtual void setStartState(shared_ptr<State>) override
  {}
  
  virtual const shared_ptr<State> & getEndState() const override
  {
    return st;
  }
  
  virtual void setEndState(shared_ptr<State>) override
  {}

};

shared_ptr<TimeConstraint> TimeConstraint::create(TimeValue nominal, TimeValue min /*= nominal*/, TimeValue max /*= nominal*/)
{
  return shared_ptr<TimeConstraint>(new JamomaTimeConstraint(nominal, min, max));
}
