#include "Editor/State.h"

using namespace OSSIA;
using namespace std;

class JamomaState : public State
{
  
private:
  
  // Implementation specific
  shared_ptr<StateElement> element;
  
public:
  
# pragma mark -
# pragma mark Life cycle
  
  JamomaState()
  {}
  
  JamomaState(const JamomaState * other)
  {}
  
  virtual ~JamomaState()
  {}
  
  virtual shared_ptr<State> clone() const override
  {
    return shared_ptr<State>(new JamomaState(this));
  }

# pragma mark -
# pragma mark Execution
  
  virtual void launch() const override
  {
    for (const auto& element : stateElements())
      element->launch();
  }
};

shared_ptr<State> State::create()
{
  return shared_ptr<State>(new JamomaState);
}