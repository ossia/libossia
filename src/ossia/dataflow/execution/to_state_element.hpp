#pragma once
#include <ossia/dataflow/typed_value.hpp>
#include <ossia/editor/state/message.hpp>

namespace ossia
{

inline ossia::message
to_state_element(ossia::net::parameter_base& p, ossia::typed_value&& v)
{
  ossia::message m{p, std::move(v.value)};
  if(auto u = v.type.target<ossia::unit_t>())
    m.dest.unit = std::move(*u);
  m.dest.index = std::move(v.index);
  return m;
}

inline ossia::message
to_state_element(ossia::net::parameter_base& p, const ossia::typed_value& v)
{
  ossia::message m{p, v.value};
  if(auto u = v.type.target<ossia::unit_t>())
    m.dest.unit = std::move(*u);
  m.dest.index = std::move(v.index);
  return m;
}

}
