#include <ossia/editor/expression/detail/ExpressionPulse_impl.hpp>
#include <ossia/editor/value/destination.hpp>
namespace ossia
{
namespace expressions
{
std::shared_ptr<expression_pulse> expression_pulse::create(const Destination& destination)
{
  return std::make_shared<::impl::JamomaExpressionPulse>(destination);
}

expression_pulse::~expression_pulse() = default;
}
}
