#pragma once
#define RAPIDJSON_HAS_STDSTRING 1
#include <rapidjson/writer.h>
#include <cstring>

namespace rapidjson
{
namespace internal
{
template <>
inline rapidjson::SizeType StrLen<char>(const char* s)
{
    RAPIDJSON_ASSERT(s != 0);
    return std::strlen(s);
}
}
}
