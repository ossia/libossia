#pragma once
#include <iostream>
#include <string>
#include <ossia/editor/state/state_element.hpp>
#include <ossia/network/base/Address.hpp>
#include <ossia/network/base/Node.hpp>

namespace OSSIA
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
                  << OSSIA::net::getAddressFromNode(m.address.get().getNode())
                  << " => "
                  << OSSIA::getValueAsString(m.value)
                  << "\n";
          }
  };
}
