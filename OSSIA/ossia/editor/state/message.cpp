// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/editor/dataspace/dataspace_visitors.hpp>
#include <ossia/editor/dataspace/value_with_unit.hpp>
#include <ossia/editor/state/detail/state_flatten_visitor.hpp>
#include <ossia/editor/state/message.hpp>
#include <ossia/editor/value/value_traits.hpp>
#include <ossia/network/base/parameter.hpp>
#include <ossia/network/base/node_functions.hpp>
namespace ossia
{
void message::launch() const
{
  ossia::net::parameter_base& addr = dest.value.get();
  const auto& unit = dest.unit;
  auto addr_unit = addr.get_unit();
  if (dest.index.empty())
  {
    if (!unit || unit == addr_unit)
    {
      addr.push_value(message_value);
    }
    else
    {
      // Convert from this message's unit to the address's unit
      addr.push_value(ossia::convert(message_value, unit, addr_unit));
    }
  }
  else
  {
    if (!unit || !addr_unit || unit == addr_unit)
    {
      auto cur = addr.value();
      auto cur_t = cur.getType();
      switch (cur_t)
      {
        case ossia::val_type::VEC2F:
        case ossia::val_type::VEC3F:
        case ossia::val_type::VEC4F:
        {
          ossia::apply(
              vec_merger{dest, dest}, cur.v, message_value.v);

          addr.push_value(std::move(cur));
          break;
        }
        case ossia::val_type::LIST:
        {
          auto& cur_list = cur.get<std::vector<ossia::value>>();
          // Insert the value of this message in the existing value array
          value_merger<true>::insert_in_list(
              cur_list, message_value, dest.index);
          addr.push_value(std::move(cur));
          break;
        }
        default:
        {
          // Create a list and put the existing value at [0]
          std::vector<ossia::value> t{std::move(cur)};
          value_merger<true>::insert_in_list(
              t, message_value, dest.index);
          addr.push_value(std::move(t));
          break;
        }
      }
    }
    else
    {
      addr.push_value(ossia::to_value(ossia::convert(
          ossia::merge(
              ossia::convert(ossia::net::get_value(addr), unit), message_value,
              dest.index),
          addr_unit)));
    }
  }
}

void piecewise_message::launch() const
{
  // If values are missing, merge with the existing ones
  auto cur = address.get().value();
  if (auto cur_list = cur.target<std::vector<ossia::value>>())
  {
    value_merger<true>::merge_list(*cur_list, message_value);
    address.get().push_value(std::move(cur));
  }
  else
  {
    address.get().push_value(message_value);
  }
}

template <std::size_t N>
void piecewise_vec_message<N>::launch() const
{
  ossia::net::parameter_base& addr = address.get();
  auto addr_unit = addr.get_unit();
  if (!unit || !addr_unit || unit == addr_unit)
  {
    if (used_values.all())
    {
      addr.push_value(message_value);
    }
    else
    {
      auto val = addr.value();
      if (val.getType()
          == ossia::value_trait<std::array<float, N>>::ossia_enum)
      {
        auto& v = val.get<std::array<float, N>>();
        for (std::size_t i = 0; i < N; i++)
        {
          if (used_values.test(i))
          {
            v[i] = message_value[i];
          }
        }

        addr.push_value(val);
      }
    }
  }
  else // unit != addr_unit
  {

    if (used_values.all())
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

      addr.push_value(ossia::convert(message_value, unit, addr_unit));
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

      addr.push_value(to_value( // Go from Unit domain to Value domain
          convert(              // Convert to the resulting address unit
              merge(       // Merge the automation value with the "unit" value
                  convert( // Put the current value in the Unit domain
                      ossia::net::get_value(addr), unit),
                  message_value, // Compute the output of the automation
                  used_values),
              addr.get_unit())));
    }
  }
}

template OSSIA_EXPORT void piecewise_vec_message<2>::launch() const;
template OSSIA_EXPORT void piecewise_vec_message<3>::launch() const;
template OSSIA_EXPORT void piecewise_vec_message<4>::launch() const;
}
