#pragma once
#include <ossia/dataflow/safe_nodes/port.hpp>
#include <boost/container/flat_map.hpp>
#include <brigand/sequences/list.hpp>
#include <brigand/algorithms/index_of.hpp>
#include <brigand/algorithms/transform.hpp>
#include <ossia/detail/algorithms.hpp>
#include <tuple>
#include <array>

namespace ossia::safe_nodes
{

template<typename T>
using timed_vec = boost::container::flat_map<int64_t, T>;

template<typename... Args>
static constexpr auto make_node(Args&&... args)
{
  return std::tuple<Args...>{std::forward<Args>(args)...};
}

template<std::size_t N>
using address_ins = std::array<address_in, N>;
template<std::size_t N>
using audio_ins = std::array<audio_in, N>;
template<std::size_t N>
using audio_outs = std::array<audio_out, N>;
template<std::size_t N>
using midi_ins = std::array<midi_in, N>;
template<std::size_t N>
using midi_outs = std::array<midi_out, N>;
template<std::size_t N>
using value_ins = std::array<value_in, N>;
template<std::size_t N>
using value_outs = std::array<value_out, N>;
template<std::size_t N>
using controls = std::array<control_in, N>;

template<typename... Args>
struct node_builder: Args...
{
  constexpr auto audio_ins() const { return *this; }
  constexpr auto audio_outs() const { return *this; }
  constexpr auto midi_ins() const { return *this; }
  constexpr auto midi_outs() const { return *this; }
  constexpr auto value_ins() const { return *this; }
  constexpr auto value_outs() const { return *this; }
  constexpr auto controls() const { return *this; }

  template<typename... SArgs>
  constexpr node_builder(SArgs&&... sargs): Args{sargs}... { }

  template<std::size_t N>
  constexpr auto address_ins(const address_in (&arg)[N]) const {
    return node_builder<std::array<address_in, N>, Args...>{ossia::to_array(arg), static_cast<Args>(*this)...};
  }
  template<std::size_t N>
  constexpr auto audio_ins(const audio_in (&arg)[N]) const {
    return node_builder<std::array<audio_in, N>, Args...>{ossia::to_array(arg), static_cast<Args>(*this)...};
  }
  template<std::size_t N>
  constexpr auto audio_outs(const audio_out (&arg)[N]) const {
    return node_builder<std::array<audio_out, N>, Args...>{ossia::to_array(arg), static_cast<Args>(*this)...};
  }

  template<std::size_t N>
  constexpr auto midi_ins(const midi_in (&arg)[N]) const {
    return node_builder<std::array<midi_in, N>, Args...>{ossia::to_array(arg), static_cast<Args>(*this)...};
  }
  template<std::size_t N>
  constexpr auto midi_outs(const midi_out (&arg)[N]) const {
    return node_builder<std::array<midi_out, N>, Args...>{ossia::to_array(arg), static_cast<Args>(*this)...};
  }

  template<std::size_t N>
  constexpr auto value_ins(const value_in (&arg)[N]) const {
    return node_builder<std::array<value_in, N>, Args...>{ossia::to_array(arg), static_cast<Args>(*this)...};
  }
  template<std::size_t N>
  constexpr auto value_outs(const value_out (&arg)[N]) const {
    return node_builder<std::array<value_out, N>, Args...>{ossia::to_array(arg), static_cast<Args>(*this)...};
  }
  template<typename... Controls>
  constexpr auto controls(Controls&&... ctrls) const {
    return node_builder<std::tuple<Controls...>, Args...>{
        std::make_tuple(ctrls...), static_cast<Args>(*this)...
    };
  }

  constexpr auto build() const {
    return std::tuple<Args...>{static_cast<Args>(*this)...};
  }
};

constexpr auto create_node() {
  return node_builder<>{};
}


template<typename T, typename...>
struct is_port : std::false_type {};
template<typename T, std::size_t N>
struct is_port<T, std::array<T, N>> : std::true_type {};

template<typename T>
struct dummy_container {
    static constexpr bool is_event = false;
    static constexpr auto begin() { return (T*)nullptr; }
    static constexpr auto end() { return (T*)nullptr; }
    static constexpr std::size_t size() { return 0; }
};
template <typename T, typename = int>
struct has_info : std::false_type { };

template <typename T>
struct has_info <
      T
    , decltype((void) T::info, 0)
> : std::true_type { };

template<typename PortType, typename T>
struct get_ports
{
    constexpr auto operator()()
    {
      if constexpr(has_info<T>::value)
      {
        using index = brigand::index_if<decltype(T::info), is_port<PortType, brigand::_1>>;

        if constexpr(!std::is_same<index, brigand::no_such_type_>::value)
        {
          using array_type = brigand::at<decltype(T::info), index>;

          return std::get<array_type>(T::info);
        }
        else
        {
          return dummy_container<PortType>{};
        }
      }
      else
      {
        return msvc();
      }
    }

