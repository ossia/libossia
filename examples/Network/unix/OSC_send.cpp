#include <ossia/network/osc/osc_unix.hpp>
#include <ossia/network/context.hpp>
#include <ossia/network/common/debug.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/base/parameter_data.hpp>
#include <boost/lexical_cast.hpp>

#include <iostream>
#include <memory>
#include <functional>

int main(int argc, char** argv)
{
  auto ctx = std::make_shared<ossia::net::network_context>();


  ossia::net::generic_device device{
        std::make_unique<ossia::net::osc_unix_client>(ctx, "ossia_echo"),
        "P"};

  ossia::net::full_parameter_data dat;
  //while (true)
  {
    std::string s = "/foo list: [int: 123, float: 4.56]";
    //std::getline(std::cin, s);

    const auto sep = s.find_first_of(' ');
    if (std::string::npos != sep)
    {
      dat.address = s.substr(0, sep);
      dat.set_value(ossia::parse_pretty_value(s.substr(sep + 1)));
    }

    device.get_protocol().push_raw(dat);
  }
}



