#include <ossia/detail/config.hpp>
#include <ossia/network/osc/osc_unix.hpp>
#include <ossia/network/context.hpp>
#include <ossia/network/common/debug.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <memory>
#include <functional>

int main(int argc, char** argv)
{
  auto ctx = std::make_shared<ossia::net::network_context>();

  ossia::net::generic_device device{
        std::make_unique<ossia::net::osc_unix_server>(ctx, "ossia_echo"),
        "P"};

  auto cb = [&] (ossia::string_view v, const ossia::value& val) {
    std::cout << v << " => " << ossia::value_to_pretty_string(val) << std::endl;
  };
  device.on_unhandled_message.connect(&cb);

  {
    ctx->context.run();
    ctx->context.reset();
  }
}
