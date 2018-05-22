#pragma once
#include <ossia/dataflow/fx_node.hpp>
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/dataflow/node_process.hpp>
#include <ossia/dataflow/safe_nodes/node.hpp>
#include <ossia/dataflow/safe_nodes/tick_policies.hpp>
#include <readerwriterqueue.h>
#include <bitset>

namespace ossia::safe_nodes
{

template<typename T, typename = void>
struct has_event_policy : std::false_type { };
template<typename T>
struct has_event_policy<T, std::void_t<typename T::event_policy>> : std::true_type { };

template<typename T, typename = void>
struct has_audio_policy : std::false_type { };
template<typename T>
struct has_audio_policy<T, std::void_t<typename T::audio_policy>> : std::true_type { };

template<typename T, typename = void>
struct has_control_policy : std::false_type { };
template<typename T>
struct has_control_policy<T, std::void_t<typename T::control_policy>> : std::true_type { };

template<typename Node_T>
class safe_node final :
    public ossia::graph_node
    , public get_state<Node_T>::type
{
public:
  using info = info_functions<Node_T>;
  static const constexpr bool has_state = has_state_t<Node_T>::value;
  using state_type = typename get_state<Node_T>::type;

  using controls_changed_list = std::bitset<info_functions<Node_T>::control_count>;
  using controls_type = typename info_functions<Node_T>::controls_type;
  using controls_values_type = typename info_functions<Node_T>::controls_values_type;
  controls_values_type controls;
  controls_changed_list controls_changed;
  moodycamel::ReaderWriterQueue<controls_values_type> cqueue;

  template<std::size_t N>
  using timed_vec_t = ossia::safe_nodes::timed_vec<typename std::tuple_element<N, controls_type>::type::type>;

  template <std::size_t... I>
  static constexpr auto get_control_accessor_types(const std::index_sequence<I...>& )
  {
    return std::tuple<timed_vec_t<I>...>{};
  }

  using control_tuple_t = decltype(get_control_accessor_types(std::make_index_sequence<ossia::safe_nodes::info_functions<Node_T>::control_count>()));
  control_tuple_t control_tuple;

  safe_node()
  {
    m_inlets.reserve(info_functions<Node_T>::inlet_size);
    m_outlets.reserve(info_functions<Node_T>::outlet_size);
    for(std::size_t i = 0; i < info_functions<Node_T>::audio_in_count; i++)
    {
      m_inlets.push_back(ossia::make_inlet<ossia::audio_port>());
    }
    for(std::size_t i = 0; i < info_functions<Node_T>::midi_in_count; i++)
    {
      m_inlets.push_back(ossia::make_inlet<ossia::midi_port>());
    }
    if constexpr(info_functions<Node_T>::value_in_count > 0)
    {
      constexpr const auto inlet_infos = get_ports<value_in, Node_T>{}();
      for(std::size_t i = 0; i < info_functions<Node_T>::value_in_count; i++)
      {
        auto inlt = ossia::make_inlet<ossia::value_port>();

        inlt->data.target<ossia::value_port>()->is_event = inlet_infos[i].is_event;
        m_inlets.push_back(std::move(inlt));
      }
    }
    for(std::size_t i = 0; i < info_functions<Node_T>::address_in_count; i++)
    {
      m_inlets.push_back(ossia::make_inlet<ossia::value_port>());
    }
    for(std::size_t i = 0; i < info_functions<Node_T>::control_count; i++)
    {
      auto inlt = ossia::make_inlet<ossia::value_port>();

      inlt->data.target<ossia::value_port>()->is_event = true;

      m_inlets.push_back(std::move(inlt));
    }

    for(std::size_t i = 0; i < info_functions<Node_T>::audio_out_count; i++)
    {
      m_outlets.push_back(ossia::make_outlet<ossia::audio_port>());
    }
    for(std::size_t i = 0; i < info_functions<Node_T>::midi_out_count; i++)
    {
      m_outlets.push_back(ossia::make_outlet<ossia::midi_port>());
    }
    for(std::size_t i = 0; i < info_functions<Node_T>::value_out_count; i++)
    {
      m_outlets.push_back(ossia::make_outlet<ossia::value_port>());
    }
  }

  template<std::size_t N>
  static constexpr auto get_inlet_accessor()
  {
    constexpr auto cat = info::categorize_inlet(N);
    if constexpr(cat == ossia::safe_nodes::inlet_kind::audio_in)
        return [] (const ossia::inlets& inl) -> const ossia::audio_port& { return *inl[N]->data.target<ossia::audio_port>(); };
    else if constexpr(cat == ossia::safe_nodes::inlet_kind::midi_in)
        return [] (const ossia::inlets& inl) -> const ossia::midi_port& { return *inl[N]->data.target<ossia::midi_port>(); };
    else if constexpr(cat == ossia::safe_nodes::inlet_kind::value_in)
        return [] (const ossia::inlets& inl) -> const ossia::value_port& { return *inl[N]->data.target<ossia::value_port>(); };
    else if constexpr(cat == ossia::safe_nodes::inlet_kind::address_in)
        return [] (const ossia::inlets& inl) -> const ossia::destination_t& { return inl[N]->address; };
    else
        throw;
  }


#if defined(_MSC_VER)
#define MSVC_CONSTEXPR const
#else
#define MSVC_CONSTEXPR constexpr
#endif

template<bool Validate, std::size_t N>
struct apply_control;

template<std::size_t N>
struct apply_control<true, N>
{
    template<typename Vec, typename Vp>
    void operator()(Vec& vec, safe_node& self, const Vp& vp)
    {
        constexpr const auto ctrls = get_controls<Node_T>{}();
        MSVC_CONSTEXPR auto ctrl = std::get<N>(ctrls);
        for (auto& v : vp)
        {
            if (auto res = ctrl.fromValue(v.value))
            {
                vec[int64_t{ v.timestamp }] = *std::move(res);
                self.controls_changed.set(N);
            }
        }
    }
};
template<std::size_t N>
struct apply_control<false, N>
{
    template<typename Vec, typename Vp>
    void operator()(Vec& vec, safe_node& self, const Vp& vp)
    {
        constexpr const auto ctrls = get_controls<Node_T>{}();
        MSVC_CONSTEXPR auto ctrl = std::get<N>(ctrls);
        for (auto& v : vp)
        {
            vec[int64_t{ v.timestamp }] = ctrl.fromValue(v.value);
            self.controls_changed.set(N);
        }
    }
};
  template<std::size_t N>
  static constexpr auto get_control_accessor()
  {
    return [] (const ossia::inlets& inl, safe_node& self) -> const auto& {
      constexpr const auto idx = info::control_start + N;
      static_assert(info::control_count > 0);
      static_assert(N < info::control_count);

      using control_type = typename std::tuple_element<N, decltype(get_controls<Node_T>{}())>::type;
      using val_type = typename control_type::type;

      ossia::safe_nodes::timed_vec<val_type>& vec = std::get<N>(self.control_tuple);
      vec.clear();
      const auto& vp = inl[idx]->data.template target<ossia::value_port>()->get_data();
      vec.reserve(vp.size() + 1);

      // in all cases, set the current value at t=0
      vec.insert(std::make_pair(int64_t{0}, std::get<N>(self.controls)));

      // copy all the values... values arrived later replace previous ones
      apply_control<control_type::must_validate, N>{}(vec, self, vp);

      // the last value will be the first for the next tick
      std::get<N>(self.controls) = vec.rbegin()->second;
      return vec;
    };
  }

  template<std::size_t N>
  static constexpr auto get_outlet_accessor()
  {
    if constexpr(N < info::audio_out_count)
        return [] (const ossia::outlets& inl) -> ossia::audio_port& { return *inl[N]->data.target<ossia::audio_port>(); };
    else if constexpr(N < (info::audio_out_count + info::midi_out_count))
        return [] (const ossia::outlets& inl) -> ossia::midi_port& { return *inl[N]->data.target<ossia::midi_port>(); };
    else if constexpr(N < (info::audio_out_count + info::midi_out_count + info::value_out_count))
        return [] (const ossia::outlets& inl) -> ossia::value_port& { return *inl[N]->data.target<ossia::value_port>(); };
    else
        throw;
  }

  template <class F, std::size_t... I>
  static constexpr void apply_inlet_impl(const F& f, const std::index_sequence<I...>& )
  {
    f(get_inlet_accessor<I>()...);
  }

  template <class F, std::size_t... I>
  static constexpr void apply_outlet_impl(const F& f, const std::index_sequence<I...>& )
  {
    f(get_outlet_accessor<I>()...);
  }

  template <class F, std::size_t... I>
  static constexpr void apply_control_impl(const F& f, const std::index_sequence<I...>& )
  {
    f(get_control_accessor<I>()...);
  }

  /////////////////

  template<typename T1, typename T3, typename Seq1, typename Seq3>
  struct forwarder;

  template<typename T1, typename T3, std::size_t... N1, std::size_t... N3>
  struct forwarder<T1, T3, std::index_sequence<N1...>, std::index_sequence<N3...>>
  {
      T1& a1;
      T3& a3;
      ossia::execution_state& st;
      template<typename... Args>
      void operator()(const ossia::token_request& sub_tk,
                      Args&&... args)
      {
        Node_T::run(
              std::get<N1>(std::forward<T1>(a1))...,
              std::forward<Args>(args)...,
              std::get<N3>(std::forward<T3>(a3))...,
              sub_tk, st);

      }

  };

  template<typename T1, typename T3, typename State, typename Seq1, typename Seq3>
  struct forwarder_state;

  template<typename T1, typename T3, typename State, std::size_t... N1, std::size_t... N3>
  struct forwarder_state<T1, T3, State, std::index_sequence<N1...>, std::index_sequence<N3...>>
  {
      T1& a1;
      T3& a3;
      ossia::execution_state& st;
      State& s;
      template<typename... Args>
      void operator()(const ossia::token_request& sub_tk,
                      Args&&... args)
      {
        Node_T::run(
              std::get<N1>(std::forward<T1>(a1))...,
              std::forward<Args>(args)...,
              std::get<N3>(std::forward<T3>(a3))...,
              sub_tk, st, s);
      }

  };
  // Expand three tuples and apply a function on the control tuple
  template<typename F, typename T1, typename T2, typename T3, std::size_t... N1, std::size_t... N2, std::size_t... N3>
  static constexpr auto invoke_impl(F&& f, T1&& a1, T2&& a2, T3&& a3,
                                    const std::index_sequence<N1...>& n1,
                                    const std::index_sequence<N2...>& n2,
                                    const std::index_sequence<N3...>& n3,
                                    const ossia::token_request& tk,
                                    ossia::execution_state& st)
  {
    f(forwarder<T1, T3, std::index_sequence<N1...>, std::index_sequence<N3...>>{a1, a3, st}, tk, std::get<N2>(std::forward<T2>(a2))...);
  }

  // Expand three tuples and apply a function on the control tuple
  template<typename F, typename T1, typename T2, typename T3, std::size_t... N1, std::size_t... N2, std::size_t... N3, typename State>
  static constexpr auto invoke_impl(F&& f, T1&& a1, T2&& a2, T3&& a3,
                                    const std::index_sequence<N1...>& n1,
                                    const std::index_sequence<N2...>& n2,
                                    const std::index_sequence<N3...>& n3,
                                    const ossia::token_request& tk,
                                    ossia::execution_state& st,
                                    State& s)
  {
    f(forwarder_state<T1, T3, State, std::index_sequence<N1...>, std::index_sequence<N3...>>{a1, a3, st, s}, tk, std::get<N2>(std::forward<T2>(a2))...);
  }

  template<typename F, typename T1, typename T2, typename T3, typename... Args>
  static constexpr auto invoke(
      F&& f,
        T1&& a1, T2&& a2, T3&& a3,
        Args&&... args)
  {
    using I1 = std::make_index_sequence<std::tuple_size_v<std::decay_t<T1>>>;
    using I2 = std::make_index_sequence<std::tuple_size_v<std::decay_t<T2>>>;
    using I3 = std::make_index_sequence<std::tuple_size_v<std::decay_t<T3>>>;

    return invoke_impl(f, std::forward<T1>(a1), std::forward<T2>(a2), std::forward<T3>(a3),
                       I1{}, I2{}, I3{}, std::forward<Args>(args)...);
  }

  void run(ossia::token_request tk, ossia::execution_state& st) override
  {
    using inlets_indices = std::make_index_sequence<info::control_start>;
    using controls_indices = std::make_index_sequence<info::control_count>;
    using outlets_indices = std::make_index_sequence<info::outlet_size>;

    ossia::inlets& inlets = this->inputs();
    ossia::outlets& outlets = this->outputs();

    if constexpr(has_state)
    {
      if constexpr(info::control_count > 0) {
        // from this, create tuples of functions
        // apply the functions to inlets and outlets
        apply_inlet_impl(
              [&] (auto&&... i) {
          apply_control_impl(
                [&] (auto&&... c) {
            apply_outlet_impl(
                  [&] (auto&&... o) {
              invoke(typename Node_T::control_policy{}, std::tie(i(inlets)...), std::tie(c(inlets, *this)...), std::tie(o(outlets)...),
                       tk, st, static_cast<state_type&>(*this));
            }, outlets_indices{});
          }, controls_indices{});
        }, inlets_indices{});
      }
      else
      {
        apply_inlet_impl(
              [&] (auto&&... i) {
          apply_outlet_impl(
                [&] (auto&&... o) {
            Node_T::run(i(inlets)..., o(outlets)...,
                      tk, st, static_cast<state_type&>(*this));
          }, outlets_indices{});
        }, inlets_indices{});
      }
    }
    else
    {
      if constexpr(info::control_count > 0) {
        // from this, create tuples of functions
        // apply the functions to inlets and outlets
        apply_inlet_impl(
              [&] (auto&&... i) {
          apply_control_impl(
                [&] (auto&&... c) {
            apply_outlet_impl(
                  [&] (auto&&... o) {
              invoke(typename Node_T::control_policy{}, std::tie(i(inlets)...), std::tie(c(inlets, *this)...), std::tie(o(outlets)...), tk, st);
            }, outlets_indices{});
          }, controls_indices{});
        }, inlets_indices{});
      }
      else
      {
        apply_inlet_impl(
              [&] (auto&&... i) {
          apply_outlet_impl(
                [&] (auto&&... o) {
            Node_T::run(i(inlets)..., o(outlets)..., tk, st);
          }, outlets_indices{});
        }, inlets_indices{});
      }
    }

    if(cqueue.size_approx() < 1 && controls_changed.any())
    {
      cqueue.try_enqueue(controls);
      controls_changed.reset();
    }
  }

  void all_notes_off() override
  {
    if constexpr(info::midi_in_count > 0)
    {
      // TODO
    }
  }

  std::string label() const override
  {
    return "Control";
  }
};

struct value_adder
{
    ossia::value_port& port;
    ossia::value v;
    void operator()() {
      // timestamp should be > all others so that it is always active ?
      port.add_raw_value(std::move(v));
    }
};

template<typename T>
struct control_updater
{
    T& control;
    T v;
    void operator()() {
      control = std::move(v);
    }
};



}
