#pragma once
#include <ossia/detail/config.hpp>

#include <ossia/detail/string_view.hpp>
#if defined(_WIN32)
#include <windows.h>

#ifdef GetObject
#undef GetObject
#endif

// Yay for windows defining uuid_t macro
#ifdef uuid_t
#undef uuid_t
#endif

#endif
#include <rapidjson/document.h>
#include <rapidjson/writer.h>

#include <cstring>

namespace ossia
{
using json_writer = rapidjson::Writer<
    rapidjson::StringBuffer, rapidjson::UTF8<>, rapidjson::UTF8<>,
    rapidjson::CrtAllocator, rapidjson::WriteFlag::kWriteNanAndInfFlag>;
}
inline std::string get_string(const rapidjson::Value& val)
{
  return std::string{val.GetString(), val.GetStringLength()};
}
inline std::string_view get_string_view(const rapidjson::Value& val)
{
  return std::string_view{val.GetString(), val.GetStringLength()};
}

inline void write_json_key(ossia::json_writer& writer, std::string_view k)
{
  writer.Key(k.data(), k.size());
}

inline void write_json(ossia::json_writer& writer, std::string_view k)
{
  writer.String(k.data(), k.size());
}

inline void write_json(ossia::json_writer& writer, char c)
{
  writer.String(&c, 1);
}

inline std::string json_to_str(const rapidjson::StringBuffer& other)
{
  return {other.GetString(), other.GetSize()};
}
