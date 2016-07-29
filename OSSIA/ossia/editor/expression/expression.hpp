#pragma once
#include <ossia/editor/expression/expression_atom.hpp>
#include <ossia/editor/expression/expression_bool.hpp>
#include <ossia/editor/expression/expression_composition.hpp>
#include <ossia/editor/expression/expression_not.hpp>
#include <ossia/editor/expression/expression_pulse.hpp>
#include <ossia/editor/expression/expression_fwd.hpp>


namespace ossia
{
namespace expressions
{
OSSIA_EXPORT bool evaluate(expression_base& e);
OSSIA_EXPORT void update(expression_base& e);

OSSIA_EXPORT bool operator==(const expression_base& lhs, const expression_base& rhs);
OSSIA_EXPORT bool operator!=(const expression_base& lhs, const expression_base& rhs);

OSSIA_EXPORT expression_callback_iterator addCallback(
    expression_base&,
    expression_result_callback);

OSSIA_EXPORT void removeCallback(
    expression_base&,
    expression_callback_iterator);
}
}
