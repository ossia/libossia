#include <ossia/editor/state/message.hpp>
#include <ossia/network/base/address.hpp>
#include <ossia/editor/state/detail/state_flatten_visitor.hpp>
#include <ossia/editor/dataspace/dataspace_visitors.hpp>
#include <ossia/editor/value/value_traits.hpp>
namespace ossia
{
void message::launch() const
{
  ossia::net::address_base& addr = destination.value.get();
  const auto& unit = destination.unit;
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
    if(!unit || !addr_unit || unit == addr_unit)
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
                vec_merger{destination, destination},
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
      /*
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

        std::cerr << "v: " << ossia::value_to_pretty_string(value) << "\n";
        std::cerr << "v1: " << ossia::to_pretty_string(v1) << "\n";
        std::cerr << "v2: " << ossia::to_pretty_string(v2) << "\n";
        std::cerr << "di: " << ossia::to_pretty_string(destination.index) << "\n";
        std::cerr << "v3: " << ossia::to_pretty_string(v3) << "\n";
        std::cerr << "v4: " << ossia::to_pretty_string(v4) << "\n";
        std::cerr << "v5: " << ossia::value_to_pretty_string(v5) << "\n\n\n";
      }
      */

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
  if(auto cur_tuple = cur.target<Tuple>())
  {
    value_merger<true>::merge_tuple(*cur_tuple, value);
    address.get().pushValue(std::move(cur));
  }
  else
  {
    address.get().pushValue(value);
  }
}

template<std::size_t N>
void piecewise_vec_message<N>::launch() const
{
  ossia::net::address_base& addr = address.get();
  auto addr_unit = addr.getUnit();
  if(!unit || !addr_unit || unit == addr_unit)
  {
    if(used_values.all())
    {
      addr.pushValue(value);
    }
    else
    {
      auto val = addr.cloneValue();
      if(val.getType() == ossia::value_trait<Vec<float, N>>::ossia_enum)
      {
        auto& v = val.get<Vec<float, N>>();
        for(std::size_t i = 0; i < N; i++)
        {
          if(used_values.test(i))
          {
            v.value[i] = value.value[i];
          }
        }

        addr.pushValue(val);
      }
    }
  }
  else // unit != addr_unit
  {


    if(used_values.all())
    {
      /*
      {
        auto v1 = ossia::make_value(value, unit);
        auto v2 =
            ossia::convert(
              v1,
              addr_unit);
        auto v3 =
            ossia::to_value(v2);

        std::cout << std::flush;
        std::cerr << std::flush;
        std::cerr << "v: " << ossia::value_to_pretty_string(value) << "\n";
        std::cerr << "v1: " << ossia::to_pretty_string(v1) << "\n";
        std::cerr << "v2: " << ossia::to_pretty_string(v2) << "\n";
        std::cerr << "v3: " << ossia::to_pretty_string(v3) << "\n";
      }
      */

      addr.pushValue(
            ossia::to_value(
              ossia::convert(
                ossia::make_value(value, unit),
                addr_unit
                )
              )
            );
    }
    else
    {
      /*
      {
        auto v1 = ossia::net::get_value(addr);
        auto v2 = convert(v1, unit);
        auto v3 = merge(v2, value, used_values);
        auto v4 = convert(v3, addr.getUnit());
        auto v5 = to_value(v4);

        std::cerr << to_pretty_string(v1) << "\n"
                  << to_pretty_string(v2) << "\n"
                  << to_pretty_string(v3) << "\n"
                  << to_pretty_string(v4) << "\n"
                  << to_pretty_string(v5) << "\n\n\n";
      }
      */


      addr.pushValue(
                to_value( // Go from Unit domain to Value domain
                  convert( // Convert to the resulting address unit
                    merge( // Merge the automation value with the "unit" value
                      convert( // Put the current value in the Unit domain
                        ossia::net::get_value(addr),
                        unit),
                      value, // Compute the output of the automation
                      used_values),
                  addr.getUnit())));

    }
  }
}

template struct piecewise_vec_message<2>;
template struct piecewise_vec_message<3>;
template struct piecewise_vec_message<4>;

}
