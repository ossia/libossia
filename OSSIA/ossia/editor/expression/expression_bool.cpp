// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/editor/expression/expression_bool.hpp>

namespace ossia
{
namespace expressions
{

expression_bool::~expression_bool() = default;

expression_bool::expression_bool(bool result) : m_result{result}
{
}
}
}
