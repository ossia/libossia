#pragma once
#include <ossia/dataflow/fx_node.hpp>
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/dataflow/node_process.hpp>
#include <ossia/dataflow/port.hpp>
#include <ossia/dataflow/safe_nodes/node.hpp>
#include <ossia/dataflow/safe_nodes/tick_policies.hpp>

#include <readerwriterqueue.h>

#include <bitset>

namespace ossia::safe_nodes
{

template <typename T, typename = void>
struct has_event_policy : std::false_type
{
};
template <typename T>
struct has_event_policy<T, std::void_t<typename T::event_policy>>
    : std::true_type
{
};

template <typename T, typename = void>
struct has_audio_policy : std::false_type
{
};
template <typename T>
struct has_audio_policy<T, std::void_t<typename T::audio_policy>>
    : std::true_type
{
};

template <typename T, typename = void>
struct has_control_policy : std::false_type
{
};
template <typename T>
struct has_control_policy<T, std::void_t<typename T::control_policy>>
    : std::true_type
{
};

template <typename Node_T>
class safe_node final : public ossia::graph_node,
                        public get_state<Node_T>::type
{
public:
  using info = info_functions<Node_T>;
  static const constexpr bool has_state = has_state_t<Node_T>::value;
  using state_type = typename get_state<Node_T>::type;

  using controls_changed_list
      = std::bitset<info_functions<Node_T>::control_count>;
  using controls_type = typename info_functions<Node_T>::controls_type;
  using controls_values_type =
      typename info_functions<Node_T>::controls_values_type;
  controls_values_type controls;
  controls_changed_list controls_changed;
  moodycamel::ReaderWriterQueue<controls_values_type> cqueue;

  template <std::size_t N>
  using timed_vec_t = ossia::safe_nodes::timed_vec<
      typename std::tuple_element<N, controls_type>::type::type>;

  template <std::size_t... I>
  static constexpr auto
  get_control_accessor_types(const std::index_sequence<I...>&) noexcept
  {
    return std::tuple<timed_vec_t<I>...>{};
  }

  using control_tuple_t = decltype(get_control_accessor_types(
      std::make_index_sequence<
          ossia::safe_nodes::info_functions<Node_T>::control_count>()));
  control_tuple_t control_tuple;

  safe_node() noexcept
  {
    m_inlets.reserve(info_functions<Node_T>::inlet_size);
    m_outlets.reserve(info_functions<Node_T>::outlet_size);
    for (auto& port : Node_T::Metadata::audio_ins)
    {
      (void)port;
      m_inlets.push_back(ossia::make_inlet<ossia::audio_port>());
    }
    for (auto& port : Node_T::Metadata::midi_ins)
    {
      (void)port;
      m_inlets.push_back(ossia::make_inlet<ossia::midi_port>());
    }
    for (auto& port : Node_T::Metadata::value_ins)
    {
      auto inlt = ossia::make_inlet<ossia::value_port>();
      inlt->data.target<ossia::value_port>()->is_event = port.is_event;
      m_inlets.push_back(std::move(inlt));
    }
    for (auto& port : Node_T::Metadata::address_ins)
    {
      (void)port;
      m_inlets.push_back(ossia::make_inlet<ossia::value_port>()); // TODO ??
    }
    for (std::size_t i = 0; i < info_functions<Node_T>::control_count; i++)
    {
      auto inlt = ossia::make_inlet<ossia::value_port>();
      inlt->data.target<ossia::value_port>()->is_event = true;
      m_inlets.push_back(std::move(inlt));
    }

    for (auto& port : Node_T::Metadata::audio_outs)
    {
      (void)port;
      m_outlets.push_back(ossia::make_outlet<ossia::audio_port>());
    }
    for (auto& port : Node_T::Metadata::midi_outs)
    {
      (void)port;
      m_outlets.push_back(ossia::make_outlet<ossia::midi_port>());
    }
    for (auto& port : Node_T::Metadata::value_outs)
    {
      (void)port;
      m_outlets.push_back(ossia::make_outlet<ossia::value_port>());
    }
  }

  template <std::size_t N>
  static constexpr auto get_inlet_accessor() noexcept
  {
    constexpr auto cat = info::categorize_inlet(N);
    if constexpr (cat == ossia::safe_nodes::inlet_kind::audio_in)
      return [](const ossia::inlets& inl) -> const ossia::audio_port& {
        return *inl[N]->data.target<ossia::audio_port>();
      };
    else if constexpr (cat == ossia::safe_nodes::inlet_kind::midi_in)
      return [](const ossia::inlets& inl) -> const ossia::midi_port& {
        return *inl[N]->data.target<ossia::midi_port>();
      };
    else if constexpr (cat == ossia::safe_nodes::inlet_kind::value_in)
      return [](const ossia::inlets& inl) -> const ossia::value_port& {
        return *inl[N]->data.target<ossia::value_port>();
      };
    else if constexpr (cat == ossia::safe_nodes::inlet_kind::address_in)
      return [](const ossia::inlets& inl) -> const ossia::destination_t& {
        return inl[N]->address;
      };
    else
      throw;
  }

  template <bool Validate, std::size_t N>
  struct apply_control;

  template <std::size_t N>
  struct apply_control<true, N>
  {
    template <typename Vec, typename Vp>
    void operator()(Vec& vec, safe_node& self, const Vp& vp) noexcept
    {
      constexpr const auto ctrls = get_controls<Node_T>{}();
      constexpr auto ctrl = std::get<N>(ctrls);
      for (auto& v : vp)
      {
        if (auto res = ctrl.fromValue(v.value))
        {
          vec[int64_t{v.timestamp}] = *std::move(res);
          self.controls_changed.set(N);
        }
      }
    }
  };
  template <std::size_t N>
  struct apply_control<false, N>
  {
    template <typename Vec, typename Vp>
    void operator()(Vec& vec, safe_node& self, const Vp& vp) noexcept
    {
      constexpr const auto ctrls = get_controls<Node_T>{}();
      constexpr auto ctrl = std::get<N>(ctrls);
      for (auto& v : vp)
      {
        vec[int64_t{v.timestamp}] = ctrl.fromValue(v.value);
        self.controls_changed.set(N);
      }
    }
  };
  template <std::size_t N>
  static constexpr auto get_control_accessor() noexcept
  {
    return [](const ossia::inlets& inl, safe_node& self) -> const auto&
    {
      constexpr const auto idx = info::control_start + N;
      static_assert(info::control_count > 0);
      static_assert(N < info::control_count);

      using control_type = typename std::tuple_element<
          N, decltype(get_controls<Node_T>{}())>::type;
      using val_type = typename control_type::type;

      ossia::safe_nodes::timed_vec<val_type>& vec
          = std::get<N>(self.control_tuple);
      vec.clear();
      const auto& vp
          = inl[idx]->data.template target<ossia::value_port>()->get_data();
      vec.container.reserve(vp.size() + 1);

      // in all cases, set the current value at t=0
      vec.insert(std::make_pair(int64_t{0}, std::get<N>(self.controls)));

      // copy all the values... values arrived later replace previous ones
      apply_control<control_type::must_validate, N>{}(vec, self, vp);

      // the last value will be the first for the next tick
      std::get<N>(self.controls) = vec.rbegin()->second;
      return vec;
    };
  }

  template <std::size_t N>
  static constexpr auto get_outlet_accessor() noexcept
  {
    if constexpr (N < info::audio_out_count)
      return [](const ossia::outlets& inl) -> ossia::audio_port& {
        return *inl[N]->data.target<ossia::audio_port>();
      };
    else if constexpr (N < (info::audio_out_count + info::midi_out_count))
      return [](const ossia::outlets& inl) -> ossia::midi_port& {
        return *inl[N]->data.target<ossia::midi_port>();
      };
    else if constexpr (
        N < (info::audio_out_count + info::midi_out_count
             + info::value_out_count))
      return [](const ossia::outlets& inl) -> ossia::value_port& {
        return *inl[N]->data.target<ossia::value_port>();
      };
    else
      throw;
  }

  /////////////////

  template <std::size_t... I, std::size_t... C, std::size_t... O >
  void
	  apply_all_impl(const std::index_sequence<I...>& i1, const std::index_sequence<C...>& i2, const std::index_sequence<O...>& i3, ossia::token_request tk, ossia::exec_state_facade st) noexcept
  {
	  ossia::inlets& inlets = this->inputs();
	  ossia::outlets& outlets = this->outputs();

	  if constexpr (has_state)
	  {
		  if constexpr (info::control_count > 0)
		  {
			  Node_T::control_policy{}([&](const ossia::token_request& sub_tk, auto&& ... ctls) {
				  Node_T::run(
					  get_inlet_accessor<I>()(inlets)...,
					  std::forward<decltype(ctls)>(ctls)...,
					  get_outlet_accessor<O>()(outlets)...,
					  sub_tk, st, static_cast<state_type&>(*this)
				  );
				  }, tk, get_control_accessor<C>()(inlets, *this)...);
		  }
		  else
		  {
			  Node_T::run(
				  get_inlet_accessor<I>()(inlets)...,
				  get_outlet_accessor<O>()(outlets)...,
				  tk, st, static_cast<state_type&>(*this)
			  );
		  }
	  }
	  else
	  {
		  if constexpr (info::control_count > 0)
		  { 
			  Node_T::control_policy{}([&](const ossia::token_request& sub_tk, auto&& ... ctls) {
				  Node_T::run(
					  get_inlet_accessor<I>()(inlets)...,
					  std::forward<decltype(ctls)>(ctls)...,
					  get_outlet_accessor<O>()(outlets)...,
					  sub_tk, st
				  );
				  }, tk, get_control_accessor<C>()(inlets, *this)...);
		  }
		  else
		  {
			  Node_T::run(
				  get_inlet_accessor<I>()(inlets)...,
				  get_outlet_accessor<O>()(outlets)...,
				  tk, st
			  );
		  }
	  }
  }
  void
  run(ossia::token_request tk, ossia::exec_state_facade st) noexcept override
  {
    using inlets_indices = std::make_index_sequence<info::control_start>;
    using controls_indices = std::make_index_sequence<info::control_count>;
    using outlets_indices = std::make_index_sequence<info::outlet_size>;
	
	apply_all_impl(inlets_indices{}, controls_indices{}, outlets_indices{}, tk, st);

    if (cqueue.size_approx() < 1 && controls_changed.any())
    {
      cqueue.try_enqueue(controls);
      controls_changed.reset();
    }
  }

  void all_notes_off() noexcept override
  {
    if constexpr (info::midi_in_count > 0)
    {
      // TODO
    }
  }

  std::string label() const noexcept override
  {
    return "Control";
  }
};

struct value_adder
{
  ossia::value_port& port;
  ossia::value v;
  void operator()()
  {
    // timestamp should be > all others so that it is always active ?
    port.write_value(std::move(v), 0);
  }
};

template <typename T>
struct control_updater
{
  T& control;
  T v;
  void operator()()
  {
    control = std::move(v);
  }
};
}
