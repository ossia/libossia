#pragma once
#include <ossia/detail/config.hpp>

#include <ossia/network/common/parameter_properties.hpp>

#include <string>
#include <vector>

namespace ossia
{

//! How a low-level type should be interpreted.
using extended_type = std::string;

// Here are the known extended types :
//! Represents a "raw" data buffer, that should not be interpreted as a
//! readable string.
//! Only meaningful for strings.
OSSIA_EXPORT extended_type generic_buffer_type();

//! Represents a filesystem path : "c:\\windows\\virus.exe",
//! "../../share/pony.png", etc.
//! Only meaningful for strings.
OSSIA_EXPORT extended_type filesystem_path_type();

//! Represents an URL or URI : "http://foo.com/whatever",
//! "120.10.10.15:123/foo", "mailto:john.smith@ossia.io"
OSSIA_EXPORT extended_type url_type();

//! Means that the array should be interpreted as a fixed float array (e.g.
//! vec2f, etc)
OSSIA_EXPORT extended_type float_array_type();

//! Means that the array should be interpreted as a dynamic float list
OSSIA_EXPORT extended_type float_list_type();

//! Means that the array should be interpreted as a dynamic int list
OSSIA_EXPORT extended_type integer_list_type();

//! Means that the array should be interpreted as a dynamic string list
OSSIA_EXPORT extended_type string_list_type();

//! Means that the blob should be a u8 blob, e.g. each value of the array
//! is a u8 value in an OSC blob
OSSIA_EXPORT extended_type u8_blob_type();

//! Means that the array is a dynamic array (e.g. ossia::val_type::LIST)
OSSIA_EXPORT extended_type list_type();

//! Means that the value is an impulse with OSC type-tag NIL
OSSIA_EXPORT extended_type nil_type();

//! Means that the value is an impulse with no OSC type-tag (,\0\0\0)
OSSIA_EXPORT extended_type empty_type();

//! Returns the possible implementation types for this high-level type.
OSSIA_EXPORT std::vector<ossia::val_type> underlying_type(const extended_type&);
}
