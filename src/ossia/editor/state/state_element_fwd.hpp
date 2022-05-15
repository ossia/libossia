#pragma once
#include <ossia/detail/config.hpp>

#include <ossia/detail/nullable_variant.hpp>
/**
 * \file state_element_fwd.hpp
 */
namespace ossia
{
struct message;
struct piecewise_message;
template <std::size_t N>
struct piecewise_vec_message;
class state;

/**
 * \typedef state_element
 *
 * A variant of elements that are meant to be triggered during
 * execution of a score.
 *
 * \see \ref message
 * \see \ref state
 * \see state_element.hpp
 */
using state_element = ossia::nullable_variant<
    message, state, piecewise_message, piecewise_vec_message<2>,
    piecewise_vec_message<3>, piecewise_vec_message<4>>;

template <std::size_t N>
using state_element_by_index =
    typename ossia::variant_element_t<N, ossia::state_element>;
}
