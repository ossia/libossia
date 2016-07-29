#include <ossia/editor/scenario/detail/Scenario_impl.hpp>
namespace ossia
{
std::shared_ptr<scenario> scenario::create()
{
  return std::make_shared<impl::JamomaScenario>();
}

scenario::~scenario() = default;
}
