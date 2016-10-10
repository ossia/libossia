#pragma once
#include <ossia/editor/dataspace/dataspace.hpp>

namespace ossia
{
/**
 * @brief get_dataspace_text
 * @return Text corresponding to the dataspace. For instance "color" or "gain".
 */
OSSIA_EXPORT
boost::string_view get_dataspace_text(ossia::unit_t);

/**
 * @brief get_unit_text
 * @return Text corresponding to the unit. For instance "rgb" or "xyz".
 */
OSSIA_EXPORT
boost::string_view get_unit_text(ossia::unit_t);

/**
 * @brief get_pretty_unit_text Unit for human readability and debugging
 *
 * Format is dataspace.unit, e.g. distance.m/s or color.rgb
 */
OSSIA_EXPORT
std::string get_pretty_unit_text(ossia::unit_t);


/**
 * @brief parse_dataspace
 * @param text Something like color, etc.
 *
 * @return A value corresponding to the dataspace, without any specific unit.
 */
OSSIA_EXPORT
ossia::unit_t parse_dataspace(boost::string_view text);

/**
 * @brief parse_unit Takes a text and a dataspace and gives a corresponding unit.
 * @param text The text to parse
 * @param dataspace A dataspace such as ossia::color_u, ossia::distance_u...
 *
 * @return Corresponding unit, or the same unit as input if it was not found.
 */
OSSIA_EXPORT
ossia::unit_t parse_unit(boost::string_view text, ossia::unit_t dataspace);


/**
 * @brief parse_pretty_unit Create a unit from a valid unit in the format
 * of get_pretty_unit_text
 */
OSSIA_EXPORT
ossia::unit_t parse_pretty_unit(boost::string_view text);


/**
 * @brief parse_unit Alternative if we already have a dataspace type at hand.
 * @note T should be a dataspace, such as ossia::color_u.
 * @todo In 2020 enforce this with concepts...
 */
template<typename T>
OSSIA_EXPORT
ossia::unit_t parse_unit(boost::string_view text, T dataspace);


/**
 * @brief make_value Create a value_with_unit
 * @param v A value
 * @param u A unit
 *
 * @return The same value with the unit attached if the types were correct.
 */
OSSIA_EXPORT
value_with_unit make_value(const ossia::value& v, const ossia::unit_t& u);

/**
 * @brief convert Convert a value to another unit in the same dataspace
 * @param v A value
 * @param t An unit of the same dataspace than the value.
 *
 * E.g. you can convert 1cm in millimeters, but not 1cm in RGB colors.
 *
 * @return The converted value if both value and unit are valid
 * and convertible, else the input value
 */
OSSIA_EXPORT
ossia::value_with_unit convert(ossia::value_with_unit v, ossia::unit_t t);


/**
 * @brief convert Convert a value + unit to a simple value
 * @param v Value to convert
 * @return Corresponding ossia::value
 */
OSSIA_EXPORT
ossia::value to_value(ossia::value_with_unit v);


/**
 * @brief to_unit Get the unit part of a value_with_unit
 */
OSSIA_EXPORT
ossia::unit_t to_unit(ossia::value_with_unit v);

/**
 * @brief to_pretty_string Pretty string of unit & value
 *
 * @return For example "[0.2, 1.6, 3.4] rgb"
 */
OSSIA_EXPORT
std::string to_pretty_string(ossia::value_with_unit v);

/**
 * @brief merge Merge a computed value in a unit-aware value
 *
 * The computed value is assumed to be of the same dataspace.
 * For instance if v is a HSV color, then t can also be an HSV color,
 * or just "h", "s", or "v" component, but not a "r" color.
 *
 * @param The value with an unit
 * @param The value to merge
 * @param Optional destination index to merge with
 * @return
 */
OSSIA_EXPORT
ossia::value_with_unit merge(
    ossia::value_with_unit,
    const ossia::value&,
    ossia::destination_index);

}
