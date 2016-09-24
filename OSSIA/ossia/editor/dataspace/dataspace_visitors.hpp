#pragma once
#include <ossia/editor/dataspace/dataspace.hpp>
#include <brigand/algorithms/fold.hpp>

namespace ossia
{
/**
 * @brief get_unit_text
 * @return Text corresponding to the unit. For instance "rgb" or "xyz".
 */
OSSIA_EXPORT boost::string_ref get_unit_text(ossia::unit_t);


/**
 * @brief parse_dataspace
 * @param text Something like color, etc.
 *
 * @return A value corresponding to the dataspace, without any specific unit.
 */
OSSIA_EXPORT ossia::unit_t parse_dataspace(boost::string_ref text);

/**
 * @brief parse_unit Takes a text and a dataspace and gives a corresponding unit.
 * @param text The text to parse
 * @param dataspace A dataspace such as ossia::color_u, ossia::distance_u...
 *
 * @return Corresponding unit, or the same unit as input if it was not found.
 */
OSSIA_EXPORT ossia::unit_t parse_unit(boost::string_ref text, ossia::unit_t dataspace);


/**
 * @brief parse_unit Alternative if we already have a dataspace type at hand.
 * @note T should be a dataspace, such as ossia::color_u.
 * @todo In 2020 enforce this with concepts...
 */
template<typename T>
OSSIA_EXPORT ossia::unit_t parse_unit(boost::string_ref text, T dataspace);

}
