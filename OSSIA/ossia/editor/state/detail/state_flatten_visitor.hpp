#pragma once
#include <ossia/detail/algorithms.hpp>
#include <ossia/editor/state/state_element.hpp>
#include <ossia/editor/value/value_algorithms.hpp>
#include <iostream>
namespace ossia
{

static bool same_vec_type(const ossia::value& lhs, const ossia::value& rhs)
{
  const auto first = lhs.getType();
  const auto second = rhs.getType();
  if(first != second)
    return false;

  switch(first)
  {
    case ossia::val_type::VEC2F:
    case ossia::val_type::VEC3F:
    case ossia::val_type::VEC4F:
      return true;
    default:
      return false;
  }
}

struct vec_merger
{
  const ossia::Destination& existing_dest;
  const ossia::Destination& incoming_dest;
  const ossia::unit_t& unit;

  template<typename T, typename U>
  ossia::state_element operator()(const T&, const U&) const
  {
    // Invalid cases.
    // TODO Do nothing, or throw ?
    throw std::runtime_error("vec_merger: invalid case");
    return {};
  }

  template<int N>
  ossia::state_element operator()(ossia::Vec<float, N>& orig, const ossia::Vec<float, N>& incoming) const
  {
    auto& existing_index = existing_dest.index;
    auto& incoming_index = incoming_dest.index;
    if(incoming_index.empty())
    {
      orig.value = incoming.value;
      return {};
    }
    else
    {
      auto i = incoming_index[0];
      if(i < N)
        orig.value[i] = incoming.value[i];

      if(existing_index != incoming_index && !existing_index.empty())
      {
        // Case where we had a message setting the index [0] and another setting the index [2]
        // for instance
        piecewise_vec_message<N> mess{existing_dest.value, orig, unit, {}};
        mess.used_values.set(existing_index[0]);
        mess.used_values.set(i);
        return mess;
      }
      else
      {
        return {};
      }
    }
  }
};

struct state_flatten_visitor_merger
{
  ossia::state& state;

  //// Const reference overloads
  //// Message incoming
  void operator()(message& existing, const message& incoming)
  {
    auto to_append_index_empty = incoming.destination.index.empty();
    auto source_index_empty = existing.destination.index.empty();
    if(same_vec_type(existing.value, incoming.value))
    {
      // We handle the Vec types a bit differently :
      // since it's very cheap, the value will contain the whole array data
      // and the index will be the relevant index in the array.
      // Hence we merge both indexes.
      auto res = eggs::variants::apply(
            vec_merger{existing.destination, incoming.destination, incoming.unit},
            existing.value.v,
            incoming.value.v);

      if(res)
      {
        // TODO do this with iterator instead !!
        state.remove(existing);
        state.add(res);
      }
    }
    else if(to_append_index_empty && source_index_empty)
    {
      // Simple case : we just replace the values
      value_merger<true>::merge_value(existing.value, incoming.value);
    }
    else
    {
      piecewise_message pw{incoming.destination.value, {}, incoming.unit};
      if(!to_append_index_empty && !source_index_empty)
      {
        // Most complex case : we create a tuple big enough to host both values
        value_merger<true>::insert_in_tuple(pw.value, existing.value, existing.destination.index);
        value_merger<true>::insert_in_tuple(pw.value, incoming.value, incoming.destination.index);
      }
      // For these cases, we mix in the one that has the index;
      // the one without index information becomes index [0] :
      else if(!to_append_index_empty)
      {
        pw.value.value.push_back(existing.value);
        value_merger<true>::insert_in_tuple(pw.value, incoming.value, incoming.destination.index);
      }
      else if(!source_index_empty)
      {
        value_merger<true>::insert_in_tuple(pw.value, existing.value, existing.destination.index);
        value_merger<true>::set_first_value(pw.value, incoming.value);
      }
      state.remove(existing);
      state.add(std::move(pw));
    }
  }

  void operator()(piecewise_message& existing, const message& incoming)
  {
    if(incoming.destination.index.empty())
    {
      // add it at [0]
      value_merger<true>::set_first_value(existing.value, incoming.value);
    }
    else
    {
      // add it wherever possible, by extending the tuple as required
      value_merger<true>::insert_in_tuple(existing.value, incoming.value, incoming.destination.index);
    }
  }

