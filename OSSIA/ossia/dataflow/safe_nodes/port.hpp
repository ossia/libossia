#pragma once

#include <string_view>
#include <cstdint>
#include <array>

namespace ossia::safe_nodes
{

enum class inlet_kind
{
  audio_in, midi_in, value_in, address_in, control_in
};
enum class outlet_kind
{
  audio_out, midi_out, value_out
};
struct address_in {
  const std::string_view name;

  template<std::size_t N>
  constexpr address_in(const char (&name)[N]): name{name, N} { }
};
struct audio_in {
  const std::string_view name;

  template<std::size_t N>
  constexpr audio_in(const char (&name)[N]): name{name, N} { }
};
struct audio_out {
  const std::string_view name;

  template<std::size_t N>
  constexpr audio_out(const char (&name)[N]): name{name, N} { }
};
struct value_in {
  const std::string_view name;
  const bool is_event{true};

  template<std::size_t N>
  constexpr value_in(const char (&name)[N]): name{name, N} { }

  template<std::size_t N>
  constexpr value_in(const char (&name)[N], bool b): name{name, N}, is_event{b} { }
};
struct value_out {
  const std::string_view name;

  template<std::size_t N>
  constexpr value_out(const char (&name)[N]): name{name, N} { }
};
struct midi_in {
  const std::string_view name;

  template<std::size_t N>
  constexpr midi_in(const char (&name)[N]): name{name, N} { }
};
struct midi_out {
  const std::string_view name;

  template<std::size_t N>
  constexpr midi_out(const char (&name)[N]): name{name, N} { }
};
struct control_in {
  const std::string_view name;

  template<std::size_t N>
  constexpr control_in(const char (&name)[N]):
    name{name, N}
  {
  }
};
}
