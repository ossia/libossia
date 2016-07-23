#include "Editor/Scenario_impl.h"
#include <Misc/Util.h>
#include <unordered_map>
#include <iostream> //! \todo to remove. only here for debug purpose

# pragma mark -
# pragma mark Life cycle

namespace OSSIA
{
  shared_ptr<Scenario> Scenario::create()
  {
    return make_shared<impl::JamomaScenario>();
  }

  Scenario::~Scenario() = default;
}
