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
  control_in
};
enum class outlet_kind
{
  audio_out,
  midi_out,
  value_out
};
struct address_in
{
  const std::string_view name;

  template <std::size_t N>
  constexpr address_in(const char (&name)[N]) : name{name, N-1}
  {
  }
};
struct audio_in
{
  const std::string_view name;

  template <std::size_t N>
  constexpr audio_in(const char (&name)[N]) : name{name, N-1}
  {
  }
};
struct audio_out
{
  const std::string_view name;

  template <std::size_t N>
  constexpr audio_out(const char (&name)[N]) : name{name, N-1}
  {
  }
};
struct value_in
{
  const std::string_view name;
  const bool is_event{true};

  template <std::size_t N>
  constexpr value_in(const char (&name)[N]) : name{name, N-1}
  {
  }

  template <std::size_t N>
  constexpr value_in(const char (&name)[N], bool b)
      : name{name, N-1}, is_event{b}
  {
  }
};
struct value_out
{
  const std::string_view name;

  template <std::size_t N>
  constexpr value_out(const char (&name)[N]) : name{name, N-1}
  {
  }
};
struct midi_in
{
  const std::string_view name;

  template <std::size_t N>
  constexpr midi_in(const char (&name)[N]) : name{name, N-1}
  {
  }
};
struct midi_out
{
  const std::string_view name;

  template <std::size_t N>
  constexpr midi_out(const char (&name)[N]) : name{name, N-1}
  {
  }
};
struct control_in
{
  const std::string_view name;

  template <std::size_t N>
  constexpr control_in(const char (&name)[N]) : name{name, N-1}
  {
  }
};
}
