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

  // Std container
  virtual iterator begin() override
  {
    return iterator();
  }
  
  virtual iterator end() override
  {
    return iterator();
  }
  
  virtual const_iterator cbegin() const override
  {
    return iterator();
  }
  
  virtual const_iterator cend() const override
  {
    return iterator();
  }
  
  virtual size_type size() const override
  {
    return size_type();
  }
  
  virtual bool empty() const override
  {
    return true;
  }
  
  virtual shared_ptr<TimeEvent> & front() override
  {
    return element;
  }
  
  virtual const shared_ptr<TimeEvent> & front() const override
  {
    return element;
  }
  
  virtual shared_ptr<TimeEvent> & back() override
  {
    return element;
  }
  
  virtual const shared_ptr<TimeEvent> & back() const override
  {
    return element;
  }
  
  virtual iterator emplace(const_iterator where, shared_ptr<State> st, shared_ptr<Expression> exp) override
  {
    return iterator();
  }
  
  virtual iterator erase(const_iterator which) override
  {
    return iterator();
  }
  
  virtual iterator erase(const_iterator first, const_iterator last) override
  {
    return iterator();
  }

};

shared_ptr<TimeNode> TimeNode::create()
{
  return nullptr;//shared_ptr<TimeNode>(new JamomaTimeNode());
}
