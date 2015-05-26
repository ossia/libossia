#include "Editor/TimeNode.h"
#include "Editor/TimeValue.h"

#include "TTScore.h"

using namespace OSSIA;
using namespace std;

class JamomaTimeNode : public TimeNode
{
  
private:
  
  // Implementation specific
  shared_ptr<TimeEvent> element;
  
public:
  // Constructors, destructor, cloning
  JamomaTimeNode()
  {
    // todo : we shouldn't init each time we create an object ...
    TTFoundationInit("/usr/local/jamoma/");
    TTModularInit("/usr/local/jamoma/");
    TTScoreInit("/usr/local/jamoma/");
  }
  
  JamomaTimeNode(const JamomaTimeNode * other)
  {}
  
  virtual ~JamomaTimeNode()
  {}
  
  virtual shared_ptr<TimeNode> clone() const override
  {
    return nullptr;//shared_ptr<TimeNode>(new JamomaTimeNode(this));
  }

  // Lecture
  virtual void play(bool log = false, string name = "") const override
  {}

  // Accessors
  virtual TimeValue getDate() const override
  {
    return TimeValue();
  }
  
  virtual TimeValue getSimultaneityMargin() const override
  {
    return TimeValue();
  }
  
  virtual void setSimultaneityMargin(TimeValue) override
  {}

};

shared_ptr<TimeNode> TimeNode::create()
{
  return nullptr;//shared_ptr<TimeNode>(new JamomaTimeNode());
}
