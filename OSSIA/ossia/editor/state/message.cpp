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
    if(auto cur_tuple = cur.try_get<Tuple>())
    {
      // Insert the value of this message in the existing value array
      value_merger<true>::insert_in_tuple(*cur_tuple, value, destination.index);
      destination.value.get().pushValue(std::move(cur));
    }
    else
    {
      // Create a tuple and put the existing value at [0]
      Tuple t{std::move(cur)};
      value_merger<true>::insert_in_tuple(t, value, destination.index);
      destination.value.get().pushValue(std::move(t));
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

}
