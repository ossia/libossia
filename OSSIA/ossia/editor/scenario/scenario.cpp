#include <ossia/editor/scenario/detail/Scenario_impl.hpp>
namespace ossia
{
std::shared_ptr<scenario> scenario::create()
{
  return std::make_shared<detail::scenario_impl>();
}

scenario::~scenario() = default;
}
