#include <iostream>
#include <memory>
#include <functional>

#include <ossia/network/osc/osc.hpp>
#include <ossia/network/common/debug.hpp>
#include <boost/program_options.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/base/parameter_data.hpp>
int main(int argc, char** argv)
{
  int remote_port = 9997;

  if(argc > 1)
  {
    try { remote_port = boost::lexical_cast<int>(argv[1]); } catch(...) {  };
  }

  ossia::net::generic_device device{
        std::make_unique<ossia::net::osc_protocol>("127.0.0.1", remote_port, 0),
        "P"};

  ossia::net::full_parameter_data dat;
  while (true)
  {
    std::string s;
    std::getline(std::cin, s);

    const auto sep = s.find_first_of(' ');
    if (std::string::npos != sep)
    {
        dat.address = s.substr(0, sep);
        dat.set_value(ossia::parse_pretty_value(s.substr(sep + 1)));
    }

    device.get_protocol().push_raw(dat);
  }
}



