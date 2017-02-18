#pragma once
#include <ossia/detail/config.hpp>
#include <rapidjson/writer.h>
#include <rapidjson/document.h>
#include <ossia/detail/string_view.hpp>
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

inline std::string getString(const rapidjson::Value& val)
{
  return std::string{val.GetString(), val.GetStringLength()};
}
inline ossia::string_view getStringView(const rapidjson::Value& val)
{
  return ossia::string_view{val.GetString(), val.GetStringLength()};
}


inline void writeKey(rapidjson::Writer<rapidjson::StringBuffer>& writer, ossia::string_view k)
{
    writer.Key(k.data(), k.size());
}

inline void writeRef(rapidjson::Writer<rapidjson::StringBuffer>& writer, ossia::string_view k)
{
    writer.String(k.data(), k.size());
}

inline void writeChar(rapidjson::Writer<rapidjson::StringBuffer>& writer, char c)
{
    writer.String(&c, 1);
}

using allocator = rapidjson::MemoryPoolAllocator<>;
