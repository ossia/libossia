#pragma once
#include "ossia_obj_base.hpp"
#include <ossia/network/local/local.hpp>
#include <ossia/network/zeroconf/zeroconf.hpp>

namespace ossia
{
namespace pd
{

class t_client : public t_object_base
{
  ossia::net::local_protocol m_local_proto;

public:
  t_client();

  static void register_children(t_client* x);
  void unregister_children();
  static void loadbang(t_client* x, t_float type);

  void on_parameter_created_callback(const ossia::net::parameter_base& param);
  void on_parameter_deleted_callback(const ossia::net::parameter_base& param);

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

  std::vector<ossia::net::minuit_connection_data> m_minuit_devices;
  std::vector<ossia::net::oscquery_connection_data> m_oscq_devices;

  std::thread* m_async_thread;

  bool m_done;
  t_symbol* m_looking_for; // the device's name we are looking for
};
}
} // namespace
