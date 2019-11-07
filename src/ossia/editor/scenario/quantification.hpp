#pragma once
#include <ossia/detail/optional.hpp>

namespace ossia
{
struct token_request;
struct time_value;

OSSIA_EXPORT
optional<time_value> get_quantification_date(const ossia::token_request& tk, double rate);
}