    constexpr auto msvc()
    {
      if constexpr(std::is_same<PortType, value_in>::value)
      {
        return T::Metadata::value_ins;
      }
      else if constexpr(std::is_same<PortType, value_out>::value)
      {
        return T::Metadata::value_outs;
      }

      else if constexpr(std::is_same<PortType, midi_in>::value)
      {
        return T::Metadata::midi_ins;
      }
      else if constexpr(std::is_same<PortType, midi_out>::value)
      {
        return T::Metadata::midi_outs;
      }

      else if constexpr(std::is_same<PortType, audio_in>::value)
      {
        return T::Metadata::audio_ins;
      }
      else if constexpr(std::is_same<PortType, audio_out>::value)
      {
        return T::Metadata::audio_outs;
      }

      else if constexpr(std::is_same<PortType, address_in>::value)
      {
        return T::Metadata::address_ins;
      }

      else if constexpr(std::is_same<PortType, control_in>::value)
      {
        return T::Metadata::controls;
      }

      else
      {
        return dummy_container<PortType>{};
      }
    }
};
template<typename...>
struct is_controls : std::false_type {};
template<typename... Args>
struct is_controls <std::tuple<Args...>> : std::true_type {};

template<typename T>
struct get_controls
{
    constexpr auto operator()()
    {
      if constexpr(has_info<T>::value)
      {
        using index = brigand::index_if<decltype(T::info), is_controls<brigand::_1>>;

        if constexpr(!std::is_same<index, brigand::no_such_type_>::value)
        {
          using tuple_type = brigand::at<decltype(T::info), index>;

          return std::get<tuple_type>(T::info);
        }
        else
        {
          return std::tuple<>{};
        }
      }
      else
      {
        using type = typename T::Metadata;
        return type::controls;
      }
    }
};


struct dummy_t { };

template<typename T>
using my_void_t = void;
template<typename T, typename = void>
struct has_state_t : std::false_type{};
template<typename T>
struct has_state_t<T, my_void_t<typename T::State>> : std::true_type{};


template<typename T, typename = void>
struct get_state { using type = dummy_t; };
template<typename T>
struct get_state<T, my_void_t<typename T::State>> { using type = typename T::State; };


template<typename T>
struct get_control_type
{
    using type = typename T::type;
};
template<typename Node_T>
struct info_functions
{
  using controls_type = decltype(get_controls<Node_T>{}());
  using controls_values_type = brigand::transform<controls_type, get_control_type<brigand::_1>>;

  static constexpr auto audio_in_count =
      get_ports<audio_in, Node_T>{}().size();
  static constexpr auto audio_out_count =
      get_ports<audio_out, Node_T>{}().size();
  static constexpr auto midi_in_count =
      get_ports<midi_in, Node_T>{}().size();
  static constexpr auto midi_out_count =
      get_ports<midi_out, Node_T>{}().size();
  static constexpr auto value_in_count =
      get_ports<value_in, Node_T>{}().size();
  static constexpr auto value_out_count =
      get_ports<value_out, Node_T>{}().size();
  static constexpr auto control_count =
      std::tuple_size<controls_type>::value;
  static constexpr auto address_in_count =
      get_ports<address_in, Node_T>{}().size();

  static constexpr auto categorize_inlet(std::size_t i)
  {
    if(i < audio_in_count)
      return inlet_kind::audio_in;
    else if(i < audio_in_count + midi_in_count)
      return inlet_kind::midi_in;
    else if(i < audio_in_count + midi_in_count + value_in_count)
      return inlet_kind::value_in;
    else if(i < audio_in_count + midi_in_count + value_in_count + address_in_count)
      return inlet_kind::address_in;
    else if(i < audio_in_count + midi_in_count + value_in_count + address_in_count + control_count)
      return inlet_kind::control_in;
    else
      throw std::runtime_error("Invalid input number");
  }


  static constexpr auto control_start =
      audio_in_count + midi_in_count + value_in_count + address_in_count;

  static constexpr auto inlet_size =
      control_start + control_count;

  static constexpr auto outlet_size =
      audio_out_count + midi_out_count + value_out_count;

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
