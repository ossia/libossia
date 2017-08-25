#pragma once
#include "ossia_obj_base.hpp"

namespace ossia
{
namespace pd
{

struct t_client : t_obj_base
{
  ossia::net::generic_device* x_device{};
  ossia::net::local_protocol x_local_proto;

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
};
}
} // namespace
