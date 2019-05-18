#pragma once
#include <ossia/editor/state/destination_qualifiers.hpp>
#include <ossia/network/dataspace/dataspace.hpp>

#include <boost/spirit/include/qi_symbols.hpp>

namespace ossia
{
using unit_parse_symbols_t
    = boost::spirit::qi::symbols<char, ossia::destination_qualifiers>;

OSSIA_EXPORT
const unit_parse_symbols_t& get_unit_parser();
}
