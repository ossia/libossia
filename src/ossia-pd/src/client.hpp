#pragma once
#include <ossia/network/local/local.hpp>
#include <ossia/network/oscquery/oscquery_mirror.hpp>
#include <ossia/network/zeroconf/zeroconf.hpp>

#include <ossia-pd/src/device_base.hpp>

namespace ossia::pd
{

class client : public device_base
{
public:
  client();

  using is_client = std::true_type;

  static void register_children(client* x);
  void unregister_children();

  static void print_protocol_help()
  {
    post("connect <protocol> <args> ...");
    post("or");
    post("connect <device_name>");
    post("the later will look for a device with that name on the network");
    post("Available protocols (case insensitive): minuit, oscquery");
    post("Protocols parameters :");
    post(
        "minuit <remoteip> <remoteport> <localport> :\n"
        "\tremoteip (symbol): ip of target device\n"
        "\tremoteport (float): port on which packet should be send\n"
        "\tlocalport (float): port to which this device is listening");
    post(
        "oscquery <oscport> <wsurl> :\n"
        "\twsurl (symbol) : url to connect to (default : "
        "ws://127.0.0.1:5678)\n");
    post(
        "osc <oscport> :\n"
        "\tip (string) : ip to connect to"
        "\toscport (float) : port to connect to");
  }

  std::vector<ossia::net::minuit_connection_data> m_minuit_devices{};
  std::vector<ossia::net::oscquery_connection_data> m_oscq_devices{};

  ossia::oscquery::oscquery_mirror_protocol* m_oscq_protocol{};

  bool m_zeroconf{true}; // true if we should lookup for device name with zeroconf

  bool is_zeroconf() const { return m_zeroconf; }
  std::string get_name() const { return m_name ? std::string(m_name->s_name) : ""; }

  static void get_mess_cb(client* x, t_symbol* s);

  static void disconnect(client* x);
  static void connect(client* x);
  static void connect_mess_cb(client* x, t_symbol*, int argc, t_atom* argv);
  static void update(client* x);
  static void poll_message(client* x);
  static void destroy(client* x);
  static void* create(t_symbol* name, int argc, t_atom* argv);

  int m_argc{};
  t_atom* m_argv{};
};
} // namespace
