#pragma once
#include <ossia/dataflow/fx_node.hpp>
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/dataflow/node_process.hpp>
#include <ossia/dataflow/port.hpp>
#include <ossia/dataflow/safe_nodes/node.hpp>
#include <ossia/dataflow/safe_nodes/tick_policies.hpp>
#include <ossia/detail/algorithms.hpp>
#include <ossia/detail/apply_type.hpp>
#include <readerwriterqueue.h>

#include <bitset>
namespace ossia{


template <class F,
          template<class...> class T1, class... T1s, std::size_t... I1s,
          template<class...> class T2, class... T2s, std::size_t... I2s>
void for_each_in_tuples2_impl(
    T1<T1s...>& t1,
    T2<T2s...>& t2,
    F&& func,
    std::index_sequence<I1s...>,
    std::index_sequence<I2s...>
    )
{
  (std::forward<F>(func)
     (
        std::get<I1s>(std::forward<T1<T1s...>>(t1)),
        std::get<I2s>(std::forward<T2<T2s...>>(t2))
     ),
   ...);
}

template <class F,
          template<class...> class T1, class... T1s,
          template<class...> class T2, class... T2s>
void for_each_in_tuples2(T1<T1s...>& t1, T2<T2s...>& t2, F&& func)
{
  for_each_in_tuples2_impl(
        t1, t2,
        std::forward<F>(func),
        std::make_index_sequence<sizeof...(T1s)>(),
        std::make_index_sequence<sizeof...(T2s)>()
        );
}

// template <template<class...> class T1, template<class...> class T2, class F>
// void for_each_in_tuples2(T1<>&& t1, T2<>&& t2, const F& func)
// {
// }
}
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


template <typename info, std::size_t N>
static constexpr auto& get_inlet_accessor(const ossia::inlets& inl) noexcept
{
  constexpr auto cat = info::categorize_inlet(N);
  if constexpr (cat == ossia::safe_nodes::inlet_kind::audio_in)
      return *inl[N]->target<ossia::audio_port>();
  else if constexpr (cat == ossia::safe_nodes::inlet_kind::midi_in)
      return *inl[N]->target<ossia::midi_port>();
  else if constexpr (cat == ossia::safe_nodes::inlet_kind::value_in)
      return *inl[N]->target<ossia::value_port>();
  else if constexpr (cat == ossia::safe_nodes::inlet_kind::address_in)
      return inl[N]->address;
  else
    throw;
}

template <typename info, std::size_t N>
static constexpr auto& get_outlet_accessor(const ossia::outlets& outl) noexcept
{
  constexpr auto cat = info::categorize_outlet(N);
  if constexpr (cat == ossia::safe_nodes::outlet_kind::audio_out)
      return *outl[N]->target<ossia::audio_port>();
  else if constexpr (cat == ossia::safe_nodes::outlet_kind::midi_out)
      return *outl[N]->target<ossia::midi_port>();
  else if constexpr (cat == ossia::safe_nodes::outlet_kind::value_out)
      return *outl[N]->target<ossia::value_port>();
  else
    throw;
}

