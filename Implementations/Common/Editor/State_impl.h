#include "Editor/State.h"

using namespace OSSIA;
using namespace std;

namespace impl
{
class JamomaState final : public State
{

private:

# pragma mark -
# pragma mark Implementation specific

  shared_ptr<StateElement> element;

public:

# pragma mark -
# pragma mark Life cycle

  JamomaState();

  JamomaState(const JamomaState * other);

  shared_ptr<State> clone() const override;

  ~JamomaState();

# pragma mark -
# pragma mark Execution

  void launch() const override;

  StateElement::Type getType() const override final;
};
}
