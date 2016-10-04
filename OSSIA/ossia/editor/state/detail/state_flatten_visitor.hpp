#pragma once
#include <ossia/detail/algorithms.hpp>
#include <ossia/editor/state/state_element.hpp>
#include <ossia/editor/value/value_algorithms.hpp>
#include <iostream>
namespace ossia
{
struct state_flatten_visitor
{
  ossia::state& state;
  bool same_vec_type(const ossia::value& lhs, const ossia::value& rhs)
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

  auto find_same_address(const message& messageToAppend)
  {
    return find_if(state, [&](const state_element& e)
    {
      auto address = &messageToAppend.destination.value.get();
      if (auto m = e.target<message>())
        return &m->destination.value.get() == address;
      else if(auto p = e.target<piecewise_message>())
        return &p->address.get() == address;
      else
        return false;
    });
  }

  auto find_same_address(const piecewise_message& pw)
  {
    return find_if(state, [&](const state_element& e)
    {
      ossia::net::address_base* address = &pw.address.get();
      if (auto m = e.target<message>())
        return &m->destination.value.get() == address;
      else if(auto p = e.target<piecewise_message>())
        return &p->address.get() == address;
      else
        return false;
    });
  }

  // Const reference overloads
  void operator()(const message& messageToAppend)
  {
    // find message with the same address to replace it
    auto it = find_same_address(messageToAppend);
    if (it == state.end())
    {
      state.add(messageToAppend);
    }
    else
    {
      // Merge messages
      if(auto source_ptr = it->target<message>())
      {
        message& source = *source_ptr;
        auto to_append_index_empty = messageToAppend.destination.index.empty();
        auto source_index_empty = source.destination.index.empty();
        if(to_append_index_empty && source_index_empty)
        {
          // Simple case : we just replace the values
          value_merger<true>::merge_value(source.value, messageToAppend.value);
        }
        else if(same_vec_type(source.value, messageToAppend.value))
        {
          // We handle the Vec types a bit differently :
          // since it's very cheap, the value will contain the whole array data
          // and the index will be the relevant index in the array.
          // Hence we merge both indexes.

          eggs::variants::apply(
                vec_merger{messageToAppend.destination.index},
                source.value.v,
                messageToAppend.value.v);

          // TODO find something better...
          if(source.destination.index != messageToAppend.destination.index)
            source.destination.index.clear();
        }
        else
        {
          piecewise_message pw{messageToAppend.destination.value, {}};
          if(!to_append_index_empty && !source_index_empty)
          {
            // Most complex case : we create a tuple big enough to host both values
            value_merger<true>::insert_in_tuple(pw.value, source.value, source.destination.index);
            value_merger<true>::insert_in_tuple(pw.value, messageToAppend.value, messageToAppend.destination.index);
          }
          // For these cases, we mix in the one that has the index;
          // the one without index information becomes index [0] :
          else if(!to_append_index_empty)
          {
            pw.value.value.push_back(source.value);
            value_merger<true>::insert_in_tuple(pw.value, messageToAppend.value, messageToAppend.destination.index);
          }
          else if(!source_index_empty)
          {
            value_merger<true>::insert_in_tuple(pw.value, source.value, source.destination.index);
            value_merger<true>::set_first_value(pw.value, messageToAppend.value);
          }
          state.remove(source);
          state.add(std::move(pw));

        }
      }
      else if(auto pw_p = it->target<piecewise_message>())
      {
        piecewise_message& pw = *pw_p;
        if(messageToAppend.destination.index.empty())
        {
          // add it at [0]
          value_merger<true>::set_first_value(pw.value, messageToAppend.value);
        }
        else
        {
          // add it wherever possible, by extending the tuple as required
          value_merger<true>::insert_in_tuple(pw.value, messageToAppend.value, messageToAppend.destination.index);
        }
      }
    }
  }

