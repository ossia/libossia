#include <ossia/network/osc/osc_factory.hpp>
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

  using conf = ossia::net::osc_protocol_configuration;
  ossia::net::generic_device device{ossia::net::make_osc_protocol(ctx,
          {
            conf::UNIX,
            conf::CLIENT,
            conf::OSC1_1,
            ossia::net::fd_configuration{"/tmp/ossia_echo.server.socket","/tmp/ossia_echo.client.socket"}
          }),
        "P"};

  ossia::net::full_parameter_data dat;

  {
    std::string s = "/foo list: [int: 123, float: 4.56]";

    const auto sep = s.find_first_of(' ');
    if (std::string::npos != sep)
    {
      dat.address = s.substr(0, sep);
      dat.set_value(ossia::parse_pretty_value(s.substr(sep + 1)));
    }

    device.get_protocol().push_raw(dat);
  }
}