  template<int N>
  void operator()(piecewise_vec_message<N>& existing, const message& incoming)
  {
    auto t = incoming.value.getType();
    using vec_type = decltype(existing.value);
    switch(t)
    {
      // incoming is a Float with index
      case ossia::val_type::FLOAT:
      {
        if(!incoming.destination.index.empty())
        {
          auto i = incoming.destination.index[0];
          if(i < N)
          {
            existing.value.value[i] = incoming.value.get<ossia::Float>().value;
            existing.used_values.set(i);
          }
        }
        break;
      }
        // incoming is a Vec<N, float> without index or with index
      case ossia::value_trait<vec_type>::ossia_enum:
      {
        // With index -> set it
        if(!incoming.destination.index.empty())
        {
          auto i = incoming.destination.index[0];
          if(i < N)
          {
            auto& inc = incoming.value.get<vec_type>();
            existing.value.value[i] = inc.value[i];
            existing.used_values.set(i);
          }
        }
        // Without index -> replace everything
        else
        {
          existing.value.value = incoming.value.get<vec_type>().value;
        }
        break;
      }
      default:
        break;
    }
  }


  //// Piecewise Message incoming
  void operator()(message& existing, const piecewise_message& incoming)
  {
    piecewise_message other = incoming;
    // Merge the message in the piecewise_message and replace it
    // If the message refers to an index in the piecewise_message we merge it
    // unless the piecewise_message also has a
    // value at this index

    if(existing.destination.index.empty())
    {
      // add it at [0]
      value_merger<false>::set_first_value(other.value, existing.value);
    }
    else
    {
      // add it wherever possible, by extending the tuple as required
      value_merger<false>::insert_in_tuple(other.value, existing.value, existing.destination.index);
    }

    state.remove(existing);
    state.remove(incoming);
    state.add(std::move(other));

  }

  void operator()(piecewise_message& existing, const piecewise_message& incoming)
  {
    // merge the new piecewise into the existing one
    value_merger<true>::merge_tuple(existing.value, incoming.value);
  }

  //// Piecewise Vec Message incoming
  template<int N>
  void operator()(piecewise_vec_message<N>& existing, const piecewise_vec_message<N>& incoming)
  {
    for(int i = 0; i < N; i++)
    {
      if(incoming.used_values.test(i))
      {
        existing.value.value[i] = incoming.value.value[i];
        existing.used_values.set(i);
      }
    }
  }

  //// R-value reference overloads
  //// Message incoming
  void operator()(message& existing, message&& incoming)
  {
    auto to_append_index_empty = incoming.destination.index.empty();
    auto source_index_empty = existing.destination.index.empty();
    if(same_vec_type(existing.value, incoming.value))
    {
      // We handle the Vec types a bit differently :
      // since it's very cheap, the value will contain the whole array data
      // and the index will be the relevant index in the array.
      // Hence we merge both indexes.
      auto res = eggs::variants::apply(
            vec_merger{existing.destination, incoming.destination, incoming.unit},
            existing.value.v,
            incoming.value.v);

      if(res)
      {
        // TODO do this with iterator instead !!
        state.remove(existing);
        state.add(res);
      }
    }
    else if(to_append_index_empty && source_index_empty)
    {
      // Simple case : we just replace the values
      value_merger<true>::merge_value(existing.value, std::move(incoming.value));
    }
    else
    {
      piecewise_message pw{incoming.destination.value, {}, incoming.unit};
      if(!to_append_index_empty && !source_index_empty)
      {
        // Most complex case : we create a tuple big enough to host both values
        value_merger<true>::insert_in_tuple(pw.value, existing.value, existing.destination.index);
        value_merger<true>::insert_in_tuple(pw.value, std::move(incoming.value), incoming.destination.index);
      }
      // For these cases, we mix in the one that has the index;
      // the one without index information becomes index [0] :
      else if(!to_append_index_empty)
      {
        pw.value.value.push_back(existing.value);
        value_merger<true>::insert_in_tuple(pw.value, std::move(incoming.value), incoming.destination.index);
      }
      else if(!source_index_empty)
      {
        value_merger<true>::insert_in_tuple(pw.value, existing.value, existing.destination.index);
        value_merger<true>::set_first_value(pw.value, std::move(incoming.value));
      }

      state.remove(existing);
      state.add(std::move(pw));
    }
  }
  void operator()(piecewise_message& existing, message&& incoming)
  {
    if(incoming.destination.index.empty())
    {
      // add it at [0]
      value_merger<true>::set_first_value(existing.value, std::move(incoming.value));
    }
    else
    {
      // add it wherever possible, by extending the tuple as required
      value_merger<true>::insert_in_tuple(existing.value, std::move(incoming.value), incoming.destination.index);
    }
  }

  //// Piecewise Message incoming
  void operator()(message& existing, piecewise_message&& incoming)
  {
    piecewise_message other = incoming;
    // Merge the message in the piecewise_message and replace it
    // If the message refers to an index in the piecewise_message we merge it
    // unless the piecewise_message also has a
    // value at this index

    if(existing.destination.index.empty())
    {
      // add it at [0]
      value_merger<false>::set_first_value(other.value, existing.value);
    }
    else
    {
      // add it wherever possible, by extending the tuple as required
      value_merger<false>::insert_in_tuple(other.value, existing.value, existing.destination.index);
    }

    state.remove(existing);
    state.remove(incoming);
    state.add(std::move(other));
  }

