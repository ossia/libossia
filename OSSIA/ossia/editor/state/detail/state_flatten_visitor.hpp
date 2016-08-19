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

  // Const reference overloads
  void operator()(const message& messageToAppend)
  {
    // find message with the same address to replace it
    auto it = find_if(state, [&](const state_element& e)
    {
      auto address = &messageToAppend.destination.value.get();
      if (auto m = e.target<message>())
        return &m->destination.value.get() == address;
      else if(auto p = e.target<piecewise_message>())
        return &p->address.get() == address;
      else
        return false;
    });

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
        if(messageToAppend.destination.index.empty() && source.destination.index.empty())
        {
          // Simple case : we just replace the values
          value_merger<true>::merge_value(source.value, messageToAppend.value);
        }
        else
        {
          piecewise_message pw{messageToAppend.destination.value, {}};
          if(!messageToAppend.destination.index.empty() && !source.destination.index.empty())
          {
            // Most complex case : we create a tuple big enough to host both values
            value_merger<true>::insert_in_tuple(pw.value, source.value, source.destination.index);
            value_merger<true>::insert_in_tuple(pw.value, messageToAppend.value, messageToAppend.destination.index);
          }
          // For these cases, we mix in the one that has the index;
          // the one without index information becomes index [0] :
          else if(!messageToAppend.destination.index.empty())
          {
            pw.value.value.push_back(source.value);
            value_merger<true>::insert_in_tuple(pw.value, messageToAppend.value, messageToAppend.destination.index);
          }
          else if(!source.destination.index.empty())
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
    auto it = find_if(state, [&](const state_element& e)
    {
      ossia::net::address_base* address = &pw.address.get();
      if (auto m = e.target<message>())
        return &m->destination.value.get() == address;
      else if(auto p = e.target<piecewise_message>())
        return &p->address.get() == address;
      else
        return false;
    });

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
    auto it = find_if(state, [&](const state_element& e)
    {
      auto address = &messageToAppend.destination.value.get();
      if (auto m = e.target<message>())
        return &m->destination.value.get() == address;
      else if(auto p = e.target<piecewise_message>())
        return &p->address.get() == address;
      else
        return false;
    });

    if (it == state.end())
    {
      state.add(std::move(messageToAppend));
    }
    else
    {
      // Merge messages
      if(auto source_ptr = it->target<message>())
      {
        message& existing_mess = *source_ptr;
        if(messageToAppend.destination.index.empty() && existing_mess.destination.index.empty())
        {
          // Simple case : we just replace the values
          value_merger<true>::merge_value(existing_mess.value, std::move(messageToAppend.value));
        }
        else
        {
          piecewise_message pw{messageToAppend.destination.value, {}};
          if(!messageToAppend.destination.index.empty() && !existing_mess.destination.index.empty())
          {
            // Most complex case : we create a tuple big enough to host both values
            value_merger<true>::insert_in_tuple(pw.value, existing_mess.value, existing_mess.destination.index);
            value_merger<true>::insert_in_tuple(pw.value, std::move(messageToAppend.value), messageToAppend.destination.index);
          }
          // For these cases, we mix in the one that has the index;
          // the one without index information becomes index [0] :
          else if(!messageToAppend.destination.index.empty())
          {
            pw.value.value.push_back(existing_mess.value);
            value_merger<true>::insert_in_tuple(pw.value, std::move(messageToAppend.value), messageToAppend.destination.index);
          }
          else if(!existing_mess.destination.index.empty())
          {
            value_merger<true>::insert_in_tuple(pw.value, existing_mess.value, existing_mess.destination.index);
            value_merger<true>::set_first_value(pw.value, std::move(messageToAppend.value));
          }

          state.remove(existing_mess);
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
    auto it = find_if(state, [&](const state_element& e)
    {
      ossia::net::address_base* address = &pw.address.get();
      if (auto m = e.target<message>())
        return &m->destination.value.get() == address;
      else if(auto p = e.target<piecewise_message>())
        return &p->address.get() == address;
      else
        return false;
    });

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
