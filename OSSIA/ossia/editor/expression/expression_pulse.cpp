#include "Editor/ExpressionPulse_impl.hpp"
#include <Editor/Value/Value.h>

# pragma mark -
# pragma mark Life cycle

namespace OSSIA
{
  shared_ptr<ExpressionPulse> ExpressionPulse::create(const Destination& destination)
  {
    return make_shared<impl::JamomaExpressionPulse>(destination);
  }

  ExpressionPulse::~ExpressionPulse() = default;
}
