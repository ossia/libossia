#include <ossia/editor/expression/expression_bool.hpp>

namespace ossia
{
namespace expressions
{

expression_bool::~expression_bool() = default;

expression_bool::expression_bool(bool result) : mResult{result}
{
}
}
}
