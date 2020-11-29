#pragma once

#include <ossia-max/src/device_base.hpp>
#include <ossia/network/base/protocol.hpp>

namespace ossia
{
namespace max
{

class device : public device_base
{
public:

  using is_device = std::true_type;

  static void register_children(device*);
  static void loadbang(device*);

  static void expose(device* x, t_symbol*, long argc, t_atom* argv);
  static void name(device* x, t_symbol*, long argc, t_atom* argv);
  static void get_protocols(device* x);
  static void stop_expose(device*x, int index);
  static void get_mess_cb(device* x, t_symbol* s);
  static void assist(ossia::max::device*, void*, long, long, char*);
  static void send_raw_osc(device* x, t_symbol* s, int argc, t_atom* argv);
  static void resend_all_values(device* x, t_symbol* s);

  static void* create(t_symbol*, long, t_atom*);
  static void destroy(ossia::max::device*);
  static void class_setup(t_class* c);
};

namespace protocol_settings
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
      "\tremoteport (int): port on which packet should be send\n"
      "\tlocalport (int): port to which this device is listening");

  post(
      "oscquery <oscport> <wsport> :\n"
      "\toscport (int) : port on which osc packet are sent\n"
      "\twsport (int) : WebSocket port on which distant application will "
      "connect.");

  post(
      "osc <remoteip> <remoteport> <localpoort>\n"
      "\tremoteip (symbol): ip of target device\n"
      "\tremoteport (int): port on which packet should be send\n"
      "\tlocalport (int): port this device is listening.");
}

} // protocol_setting namespace
} // max namespace
} // ossia namespace

