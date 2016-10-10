#include <ossia/editor/state/message.hpp>
#include <ossia/network/base/address.hpp>
#include <ossia/editor/state/detail/state_flatten_visitor.hpp>
#include <ossia/editor/dataspace/dataspace_visitors.hpp>
namespace ossia
{
void message::launch() const
{
  ossia::net::address_base& addr = destination.value.get();
  auto addr_unit = addr.getUnit();
  if(destination.index.empty())
  {
    if(!unit || unit == addr_unit)
    {
      addr.pushValue(value);
    }
    else
    {
      // Convert from this message's unit to the address's unit
      addr.pushValue(
            ossia::to_value(
              ossia::convert(
                ossia::make_value(value, unit),
                addr_unit)));
    }
  }
  else
  {
    if(!unit || unit == addr_unit)
    {
      auto cur = addr.cloneValue();
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

          addr.pushValue(std::move(cur));
          break;
        }
        case ossia::val_type::TUPLE:
        {
          auto& cur_tuple = cur.get<Tuple>();
          // Insert the value of this message in the existing value array
          value_merger<true>::insert_in_tuple(cur_tuple, value, destination.index);
          addr.pushValue(std::move(cur));
          break;
        }
        default:
        {
          // Create a tuple and put the existing value at [0]
          Tuple t{std::move(cur)};
          value_merger<true>::insert_in_tuple(t, value, destination.index);
          addr.pushValue(std::move(t));
          break;
        }
      }
    }
    else
    {
      {
        auto v1 = ossia::net::get_value(addr);
        auto v2 =
            ossia::convert(
              v1,
              unit);
        auto v3 =
            ossia::merge(
              v2,
              value,
              destination.index);
        auto v4 =
            ossia::convert(
              v3, addr_unit);
        auto v5 =
            ossia::to_value(v4);

        std::cerr << ossia::value_to_pretty_string(value) << "\n";
        std::cerr << ossia::to_pretty_string(v1) << "\n";
        std::cerr << ossia::to_pretty_string(v2) << "\n";
        std::cerr << ossia::to_pretty_string(destination.index) << "\n";
        std::cerr << ossia::to_pretty_string(v3) << "\n";
        std::cerr << ossia::to_pretty_string(v4) << "\n";
        std::cerr << ossia::value_to_pretty_string(v5) << "\n\n\n";
      }
      addr.pushValue(
            ossia::to_value(
              ossia::convert(
                ossia::merge(
                  ossia::convert(
                    ossia::net::get_value(addr),
                    unit),
                  value,
                  destination.index),
                addr_unit)
              )
            );
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
  // piecewise_vec_message's basis is the current address value so
  // we can just push it, with a potential conversion

  ossia::net::address_base& addr = address.get();
  auto addr_unit = addr.getUnit();
  if(unit == addr_unit)
  {
    if(used_values.all())
    {
      addr.pushValue(value);
    }
    else
    {
      auto val = addr.cloneValue();
      switch(val.getType())
      {
        case ossia::val_type::VEC2F:
        {
          auto& v = val.get<Vec2f>();
          for(int i = 0; i < v.size_value; i++)
          {
            if(used_values.test(i))
            {
              v.value[i] = value.value[i];
            }
          }
          break;
        }
        case ossia::val_type::VEC3F:
        {
          auto& v = val.get<Vec3f>();
          for(int i = 0; i < v.size_value; i++)
          {
            if(used_values.test(i))
            {
              v.value[i] = value.value[i];
            }
          }
          break;
        }
        case ossia::val_type::VEC4F:
        {
          auto& v = val.get<Vec4f>();
          for(int i = 0; i < v.size_value; i++)
          {
            if(used_values.test(i))
            {
              v.value[i] = value.value[i];
            }
          }
          break;
        }
        default:
          break;
      }
      addr.pushValue(val);
    }
  }
  else
  {
    addr.pushValue(
          ossia::to_value(
            ossia::convert(
              ossia::make_value(value, unit),
              addr_unit
              )
            )
          );
  }

}

template class piecewise_vec_message<2>;
template class piecewise_vec_message<3>;
template class piecewise_vec_message<4>;

}
