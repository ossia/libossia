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
/**
 * @brief evaluate
 * @param e An expression
 * @return The truth value of the expression
 */
OSSIA_EXPORT bool evaluate(const expression_base& e);

/**
 * @brief update
 * @param e An expression
 *
 * Recursively asks an expression to query the addresses that it
 * refers, in order to check if the value has changed.
 */
OSSIA_EXPORT void update(const expression_base& e);

OSSIA_EXPORT inline bool evaluate(const expression_ptr& e)
{
  return evaluate(*e);
}
OSSIA_EXPORT inline void update(const expression_ptr& e)
{
  update(*e);
}

/**
 * @brief operator==(const expression_base& lhs, const expression_base& rhs) Compares two expressions.
 *
 * Two expressions are comparable if they have all the same terms.
 */
OSSIA_EXPORT bool
operator==(const expression_base& lhs, const expression_base& rhs);
OSSIA_EXPORT bool
operator!=(const expression_base& lhs, const expression_base& rhs);

/**
 * @brief add_callback Add a callback to an expression.
 */
OSSIA_EXPORT expression_callback_iterator
add_callback(expression_base&, expression_result_callback);


/**
 * @brief remove_callback Remove a callback to an expression.
 */
OSSIA_EXPORT void
remove_callback(expression_base&, expression_callback_iterator);

/**
 * @brief callback_count
 * @return Number of callbacks in an expression.
 */
OSSIA_EXPORT std::size_t callback_count(expression_base&);

/**
  \brief expression_true Convenience constant expression always evaluating to true.
 */
const expression_base expression_true{
    eggs::variants::in_place<expression_bool>, true};

/**
  \brief expression_true Convenience constant expression always evaluating to false.
 */
const expression_base expression_false{
    eggs::variants::in_place<expression_bool>, false};



/**
 * The following functions are factories used to create
 * expressions.
 */
OSSIA_EXPORT inline expression_ptr make_expression_true()
{
  return std::make_unique<expression_base>(
      eggs::variants::in_place<expression_bool>, true);
}
OSSIA_EXPORT inline expression_ptr make_expression_false()
{
  return std::make_unique<expression_base>(
      eggs::variants::in_place<expression_bool>, false);
}

template <typename... Args>
expression_ptr make_expression_atom(Args&&... args)
{
  return std::make_unique<expression_base>(
      eggs::variants::in_place<expression_atom>, std::forward<Args>(args)...);
}

template <typename... Args>
expression_ptr make_expression_bool(Args&&... args)
{
  return std::make_unique<expression_base>(
      eggs::variants::in_place<expression_bool>, std::forward<Args>(args)...);
}

template <typename... Args>
expression_ptr make_expression_composition(Args&&... args)
{
  return std::make_unique<expression_base>(
      eggs::variants::in_place<expression_composition>,
      std::forward<Args>(args)...);
}

template <typename... Args>
expression_ptr make_expression_not(Args&&... args)
{
  return std::make_unique<expression_base>(
      eggs::variants::in_place<expression_not>, std::forward<Args>(args)...);
}

template <typename... Args>
expression_ptr make_expression_pulse(Args&&... args)
{
  return std::make_unique<expression_base>(
      eggs::variants::in_place<expression_pulse>, std::forward<Args>(args)...);
}
}
}
