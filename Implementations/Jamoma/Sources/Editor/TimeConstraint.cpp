#include "Editor/TimeConstraint.h"

using namespace OSSIA;
using namespace std;

class JamomaTimeConstraint : public TimeConstraint
{
  
private:
  // Implementation Specific
  shared_ptr<TimeNode> tnode;
  shared_ptr<State> st;
  shared_ptr<TimeProcess> element;
  
public:
  // Constructors, destructor, cloning
  JamomaTimeConstraint(TimeValue nominal, TimeValue min = nominal, TimeValue max = nominal)
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
  
  virtual shared_ptr<TimeProcess> & front() override
  {
    return element;
  }
  
  virtual const shared_ptr<TimeProcess> & front() const override
  {
    return element;
  }
  
  virtual shared_ptr<TimeProcess> & back() override
  {
    return element;
  }
  
  virtual const shared_ptr<TimeProcess> & back() const override
  {
    return element;
  }
  
  virtual iterator insert(const_iterator where, shared_ptr<TimeProcess> what) override
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

shared_ptr<TimeConstraint> TimeConstraint::create(TimeValue nominal, TimeValue min = nominal, TimeValue max = nominal)
{
  return shared_ptr<TimeConstraint>(new JamomaTimeConstraint(nominal, min, max));
}