template <typename Node_T>
class safe_node final : public ossia::nonowning_graph_node,
                        public get_state<Node_T>::type
{
public:
  using info = info_functions<Node_T>;
  static const constexpr bool has_state = has_state_t<Node_T>::value;
  using state_type = typename get_state<Node_T>::type;

  using controls_changed_list = std::bitset<info_functions<Node_T>::control_count>;
  using controls_type = typename info_functions<Node_T>::controls_type;
  using controls_values_type = typename info_functions<Node_T>::controls_values_type;
  using control_tuple_t = typename ossia::apply_type<controls_values_type, timed_vec>::type;

  // std::tuple<float, int...> : current running values of the controls
  controls_values_type controls;

  // bitset : 1 if the control has changed since the last tick, 0 else
  controls_changed_list controls_changed;

  // used to communicate control changes from the ui
  moodycamel::ReaderWriterQueue<controls_values_type> cqueue;

  // holds the std::tuple<timed_vec<float>, ...>
  control_tuple_t control_tuple;



  using control_outs_changed_list = std::bitset<info_functions<Node_T>::control_out_count>;
  using control_outs_type = typename info_functions<Node_T>::control_outs_type;
  using control_outs_values_type = typename info_functions<Node_T>::control_outs_values_type;
  using control_out_tuple_t = typename ossia::apply_type<control_outs_values_type, timed_vec>::type;


  control_outs_values_type control_outs;
  control_outs_changed_list control_outs_changed;
  moodycamel::ReaderWriterQueue<control_outs_values_type> control_outs_queue;
  control_out_tuple_t control_outs_tuple;

  std::array<ossia::audio_inlet, info::audio_in_count> audio_in_ports;
  std::array<ossia::midi_inlet, info::midi_in_count> midi_in_ports;
  std::array<ossia::value_inlet, info::value_in_count> value_in_ports;
  std::array<ossia::value_inlet, info::address_in_count> address_in_ports;
  std::array<ossia::value_inlet, info::control_count> control_in_ports;

  std::array<ossia::audio_outlet, info::audio_out_count> audio_out_ports;
  std::array<ossia::midi_outlet, info::midi_out_count> midi_out_ports;
  std::array<ossia::value_outlet, info::value_out_count> value_out_ports;
  std::array<ossia::value_outlet, info::control_out_count> control_out_ports;

  safe_node() noexcept
  {
    m_inlets.reserve(info_functions<Node_T>::inlet_size);
    m_outlets.reserve(info_functions<Node_T>::outlet_size);
    if constexpr(info::audio_in_count > 0)
    for (auto& port : this->audio_in_ports)
      m_inlets.push_back(std::addressof(port));

    if constexpr(info::midi_in_count > 0)
    for (auto& port : this->midi_in_ports)
      m_inlets.push_back(std::addressof(port));

    if constexpr(info::value_in_count > 0)
    for (std::size_t i = 0; i < info::value_in_count; i++)
    {
      (*value_in_ports[i]).is_event = Node_T::Metadata::value_ins[i].is_event;
      m_inlets.push_back(std::addressof(value_in_ports[i]));
    }

    if constexpr(info::address_in_count > 0)
    for (auto& port : this->address_in_ports)
      m_inlets.push_back(std::addressof(port));

    if constexpr(info::control_count > 0)
    for (auto& port : this->control_in_ports)
    {
      (*port).is_event = true;
      m_inlets.push_back(std::addressof(port));
    }

    {
      int ctrl_i = 0;
      for_each_in_tuple(Node_T::Metadata::controls, [&] (auto& ctrl_info) {
        ctrl_info.setup_exec(this->control_in_ports[ctrl_i]);
        ctrl_i++;
      });
    }

    if constexpr(info::audio_out_count > 0)
    for (auto& port : this->audio_out_ports)
      m_outlets.push_back(std::addressof(port));

    if constexpr(info::midi_out_count > 0)
    for (auto& port : this->midi_out_ports)
      m_outlets.push_back(std::addressof(port));

    if constexpr(info::value_out_count > 0)
    for (auto& port : this->value_out_ports)
      m_outlets.push_back(std::addressof(port));

    if constexpr(info::control_out_count > 0)
    for (auto& port : this->control_out_ports)
      m_outlets.push_back(std::addressof(port));
  }

  template <std::size_t N>
  constexpr const auto& get_control_accessor (const ossia::inlets& inl) noexcept
  {
    constexpr const auto idx = info::control_start + N;
    static_assert(info::control_count > 0);
    static_assert(N < info::control_count);

    using control_type = typename std::tuple_element<N, decltype(Node_T::Metadata::controls)>::type;
    using val_type = typename control_type::type;

    ossia::safe_nodes::timed_vec<val_type>& vec
        = std::get<N>(this->control_tuple);
    vec.clear();
    const auto& vp
        = static_cast<ossia::value_inlet*>(inl[idx])->data.get_data();
    vec.container.reserve(vp.size() + 1);

    // in all cases, set the current value at t=0
    vec.insert(std::make_pair(int64_t{0}, std::get<N>(this->controls)));

    // copy all the values... values arrived later replace previous ones
    apply_control<control_type::must_validate, N>(vec, vp);

    // the last value will be the first for the next tick
    std::get<N>(this->controls) = vec.rbegin()->second;
    return vec;
  }

  template <std::size_t N>
  constexpr auto& get_control_outlet_accessor (const ossia::outlets& outl) noexcept
  {
    static_assert(info::control_out_count > 0);
    static_assert(N < info::control_out_count);

    return std::get<N>(this->control_outs_tuple);
  }

  template <bool Validate, std::size_t N, typename Vec, typename Vp>
  void apply_control(Vec& vec, const Vp& vp) noexcept
  {
    constexpr const auto ctrl = std::get<N>(Node_T::Metadata::controls);
    if constexpr(Validate)
    {
      for (auto& v : vp)
      {
        if (auto res = ctrl.fromValue(v.value))
        {
          vec[int64_t{v.timestamp}] = *std::move(res);
          this->controls_changed.set(N);
        }
      }
    }
    else
    {
      for (auto& v : vp)
      {
        vec[int64_t{v.timestamp}] = ctrl.fromValue(v.value);
        this->controls_changed.set(N);
      }
    }
  }
  /////////////////

  template <std::size_t... I, std::size_t... CI, std::size_t... O, std::size_t... CO>
  void
    apply_all_impl(
      const std::index_sequence<I...>&,
      const std::index_sequence<CI...>&,
      const std::index_sequence<O...>&,
      const std::index_sequence<CO...>&,
      ossia::token_request tk,
      ossia::exec_state_facade st) noexcept
  {
    ossia::inlets& inlets = this->root_inputs();
    ossia::outlets& outlets = this->root_outputs();

    if constexpr (has_state)
    {
      if constexpr (info::control_count > 0)
      {
        using policy = typename Node_T::control_policy;
        policy{}([&](const ossia::token_request& sub_tk, auto&& ... ctls) {
          Node_T::run(
            get_inlet_accessor<info, I>(inlets)...,
            std::forward<decltype(ctls)>(ctls)...,
            get_outlet_accessor<info, O>(outlets)...,
            get_control_outlet_accessor<CO>(outlets)...,
            sub_tk, st, static_cast<state_type&>(*this)
          );
          }, tk, get_control_accessor<CI>(inlets)...);
      }
      else
      {
        Node_T::run(
          get_inlet_accessor<info, I>(inlets)...,
          get_outlet_accessor<info, O>(outlets)...,
          get_control_outlet_accessor<CO>(outlets)...,
          tk, st, static_cast<state_type&>(*this)
        );
      }
    }
    else
    {
      if constexpr (info::control_count > 0)
      {
        using policy = typename Node_T::control_policy;
        policy{}([&](const ossia::token_request& sub_tk, auto&& ... ctls) {
          Node_T::run(
            get_inlet_accessor<info, I>(inlets)...,
            std::forward<decltype(ctls)>(ctls)...,
            get_outlet_accessor<info, O>(outlets)...,
            get_control_outlet_accessor<CO>(outlets)...,
            sub_tk, st
          );
          }, tk, get_control_accessor<CI>(inlets)...);
      }
      else
      {
        Node_T::run(
          get_inlet_accessor<info, I>(inlets)...,
          get_outlet_accessor<info, O>(outlets)...,
          get_control_outlet_accessor<CO>(outlets)...,
          tk, st
        );
      }
    }
  }



  void
  run(const ossia::token_request& tk, ossia::exec_state_facade st) noexcept override
  {
    using inlets_indices = std::make_index_sequence<info::control_start>;
    using controls_indices = std::make_index_sequence<info::control_count>;
    using outlets_indices = std::make_index_sequence<info::control_out_start>;
    using control_outs_indices = std::make_index_sequence<info::control_out_count>;

    apply_all_impl(inlets_indices{}, controls_indices{}, outlets_indices{}, control_outs_indices{}, tk, st);

    if constexpr(info::control_count > 0)
    {
      if (cqueue.size_approx() < 1 && controls_changed.any())
      {
        cqueue.try_enqueue(controls);
        controls_changed.reset();
      }
    }

    if constexpr(info::control_out_count > 0)
    {
      std::size_t i = 0;
      bool ok = false;
      ossia::for_each_in_tuples2(this->control_outs_tuple, this->control_outs, [&] (auto&& control_in, auto&& control_out) {
        if(!control_in.empty())
        {
          ok = true;
          control_out = std::move(control_in.container.back().second);
          control_in.clear();
        }

        i++;
      });

      if(ok)
      {
        this->control_outs_queue.emplace(this->control_outs);
      }
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
