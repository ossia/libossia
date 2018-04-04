#include <ossia/detail/config.hpp>
#include <ossia/network/osc/osc.hpp>
#include <ossia/network/common/debug.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <memory>
#include <functional>

int main(int argc, char** argv)
{
  int local_port = 9997;

  if(argc > 1)
  {
    try { local_port = boost::lexical_cast<int>(argv[1]); } catch(...) {  };
  }

  ossia::net::generic_device device{
        std::make_unique<ossia::net::osc_protocol>("", 0, local_port),
        "P"};

  auto cb = [] (ossia::string_view v, const ossia::value& val) {
    std::cout << v << " => " << ossia::value_to_pretty_string(val) << std::endl;
  };
  device.on_unhandled_message.connect(&cb);

  while (true)
    ;
}
