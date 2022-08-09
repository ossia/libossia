#include <ossia/network/exceptions.hpp>

namespace ossia
{

parse_error::~parse_error() = default;

connection_error::~connection_error() = default;

invalid_node_error::~invalid_node_error() = default;

node_not_found_error::~node_not_found_error() = default;

bad_request_error::~bad_request_error() = default;

invalid_value_type_error::~invalid_value_type_error() = default;
}
