#pragma once
#include <ossia/detail/config.hpp>
#if defined(OSSIA_HAS_FMT)
#include <ossia/editor/state/state_element.hpp>
#include <ossia/network/base/node.hpp>
#include <ossia/network/base/node_functions.hpp>
#include <ossia/network/base/osc_address.hpp>
#include <ossia/network/base/parameter.hpp>
#include <ossia/network/dataspace/dataspace_visitors.hpp>
#include <ossia/network/value/value.hpp>

#include <ostream>
#include <string>

namespace ossia
{
struct state_print_visitor
{
  std::ostream& out;
  std::string padding;

  void operator()(const state& s)
  {
    out << padding << "state {\n";
    padding.push_back(' ');
    for(auto& sub : s)
      ossia::apply_nonnull(*this, sub);
    padding.pop_back();
    out << "}\n";
  }

  void operator()(const message& m)
  {
    out << padding << "message: " << ossia::to_pretty_string(m.dest) << " => "
        << ossia::value_to_pretty_string(m.message_value);

    out << "\n";
  }

  void operator()(const piecewise_message& m)
  {
    out << padding
        << "piecewise_message: " << ossia::net::address_string_from_node(m.address.get())
        << " => " << ossia::value_to_pretty_string(m.message_value) << "\n";
  }

  template <std::size_t N>
  void operator()(const piecewise_vec_message<N>& m)
  {
    out << padding << "piecewise_vec_message<" << N
        << ">: " << ossia::net::address_string_from_node(m.address.get()) << " => "
        << ossia::value_to_pretty_string(m.message_value) << "\n";
  }

  void operator()(const ossia::monostate&) { out << "no state\n"; }
};
}
#endif
