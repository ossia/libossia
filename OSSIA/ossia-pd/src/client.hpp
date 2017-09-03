#pragma once
#include <ossia-pd/src/device_base.hpp>
#include <ossia/network/local/local.hpp>
#include <ossia/network/zeroconf/zeroconf.hpp>
#include <ossia/network/oscquery/oscquery_mirror.hpp>

namespace ossia
{
namespace pd
{

class client : public device_base
{
public:
  client();

  static void register_children(client* x);
  void unregister_children();
  static void loadbang(client* x, t_float type);

  static void print_protocol_help()
  {
    post("connect <protocol> <args> ...");
    post("Available protocols (case sensitive): Minuit, oscquery");
    post("Protocols parameters :");
    post(
        "Minuit <remoteip> <remoteport> <localport> :\n"
        "\tremoteip (symbol): ip of target device\n"
        "\tremoteport (float): port on which packet should be send\n"
        "\tlocalport (float): port to which this device is listening");
    post(
        "oscquery <oscport> <wsurl> :\n"
        "\twsurl (symbol) : url to connect to (default : "
        "ws://127.0.0.1:5678)\n");
  }

  std::vector<ossia::net::minuit_connection_data> m_minuit_devices{};
  std::vector<ossia::net::oscquery_connection_data> m_oscq_devices{};

  std::thread* m_async_thread{};

  ossia::oscquery::oscquery_mirror_protocol* m_oscq_protocol{};

  bool m_done{true};
  t_symbol* m_looking_for{}; // the device's name we are looking for
};
}
} // namespace
