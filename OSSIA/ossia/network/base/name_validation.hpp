#pragma once
#include <ossia/detail/algorithms.hpp>
#include <vector>
#include <string>
#include <ossia_export.h>
namespace ossia
{
namespace net
{
// The definition is in node.cpp
OSSIA_EXPORT std::string sanitize_name(std::string name);
OSSIA_EXPORT std::string sanitize_name(std::string name_base, const std::vector<std::string>& brethren);

template<typename Char_T>
bool is_valid_character_for_name(Char_T c)
{
    return (c >= 'a' && c <= 'z')
        || (c >= 'A' && c <= 'Z')
        || (c >= '0' && c <= '9')
        || (c == '.')
        || (c == '~')
        || (c == '_')
        || (c == '(')
        || (c == ')')
        || (c == '-');
}

inline boost::string_view name_characters() noexcept
{ return make_string_view("a-zA-Z0-9_~().-"); }

}
}
