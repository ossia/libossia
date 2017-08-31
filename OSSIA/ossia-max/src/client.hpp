#pragma once

#include "ossia_object_base.hpp"
#include <ossia/network/local/local.hpp>
#include <ossia/network/zeroconf/zeroconf.hpp>

namespace ossia
{
namespace max
{

#pragma mark -
#pragma mark t_client structure declaration

struct t_client : t_object_base
{
  ossia::net::local_protocol m_local_proto;

  static void register_children(t_client*);
  void unregister_children();
  static void loadbang(t_client*);

  static void connect(ossia::max::t_client*, t_symbol*, int, t_atom*);
  static void disconnect(ossia::max::t_client*);
  static void update(t_client* x);
  static void getdevices(t_client*x);

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
  t_symbol* m_looking_for; // the name of the device we are looking for

};

} // max namespace
} // ossia namespace

#pragma mark -
#pragma mark ossia_client class declaration

extern "C" {
void* ossia_client_new(t_symbol*, long, t_atom*);
void ossia_client_free(ossia::max::t_client*);
}
