#pragma once
#include <eggs/variant.hpp>
namespace ossia
{
struct message;
class state;
class custom_state;

/**
 * \typedef state_element
 *
 * A variant of elements that are meant to be triggered during
 * execution of a score.
 *
 * \see \ref message
 * \see \ref state
 * \see \ref custom_state
 * \see state_element.hpp
 */
using state_element = eggs::variant<message, state, custom_state>;
}
