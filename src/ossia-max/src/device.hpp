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
  void unregister_children();
  static void loadbang(device*);

  std::vector<std::vector<t_atom>> m_protocols;

  //static void* create(t_symbol* name, int argc, t_atom* argv);
  //static void destroy(device* x);
  static void expose(device* x, t_symbol*, long argc, t_atom* argv);
  static void name(device* x, t_symbol*, long argc, t_atom* argv);
  static void get_protocols(device* x);
  static void stop_expose(device*x, int index);
  static void get_mess_cb(device* x, t_symbol* s);

  static void* create(t_symbol*, long, t_atom*);
  static void destroy(ossia::max::device*);


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
      "\tremoteport (float): port on which packet should be send\n"
      "\tlocalport (float): port to which this device is listening");

  post(
      "oscquery <oscport> <wsport> :\n"
      "\toscport (float) : post on which osc packet are sent\n"
      "\twsport (float) : WebSocket port on which distant application will "
      "connect.");

  post(
      "osc <remoteip> <remoteport> <localpoort>\n"
      "\tremoteip (symbol): ip of target device\n"
      "\tremoteport (float): port on which packet should be send\n"
      "\tlocalport (port): port this device is listening.");
}

} // protocol_setting namespace
} // max namespace
} // ossia namespace

