#pragma once
#include <ossia/network/common/address_properties.hpp>
#include <string>
#include <vector>
#include <ossia_export.h>

namespace ossia
{

//! How a low-level type should be interpreted.
using extended_type = std::string;

// Here are the known extended types :
//! Represents a "raw" data buffer, that should not be interpreted as a readable string.
//! Only meaningful for strings.
OSSIA_EXPORT extended_type generic_buffer_type();

//! Represents a filesystem path : "c:\\windows\\virus.exe", "../../share/pony.png", etc.
//! Only meaningful for strings.
OSSIA_EXPORT extended_type filesystem_path_type();

//! Means that the array should be interpreted as a fixed float array (e.g. vec2f, etc)
OSSIA_EXPORT extended_type float_array_type();

//! Means that the array is a dynamic array (e.g. ossia::val_type::TUPLE)
OSSIA_EXPORT extended_type tuple_type();

//! Returns the possible implementation types for this high-level type.
OSSIA_EXPORT std::vector<ossia::val_type> underlying_type(const extended_type&);

}
