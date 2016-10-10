#include <ossia/editor/state/message.hpp>
#include <ossia/network/base/address.hpp>
#include <ossia/editor/state/detail/state_flatten_visitor.hpp>

namespace ossia
{
void message::launch() const
{
  if(destination.index.empty())
    destination.value.get().pushValue(value);
  else
  {
    auto cur = destination.value.get().cloneValue();
    auto cur_t = cur.getType();
    switch(cur_t)
    {
      case ossia::val_type::VEC2F:
      case ossia::val_type::VEC3F:
      case ossia::val_type::VEC4F:
      {
        eggs::variants::apply(
              vec_merger{destination, destination, unit},
              cur.v,
              value.v);

        destination.value.get().pushValue(std::move(cur));
        break;
      }
      case ossia::val_type::TUPLE:
      {
        auto& cur_tuple = cur.get<Tuple>();
        // Insert the value of this message in the existing value array
        value_merger<true>::insert_in_tuple(cur_tuple, value, destination.index);
        destination.value.get().pushValue(std::move(cur));
        break;
      }
      default:
      {
        // Create a tuple and put the existing value at [0]
        Tuple t{std::move(cur)};
        value_merger<true>::insert_in_tuple(t, value, destination.index);
        destination.value.get().pushValue(std::move(t));
        break;
      }
    }

  }
}

void piecewise_message::launch() const
{
  // If values are missing, merge with the existing ones
  auto cur = address.get().cloneValue();
  if(auto cur_tuple = cur.try_get<Tuple>())
  {
    value_merger<true>::merge_tuple(*cur_tuple, value);
    address.get().pushValue(std::move(cur));
  }
  else
  {
    address.get().pushValue(value);
  }
}

template<int N>
void piecewise_vec_message<N>::launch() const
{
  // TODO
}

template class piecewise_vec_message<2>;
template class piecewise_vec_message<3>;
template class piecewise_vec_message<4>;

}
