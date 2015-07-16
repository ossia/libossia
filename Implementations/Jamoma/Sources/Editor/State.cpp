#include "Editor/State.h"

using namespace OSSIA;
using namespace std;

class JamomaState : public State
{
  
private:
  
# pragma mark -
# pragma mark Implementation specific
  
  shared_ptr<StateElement> element;
  
public:
  
# pragma mark -
# pragma mark Life cycle
  
  JamomaState()
  {}
  
  JamomaState(const JamomaState * other)
  {}
  
  virtual shared_ptr<State> clone() const override
  {
    return make_shared<JamomaState>(this);
  }

  virtual ~JamomaState()
  {}

# pragma mark -
# pragma mark Execution
  
  virtual void launch() const override
  {
    for (const auto& element : stateElements())
      element->launch();
  }
};

namespace OSSIA
{
  shared_ptr<State> State::create()
  {
    return make_shared<JamomaState>();
  }
}