  void operator()(const ossia::piecewise_message& pw)
  {
    auto it = find_same_address(pw);
    if (it == state.end())
    {
      state.add(pw);
    }
    else
    {
      piecewise_message other = pw;
      if(auto mess_ptr = it->target<message>())
      {
        // Merge the message in the piecewise_message and replace it
        // If the message refers to an index in the piecewise_message we merge it
        // unless the piecewise_message also has a
        // value at this index

        if(mess_ptr->destination.index.empty())
        {
          // add it at [0]
          value_merger<false>::set_first_value(other.value, mess_ptr->value);
        }
        else
        {
          // add it wherever possible, by extending the tuple as required
          value_merger<false>::insert_in_tuple(other.value, mess_ptr->value, mess_ptr->destination.index);
        }

        state.remove(*mess_ptr);
        state.remove(pw);
        state.add(std::move(other));
      }
      else if(auto pw_ptr = it->target<piecewise_message>())
      {
        // merge the new piecewise into the existing one
        value_merger<true>::merge_tuple(pw_ptr->value, pw.value);
      }
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

  // rvalue reference overloads
  void operator()(message&& messageToAppend)
  {
    // find message with the same address to replace it
    auto it = find_same_address(messageToAppend);
    if (it == state.end())
    {
      state.add(std::move(messageToAppend));
    }
    else
    {
      // Merge messages
      if(auto source_ptr = it->target<message>())
      {
        message& source = *source_ptr;
        auto to_append_index_empty = messageToAppend.destination.index.empty();
        auto source_index_empty = source.destination.index.empty();
        if(to_append_index_empty && source_index_empty)
        {
          // Simple case : we just replace the values
          value_merger<true>::merge_value(source.value, std::move(messageToAppend.value));
        }
        else if(same_vec_type(source.value, messageToAppend.value))
        {
          // We handle the Vec types a bit differently :
          // since it's very cheap, the value will contain the whole array data
          // and the index will be the relevant index in the array.
          // Hence we merge both indexes.

          eggs::variants::apply(
                vec_merger{messageToAppend.destination.index},
                source.value.v,
                messageToAppend.value.v);

          // TODO find something better...
          if(source.destination.index != messageToAppend.destination.index)
            source.destination.index.clear();
        }
        else
        {
          piecewise_message pw{messageToAppend.destination.value, {}};
          if(!to_append_index_empty && !source_index_empty)
          {
            // Most complex case : we create a tuple big enough to host both values
            value_merger<true>::insert_in_tuple(pw.value, source.value, source.destination.index);
            value_merger<true>::insert_in_tuple(pw.value, std::move(messageToAppend.value), messageToAppend.destination.index);
          }
          // For these cases, we mix in the one that has the index;
          // the one without index information becomes index [0] :
          else if(!to_append_index_empty)
          {
            pw.value.value.push_back(source.value);
            value_merger<true>::insert_in_tuple(pw.value, std::move(messageToAppend.value), messageToAppend.destination.index);
          }
          else if(!source_index_empty)
          {
            value_merger<true>::insert_in_tuple(pw.value, source.value, source.destination.index);
            value_merger<true>::set_first_value(pw.value, std::move(messageToAppend.value));
          }

          state.remove(source);
          state.add(std::move(pw));
        }
      }
      else if(auto pw_p = it->target<piecewise_message>())
      {
        piecewise_message& existing_pw = *pw_p;
        if(messageToAppend.destination.index.empty())
        {
          // add it at [0]
          value_merger<true>::set_first_value(existing_pw.value, std::move(messageToAppend.value));
        }
        else
        {
          // add it wherever possible, by extending the tuple as required
          value_merger<true>::insert_in_tuple(existing_pw.value, std::move(messageToAppend.value), messageToAppend.destination.index);
        }
      }
    }
  }

  void operator()(ossia::piecewise_message&& pw)
  {
    auto it = find_same_address(pw);
    if (it == state.end())
    {
      state.add(std::move(pw));
    }
    else
    {
      piecewise_message other = pw;
      if(auto mess_ptr = it->target<message>())
      {
        // Merge the message in the piecewise_message and replace it
        // If the message refers to an index in the piecewise_message we merge it
        // unless the piecewise_message also has a
        // value at this index

        if(mess_ptr->destination.index.empty())
        {
          // add it at [0]
          value_merger<false>::set_first_value(other.value, mess_ptr->value);
        }
        else
        {
          // add it wherever possible, by extending the tuple as required
          value_merger<false>::insert_in_tuple(other.value, mess_ptr->value, mess_ptr->destination.index);
        }

        state.remove(*mess_ptr);
        state.remove(pw);
        state.add(std::move(other));
      }
      else if(auto pw_ptr = it->target<piecewise_message>())
      {
        // merge the new piecewise into the existing one
        value_merger<true>::merge_tuple(pw_ptr->value, std::move(pw.value));
      }
    }
  }

  void operator()(ossia::state&& s)
  {
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
