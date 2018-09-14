#pragma once
#include <ossia/dataflow/safe_nodes/port.hpp>
#include <ossia/detail/algorithms.hpp>
#include <ossia/detail/flat_map.hpp>

#include <brigand/algorithms/index_of.hpp>
#include <brigand/algorithms/transform.hpp>
#include <brigand/sequences/list.hpp>

#include <array>
#include <tuple>

namespace ossia::safe_nodes
{

template <typename T>
using timed_vec = ossia::flat_map<int64_t, T>;

template <typename... Args>
static constexpr auto make_node(Args&&... args)
{
  return std::tuple<Args...>{std::forward<Args>(args)...};
}

template <typename T, typename...>
struct is_port : std::false_type
{
};
template <typename T, std::size_t N>
struct is_port<T, std::array<T, N>> : std::true_type
{
};

template <typename T>
struct dummy_container
{
  static constexpr bool is_event = false;
  static constexpr auto begin()
  {
    return (T*)nullptr;
  }
  static constexpr auto end()
  {
    return (T*)nullptr;
  }
  static constexpr std::size_t size()
  {
    return 0;
  }
};

template <typename...>
struct is_controls : std::false_type
{
};
template <typename... Args>
struct is_controls<std::tuple<Args...>> : std::true_type
{
};

template <typename T>
struct get_controls
{
  constexpr auto operator()()
  {
    using type = typename T::Metadata;
    return type::controls;
  }
};

struct dummy_t
{
};

template <typename T>
using my_void_t = void;
template <typename T, typename = void>
struct has_state_t : std::false_type
{
};
template <typename T>
struct has_state_t<T, my_void_t<typename T::State>> : std::true_type
{
};

template <typename T, typename = void>
struct get_state
{
  using type = dummy_t;
};
template <typename T>
struct get_state<T, my_void_t<typename T::State>>
{
  using type = typename T::State;
};

template <typename T>
struct get_control_type
{
  using type = typename T::type;
};
template <typename Node_T>
struct info_functions
{
  using controls_type = decltype(get_controls<Node_T>{}());
  using controls_values_type
      = brigand::transform<controls_type, get_control_type<brigand::_1>>;

  static constexpr auto audio_in_count
      = std::size(Node_T::Metadata::audio_ins);
  static constexpr auto audio_out_count
      = std::size(Node_T::Metadata::audio_outs);
  static constexpr auto midi_in_count = std::size(Node_T::Metadata::midi_ins);
  static constexpr auto midi_out_count
      = std::size(Node_T::Metadata::midi_outs);
  static constexpr auto value_in_count
      = std::size(Node_T::Metadata::value_ins);
  static constexpr auto value_out_count
      = std::size(Node_T::Metadata::value_outs);
  static constexpr auto address_in_count
      = std::size(Node_T::Metadata::address_ins);
  static constexpr auto control_count
      = std::tuple_size_v<decltype(Node_T::Metadata::controls)>;

  static constexpr auto categorize_inlet(std::size_t i)
  {
    if (i < audio_in_count)
      return inlet_kind::audio_in;
    else if (i < audio_in_count + midi_in_count)
      return inlet_kind::midi_in;
    else if (i < audio_in_count + midi_in_count + value_in_count)
      return inlet_kind::value_in;
    else if (
        i < audio_in_count + midi_in_count + value_in_count + address_in_count)
      return inlet_kind::address_in;
    else if (
        i < audio_in_count + midi_in_count + value_in_count + address_in_count
                + control_count)
      return inlet_kind::control_in;
    else
      throw std::runtime_error("Invalid input number");
  }

  static constexpr auto control_start
      = audio_in_count + midi_in_count + value_in_count + address_in_count;

  static constexpr auto inlet_size = control_start + control_count;

  static constexpr auto outlet_size
      = audio_out_count + midi_out_count + value_out_count;
};

struct base_metadata
{
  using value_in = ossia::safe_nodes::value_in;
  using value_out = ossia::safe_nodes::value_out;
  using audio_in = ossia::safe_nodes::audio_in;
  using audio_out = ossia::safe_nodes::audio_out;
  using midi_in = ossia::safe_nodes::midi_in;
  using midi_out = ossia::safe_nodes::midi_out;
  using address_in = ossia::safe_nodes::address_in;

  static const constexpr dummy_container<value_in> value_ins{};
  static const constexpr dummy_container<value_out> value_outs{};
  static const constexpr dummy_container<audio_in> audio_ins{};
  static const constexpr dummy_container<audio_out> audio_outs{};
  static const constexpr dummy_container<midi_in> midi_ins{};
  static const constexpr dummy_container<midi_out> midi_outs{};
  static const constexpr dummy_container<address_in> address_ins{};
  static const constexpr std::tuple<> controls{};
};
}
