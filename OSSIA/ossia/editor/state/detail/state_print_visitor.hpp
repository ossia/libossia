#pragma once
#include <iostream>
#include <string>
#include <ossia/editor/state/state_element.hpp>
#include <ossia/network/base/address.hpp>
#include <ossia/network/base/node.hpp>

namespace ossia
{
  struct StatePrintVisitor
  {
          std::ostream& out;
          std::string padding;

          void operator()(const State& s)
          {
              out << padding
                  << "state {\n";
              padding.push_back(' ');
              for(auto& sub : s)
                  eggs::variants::apply(*this, sub);
              padding.pop_back();
              out << "}\n";
          }

          void operator()(const CustomState& e)
          {
              out << padding
                  << "custom\n";
          }

          void operator()(const Message& m)
          {
              out << padding
                  << "message: "
                  << ossia::net::getAddressFromNode(m.address.get().getNode())
                  << " => "
                  << ossia::getValueAsString(m.value)
                  << "\n";
          }
  };
}
