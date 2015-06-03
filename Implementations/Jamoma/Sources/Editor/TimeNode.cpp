#include "Editor/TimeNode.h"
#include "Editor/TimeValue.h"

using namespace OSSIA;
using namespace std;

class JamomaTimeNode : public TimeNode
{
  
private:
  
  // Implementation specific
  shared_ptr<TimeEvent> element;
  
public:
  // Life cycle
  JamomaTimeNode()
  {}
  
  JamomaTimeNode(const JamomaTimeNode * other)
  {}
  
  virtual ~JamomaTimeNode()
  {}
  
  virtual shared_ptr<TimeNode> clone() const override
  {
    return nullptr;//shared_ptr<TimeNode>(new JamomaTimeNode(this));
  }

  // Execution
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
