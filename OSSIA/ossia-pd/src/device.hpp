#pragma once
#include "ossia_obj_base.hpp"

namespace ossia
{
namespace pd
{

namespace Protocol_Settings
{
struct minuit
{
  std::string remoteip{"locahost"};
  unsigned int remoteport = 13579;
  unsigned int localport = 9998;
};

struct oscquery
{
  unsigned int oscport = 9999;
  unsigned int wsport = 5678;
};

struct osc
{
  std::string remoteip{"localhost"};
  unsigned int remoteport = 9997;
  unsigned int localport = 9996;
};

static void print_protocol_help()
{
  post("expose <protocol> <args> ...");
  post("Available protocols (case sensitive): Minuit, oscquery, osc");
  post("Protocols parameters :");
  post(
      "Minuit <remoteip> <remoteport> <localport> :\n"
      "\tremoteip (symbol): ip of target device\n"
      "\tremoteport (float): port on which packet should be send\n"
      "\tlocalport (float): port to which this device is listening\n"
      "\tdefault sending on port 13579, listening on 9998");
  post(
      "oscquery <oscport> <wsport> :\n"
      "\toscport (float) : port on which osc packet are sent\n"
      "\twsport (float) : WebSocket port on which distant application will "
      "connect.\n"
      "\tdefault ports: OSC 9999, WebSocket 5678");
  post(
      "osc <remoteip> <remoteport> <localpoort>\n"
      "\tremoteip (symbol): ip of target device\n"
      "\tremoteport (float): port on which packet should be send\n"
      "\tlocalport (port): port this device is listening.\n"
      "\tdefault sending on port 9997, listening on 9996");
}
};

struct t_device : t_obj_base
{
  static void register_children(t_device* x);
  void unregister_children();
  static void loadbang(t_device* x, t_float type);

  void on_parameter_created_callback(const ossia::net::parameter_base& param);
  void on_parameter_deleted_callback(const ossia::net::parameter_base& param);

};

void device_expose(t_device* x, t_symbol*, int argc, t_atom* argv);
void device_name(t_device* x, t_symbol*, int argc, t_atom* argv);

}
} // namespace
