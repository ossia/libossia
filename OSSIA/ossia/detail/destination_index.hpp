#pragma once
#include <ossia/detail/config.hpp>
#include <chobo/static_vector.hpp>
#include <iterator>

/**
 * \file destination_index.hpp
 */

namespace ossia
{
/**
 * Identifies an element in a nested tuple.
 * For instance :
 *
 * \code
 * Tuple_A{
 *  Tuple_B{1, 2, 3},
 *  Tuple_C{
 *    Tuple_D{4},
 *    Tuple_E{5}
 *  }
 * }
 *
 * destination_index{}        => Tuple_A
 * destination_index{0}       => Tuple_B
 * destination_index{0, 1}    => '1'
 * destination_index{0, 2}    => '2'
 * destination_index{0, 3}    => '3'
 * destination_index{1}       => Tuple_C
 * destination_index{1, 0}    => Tuple_D
 * destination_index{1, 0, 0} => '4'
 * destination_index{1, 1}    => Tuple_E
 * destination_index{1, 1, 0} => '5'
 * \endcode
 *
 * There are at most 8 nested depth level.
 */
using destination_index = chobo::static_vector<uint8_t, 8>;
}