  void operator()(piecewise_message& existing, piecewise_message&& incoming)
  {
    // merge the new piecewise into the existing one
    value_merger<true>::merge_tuple(existing.value, std::move(incoming.value));
  }


  //// Piecewise Vec Message incoming : we don't have rvalue overloads since
  //// it's faster to copy.


  //// Error cases:
  template<typename T>
  void operator()(ossia::state&, const T&)
  {
    throw std::runtime_error("state_flatten_visitor_merger: "
                             "impossible case (state <- *");
  }
  template<typename T>
  void operator()(ossia::custom_state&, const T&)
  {
    throw std::runtime_error("state_flatten_visitor_merger: "
                             "impossible case (custom_state <- *");
  }

  template<int M>
  void operator()(message& existing, const piecewise_vec_message<M>& incoming)
  {
    throw std::runtime_error("state_flatten_visitor_merger: "
                             "impossible case (message <- const piecewise_vec_message&");
  }

  template<int M>
  void operator()(piecewise_message& existing, const piecewise_vec_message<M>& incoming)
  {
    throw std::runtime_error("state_flatten_visitor_merger: "
                             "impossible case (piecewise_message <- const piecewise_vec_message&");
  }

  template<int N>
  void operator()(piecewise_vec_message<N>& existing, const piecewise_message& incoming)
  {
    throw std::runtime_error("state_flatten_visitor_merger: "
                             "impossible case (piecewise_vec_message <- const piecewise_message&");
  }
  template<int N>
  void operator()(piecewise_vec_message<N>& existing, piecewise_message&& incoming)
  {
    throw std::runtime_error("state_flatten_visitor_merger: "
                             "impossible case (piecewise_vec_message <- piecewise_message&&");
  }

  template<int N, int M>
  void operator()(piecewise_vec_message<N>& existing, const piecewise_vec_message<M>& incoming)
  {
    throw std::runtime_error("state_flatten_visitor_merger: "
                             "impossible case (piecewise_vec_message<N> <- piecewise_vec_message<M>");
  }
};

struct state_flatten_visitor
{
  ossia::state& state;

  static ossia::net::address_base* address_ptr(const message& m)
  {
    return &m.destination.value.get();
  }

  static ossia::net::address_base* address_ptr(const piecewise_message& m)
  {
    return &m.address.get();
  }

  template<int N>
  static ossia::net::address_base* address_ptr(const piecewise_vec_message<N>& m)
  {
    return &m.address.get();
  }


  template<typename T>
  auto find_same_address(const T& incoming)
  {
    return find_if(state, [&](const state_element& e)
    {
      auto address = address_ptr(incoming);
      if (auto m = e.target<message>())
        return &m->destination.value.get() == address && incoming.unit == m->unit;
      else if(auto p = e.target<piecewise_message>())
        return &p->address.get() == address && incoming.unit == p->unit;
      else if(auto p2 = e.target<piecewise_vec_message<2>>())
        return &p2->address.get() == address && incoming.unit == p2->unit;
      else if(auto p3 = e.target<piecewise_vec_message<3>>())
        return &p3->address.get() == address && incoming.unit == p3->unit;
      else if(auto p4 = e.target<piecewise_vec_message<4>>())
        return &p4->address.get() == address && incoming.unit == p4->unit;
      else
        return false;
    });
  }

  //// const-reference overloads
  template<typename Message_T>
  void operator()(Message_T&& incoming)
  {
    // find message with the same address to replace it
    auto it = find_same_address(incoming);
    if (it == state.end())
    {
      state.add(std::forward<Message_T>(incoming));
    }
    else
    {
      // Merge messages
      eggs::variants::apply([&] (auto& src) {
        state_flatten_visitor_merger{state}(src, std::forward<Message_T>(incoming));
      }, *it);
    }
  }

  void operator()(const ossia::state& s)
  {
    state.reserve(state.size() + s.size());
    for (const auto& e : s)
    {
      flatten_and_filter(state, e);
    }
  }

  void operator()(const custom_state& e)
  {
    state.add(e);
  }

  void operator()(ossia::state&& s)
  {
    state.reserve(state.size() + s.size());
    for (auto&& e : s)
    {
      flatten_and_filter(state, std::move(e));
    }
  }

  void operator()(custom_state&& e)
  {
    state.add(std::move(e));
  }

  void operator()()
  {
  }
};
}
