#pragma once

#include <ossia/network/local/local.hpp>
#include <ossia/network/oscquery/oscquery_mirror.hpp>
#include <ossia/network/zeroconf/zeroconf.hpp>

#include <ossia-max/src/device_base.hpp>

namespace ossia
{
namespace max_binding
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
    post(
        "osc <remoteip> <remoteport> <localport> :\n"
        "\tremoteip (symbol): ip of target device\n"
        "\tremoteport (float): port on which packet should be send\n"
        "\tlocalport (float): port to which this device is listening");
  }

  std::vector<ossia::net::minuit_connection_data> m_minuit_devices;
  std::vector<ossia::net::oscquery_connection_data> m_oscq_devices;

  void* m_poll_clock;

  ossia::oscquery::oscquery_mirror_protocol* m_oscq_protocol{};

  bool m_zeroconf{true};
  bool is_zeroconf() const { return m_zeroconf; }
  std::string get_name() const { return m_name ? std::string(m_name->s_name) : ""; }

  void set_feedback(bool b);

  static t_max_err do_notify(client *x, t_symbol *, t_symbol *msg, void *, void *data);

  static void refresh(client*);
  static void connect(client*);
  static void connect_mess_cb(client* x, t_symbol*, int argc, t_atom* argv);
  static void disconnect(client*);
  static void get_devices(client* x);
  static void check_thread_status(client* x);
  static void update(client* x);
  static void poll_message(client* x);
  // static void destroy(client* x);
  // static void* create(t_symbol* name, int argc, t_atom* argv);
  static void find_devices_async(client* x);
  static void assist(client*, void*, long, long, char*);
  static void get_mess_cb(client*, t_symbol*);

  static void* create(t_symbol*, long, t_atom*);
  static void destroy(ossia::max_binding::client*);

  int m_argc{};
  t_atom* m_argv{};
  long m_feedback{1};
};

} // max namespace
} // ossia namespace
