#include <ossia/editor/expression/detail/ExpressionPulse_impl.hpp>
#include <ossia/editor/value/value.hpp>

# pragma mark -
# pragma mark Life cycle

namespace OSSIA
{
  std::shared_ptr<ExpressionPulse> ExpressionPulse::create(const Destination& destination)
  {
    return std::make_shared<impl::JamomaExpressionPulse>(destination);
  }

  ExpressionPulse::~ExpressionPulse() = default;
}
