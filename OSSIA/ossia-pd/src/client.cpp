// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "client.hpp"
#include "model.hpp"
#include "parameter.hpp"
#include "remote.hpp"
#include "view.hpp"
#include "utils.hpp"

#include <ossia/network/osc/osc.hpp>
#include <ossia/network/oscquery/oscquery_mirror.hpp>

#include <functional>
#include <iostream>
#include <memory>

namespace ossia
{
namespace pd
{

static void* client_new(t_symbol* name, int argc, t_atom* argv)
{
  auto& ossia_pd = ossia_pd::instance();
  t_client* x = (t_client*)eobj_new(ossia_pd.client);
  // TODO SANITIZE : Direct leak
  t_binbuf* d = binbuf_via_atoms(argc, argv);

  if (x && d)
  {
    x->x_otype = Type::client;

    x->x_name = gensym("Pd");
    x->x_device = nullptr;
    x->x_node = nullptr;
    x->x_dumpout = outlet_new((t_object*)x, gensym("dumpout"));

    if (argc != 0 && argv[0].a_type == A_SYMBOL)
    {
      x->x_name = atom_getsymbol(argv);
    }

    ebox_attrprocess_viabinbuf(x, d);
  }

  return (x);
}

static void client_free(t_client* x)
{
  x->x_dead = true;
  x->unregister_children();
  if (x->x_device)
    delete (x->x_device);
  x->x_device = nullptr;
  register_quarantinized();
}

static void client_dump(t_client* x)
{
  if (x->x_device)
    obj_namespace(x, x->x_device->get_root_node());
}

void t_client::loadbang(t_client* x, t_float type)
{
  if (LB_LOAD == (int)type)
  {
    register_children(x);
  }
}

void t_client::register_children(t_client* x)
{

  std::vector<t_obj_base*> viewnodes
      = find_child_to_register(x, x->x_obj.o_canvas->gl_list, "ossia.view");
  for (auto v : viewnodes)
  {
    if (v->x_otype == Type::view)
    {
      t_view* view = (t_view*)v;
      view->register_node(x->x_node);
    }
    else if (v->x_otype == Type::remote)
    {
      t_remote* remote = (t_remote*)v;
      remote->register_node(x->x_node);
    }
  }
}

void t_client::unregister_children()
{
  std::vector<t_obj_base*> viewnode
      = find_child_to_register(this, x_obj.o_canvas->gl_list, "ossia.view");
  for (auto v : viewnode)
  {
    if (v->x_otype == Type::view)
    {
      t_view* view = (t_view*)v;
      view->unregister();
    }
    else if (v->x_otype == Type::remote)
    {
      t_remote* remote = (t_remote*)v;
      remote->unregister();
    }
  }
}

static void client_update(t_client* x)
{
  if (x->x_device)
  {
    x->x_device->get_protocol().update(*x->x_device);
    x->x_node = &x->x_device->get_root_node();
    t_client::register_children(x);
  }
}

static void client_connect(t_client* x, t_symbol*, int argc, t_atom* argv)
{

  client_free(x); // uregister and delete x_device
  if (argc && argv->a_type == A_SYMBOL)
  {
    std::string protocol = argv->a_w.w_symbol->s_name;
    if (protocol == "Minuit")
    {
      std::string device_name = "Pd";
      std::string remoteip = "127.0.0.1";
      int remoteport = 6666;
      int localport = 9999;
      Protocol_Settings::minuit settings{};
      argc--;
      argv++;
      if (argc == 4 && argv[0].a_type == A_SYMBOL && argv[1].a_type == A_SYMBOL
          && argv[2].a_type == A_FLOAT && argv[3].a_type == A_FLOAT)
      {
        device_name = atom_getsymbol(argv++)->s_name;
        remoteip = atom_getsymbol(argv++)->s_name;
        remoteport = atom_getfloat(argv++);
        localport = atom_getfloat(argv++);
      }

      try
      {
        x->x_device = new ossia::net::generic_device{
            std::make_unique<ossia::net::minuit_protocol>(
                device_name, remoteip, remoteport, localport),
            x->x_name->s_name};
      }
      catch (const std::exception& e)
      {
        pd_error(x, "%s", e.what());
        return;
      }
      logpost(
          x, 3,
          "New 'Minuit' protocol connected to %s on port %u and listening on "
          "port %u",
          settings.remoteip.c_str(), settings.remoteport, settings.localport);
    }
    else if (protocol == "oscquery")
    {
      argc--;
      argv++;
      std::string wsurl = "ws://127.0.0.1:5678";
      if (argc == 1 && argv[0].a_type == A_SYMBOL)
      {
        wsurl = atom_getsymbol(argv)->s_name;
      }

      try
      {
        auto protocol = new ossia::oscquery::oscquery_mirror_protocol{wsurl};
        x->x_device = new ossia::net::generic_device{
            std::unique_ptr<ossia::net::protocol_base>(protocol), "Pd"};

        std::cout << "connected to device " << x->x_device->get_name()
                  << " on " << wsurl << std::endl;
      }
      catch (const std::exception& e)
      {
        pd_error(x, "%s", e.what());
        return;
      }
      logpost(x, 3, "Connected with 'oscquery' protocol to %s", wsurl.c_str());
    }
    else
    {
      pd_error((t_object*)x, "Unknown protocol: %s", protocol.c_str());
      return;
    }
  }
  else
  {
    t_client::print_protocol_help();
    return;
  }
  client_update(x);
}

extern "C" void setup_ossia0x2eclient(void)
{
  t_eclass* c = eclass_new(
      "ossia.client", (method)client_new, (method)client_free,
      (short)sizeof(t_client), CLASS_DEFAULT, A_GIMME, 0);

  if (c)
  {
    class_addcreator((t_newmethod)client_new,gensym("ø.client"), A_GIMME, 0);

    eclass_addmethod(
        c, (method)t_client::register_children, "register", A_NULL, 0);
    eclass_addmethod(c, (method)client_update, "update", A_NULL, 0);
    eclass_addmethod(c, (method)t_client::loadbang, "loadbang", A_NULL, 0);
    eclass_addmethod(c, (method)client_dump, "dump", A_NULL, 0);
    eclass_addmethod(c, (method)client_connect, "connect", A_GIMME, 0);
    eclass_addmethod(
        c, (method)Protocol_Settings::print_protocol_help, "help", A_NULL, 0);
  }

  auto& ossia_pd = ossia_pd::instance();
  ossia_pd.client = c;

}
} // pd namespace
} // ossia namespace
