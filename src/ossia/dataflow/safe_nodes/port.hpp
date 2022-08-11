#pragma once

#include <array>
#include <cstdint>
#include <string_view>

namespace ossia::safe_nodes
{

enum class inlet_kind
{
  audio_in,
  midi_in,
  value_in,
  address_in,
  control_in,
};
enum class outlet_kind
{
  audio_out,
  midi_out,
  value_out,
  control_out,
};
struct address_in
{
  const std::string_view name;

  constexpr address_in(const char* name)
      : name{name}
  {
  }
};
struct audio_in
{
  const std::string_view name;

  constexpr audio_in(const char* name)
      : name{name}
  {
  }
};
struct audio_out
{
  const std::string_view name;

  constexpr audio_out(const char* name)
      : name{name}
  {
  }
};
struct value_in
{
  const std::string_view name;
  const bool is_event{true};

  constexpr value_in(const char* name)
      : name{name}
  {
  }

  constexpr value_in(const char* name, bool b)
      : name{name}
      , is_event{b}
  {
  }
};
struct value_out
{
  const std::string_view name;
  const std::string_view type;

  constexpr value_out(const char* name)
      : name{name}
  {
  }

  constexpr value_out(std::string_view name, std::string_view t)
      : name{name}
      , type{t}
  {
  }
};
struct midi_in
{
  const std::string_view name;

  constexpr midi_in(const char* name)
      : name{name}
  {
  }
};
struct midi_out
{
  const std::string_view name;

  constexpr midi_out(const char* name)
      : name{name}
  {
  }
};
struct control_in
{
  const std::string_view name;

  constexpr control_in(const char* name)
      : name{name}
  {
  }
};

struct control_out
{
  const std::string_view name;

  constexpr control_out(const char* name)
      : name{name}
  {
  }
};
}
