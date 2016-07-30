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
OSSIA_EXPORT bool evaluate(const expression_base& e);
OSSIA_EXPORT void update(const expression_base& e);

OSSIA_EXPORT inline bool evaluate(const expression_ptr& e) { return evaluate(*e); }
OSSIA_EXPORT inline void update(const expression_ptr& e)  { update(*e); }

OSSIA_EXPORT bool operator==(const expression_base& lhs, const expression_base& rhs);
OSSIA_EXPORT bool operator!=(const expression_base& lhs, const expression_base& rhs);

OSSIA_EXPORT expression_callback_iterator add_callback(
    expression_base&,
    expression_result_callback);

OSSIA_EXPORT void remove_callback(
    expression_base&,
    expression_callback_iterator);

OSSIA_EXPORT void remove_callback(
    expression_base&,
    expression_callback_iterator);


const expression_base expression_true{eggs::variants::in_place<expression_bool>, true};
const expression_base expression_false{eggs::variants::in_place<expression_bool>, false};

OSSIA_EXPORT inline expression_ptr make_expression_true() {
  return std::make_unique<expression_base>(eggs::variants::in_place<expression_bool>, true); }
OSSIA_EXPORT inline expression_ptr make_expression_false() {
  return std::make_unique<expression_base>(eggs::variants::in_place<expression_bool>, false); }

template<typename... Args>
expression_ptr make_expression_atom(Args&&... args)
{
  return std::make_unique<expression_base>(
        eggs::variants::in_place<expression_atom>,
        std::forward<Args>(args)...);
}

template<typename... Args>
expression_ptr make_expression_bool(Args&&... args)
{
  return std::make_unique<expression_base>(
        eggs::variants::in_place<expression_bool>,
        std::forward<Args>(args)...);
}

template<typename... Args>
expression_ptr make_expression_composition(Args&&... args)
{
  return std::make_unique<expression_base>(
        eggs::variants::in_place<expression_composition>,
        std::forward<Args>(args)...);
}

template<typename... Args>
expression_ptr make_expression_not(Args&&... args)
{
  return std::make_unique<expression_base>(
        eggs::variants::in_place<expression_not>,
        std::forward<Args>(args)...);
}

template<typename... Args>
expression_ptr make_expression_pulse(Args&&... args)
{
  return std::make_unique<expression_base>(
        eggs::variants::in_place<expression_pulse>,
        std::forward<Args>(args)...);
}


}
}
