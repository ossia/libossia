#include <ossia/editor/scenario/detail/Scenario_impl.hpp>
#include <ossia/detail/algorithms.hpp>
#include <unordered_map>
#include <iostream> //! \todo to remove. only here for debug purpose

# pragma mark -
# pragma mark Life cycle

namespace OSSIA
{
std::shared_ptr<Scenario> Scenario::create()
{
  return std::make_shared<impl::JamomaScenario>();
}

Scenario::~Scenario() = default;
}
