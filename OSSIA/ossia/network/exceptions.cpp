#include <ossia/network/exceptions.hpp>

namespace ossia
{

parse_error::~parse_error()
{
}

connection_error::~connection_error()
{
}

invalid_node_error::~invalid_node_error()
{
}

node_not_found_error::~node_not_found_error()
{
}

bad_request_error::~bad_request_error()
{
}

invalid_value_type_error::~invalid_value_type_error()
{
}
}
