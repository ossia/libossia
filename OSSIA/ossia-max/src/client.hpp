#pragma once

#include <ossia-max/src/device_base.hpp>
#include <ossia/network/local/local.hpp>
#include <ossia/network/zeroconf/zeroconf.hpp>
#include <ossia/network/oscquery/oscquery_mirror.hpp>

namespace ossia
{
namespace max
{

#pragma mark -
#pragma mark t_client structure declaration

class client : public device_base
{
public:

  using is_client = std::true_type;

  static void register_children(client*);
  void unregister_children();
  static void loadbang(client*);

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

  ossia::oscquery::oscquery_mirror_protocol* m_oscq_protocol{};

  bool m_done{true};
  t_symbol* m_looking_for{}; // the name of the device we are looking for

  static void connect(ossia::max::client*, t_symbol*, int, t_atom*);
  static void disconnect(ossia::max::client*);
  static void get_devices(client*x);
  static void check_thread_status(client* x);
  static void update(client* x);
  static void poll_message(client* x);
  // static void destroy(client* x);
  // static void* create(t_symbol* name, int argc, t_atom* argv);
  static void find_devices_async(client* x);
  static void assist(client*, void*, long, long, char*);
  static void get_mess_cb(client*, t_symbol*);

  static void* create(t_symbol*, long, t_atom*);
  static void destroy(ossia::max::client*);

};

} // max namespace
} // ossia namespace

