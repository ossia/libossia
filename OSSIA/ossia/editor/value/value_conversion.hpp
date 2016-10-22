#pragma once
#include <ossia/network/common/address_properties.hpp>
#include <ossia/editor/value/value.hpp>

namespace ossia
{

/**
 * @brief convert Convert a value to the given type.
 * @param val A value
 * @return The closest possible conversion of the value to T, else a default-constructed T.
 */
template<typename T>
OSSIA_EXPORT T convert(
    const ossia::value& val);

/**
 * @brief convert Convert tuples to array<float, 2/3/4>
 */
template<typename T>
OSSIA_EXPORT T convert(
    const ossia::Tuple& val);

OSSIA_EXPORT ossia::value convert(
    const ossia::value& val,
    ossia::val_type newtype);
}
