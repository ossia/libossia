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

static void client_free(t_client* x)
{
  x->m_dead = true;
  x->unregister_children();
  if (x->m_device)
    delete (x->m_device);
  x->m_device = nullptr;
  ossia_pd::instance().clients.remove_all(x);
  outlet_free(x->m_dumpout);
  register_quarantinized();
}

static void* client_new(t_symbol* name, int argc, t_atom* argv)
{
  auto& ossia_pd = ossia_pd::instance();
  t_client* x = (t_client*)eobj_new(ossia_pd.client);
  // TODO SANITIZE : Direct leak
  t_binbuf* d = binbuf_via_atoms(argc, argv);

  if (x && d)
  {
    ossia_pd.clients.push_back(x);
    x->m_otype = Type::client;

    x->m_name = gensym("Pd");
    x->m_device = nullptr;
    x->m_node = nullptr;
    x->m_parent_node = nullptr;
    x->m_dumpout = outlet_new((t_object*)x, gensym("dumpout"));

    if (argc != 0 && argv[0].a_type == A_SYMBOL)
    {
      x->m_name = atom_getsymbol(argv);
    }

    ebox_attrprocess_viabinbuf(x, d);

    if (find_peer(x))
    {
      error(
            "Only one [ø.device]/[ø.client] intance per patcher is allowed.");
      client_free(x);
      x = nullptr;
    }
  }

  return (x);
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
      = find_child_to_register(x, x->m_obj.o_canvas->gl_list, "ossia.view");
  for (auto v : viewnodes)
  {
    if (v->m_otype == Type::view)
    {
      t_view* view = (t_view*)v;
      view->register_node(x->m_node);
    }
    else if (v->m_otype == Type::remote)
    {
      t_remote* remote = (t_remote*)v;
      remote->register_node(x->m_node);
    }
  }
}

void t_client::unregister_children()
{
  std::vector<t_obj_base*> viewnode
      = find_child_to_register(this, m_obj.o_canvas->gl_list, "ossia.view");
  for (auto v : viewnode)
  {
    if (v->m_otype == Type::view)
    {
      t_view* view = (t_view*)v;
      view->unregister();
    }
    else if (v->m_otype == Type::remote)
    {
      t_remote* remote = (t_remote*)v;
      remote->unregister();
    }
  }
}

void t_client::on_parameter_created_callback(const ossia::net::parameter_base& param)
{
  auto& node = param.get_node();
  std::string addr = ossia::net::address_string_from_node(node);
  t_atom a[2];
  SETSYMBOL(a, gensym("create"));
  SETSYMBOL(a+1, gensym(addr.c_str()));
  outlet_anything(m_dumpout, gensym("parameter"), 2, a);
}

void t_client::on_parameter_deleted_callback(const ossia::net::parameter_base& param)
{
  auto& node = param.get_node();
  std::string addr = ossia::net::address_string_from_node(node);
  t_atom a[2];
  SETSYMBOL(a, gensym("delete"));
  SETSYMBOL(a+1, gensym(addr.c_str()));
  outlet_anything(m_dumpout, gensym("parameter"), 2, a);
}

static void client_update(t_client* x)
{
  if (x->m_device)
  {
    x->m_device->get_protocol().update(*x->m_device);
    x->m_node = &x->m_device->get_root_node();

    t_client::register_children(x);
  }
}

static void client_connect(t_client* x, t_symbol*, int argc, t_atom* argv)
{

  // unregister and free x_device
  x->unregister_children();
  if (x->m_device)
    delete (x->m_device);
  x->m_device = nullptr;

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
        x->m_device = new ossia::net::generic_device{
            std::make_unique<ossia::net::minuit_protocol>(
                device_name, remoteip, remoteport, localport),
            x->m_name->s_name};
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
        x->m_device = new ossia::net::generic_device{
            std::unique_ptr<ossia::net::protocol_base>(protocol), "Pd"};

        std::cout << "connected to device " << x->m_device->get_name()
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

  if (x->m_device)
  {
    x->m_device->on_parameter_created.connect<t_client, &t_client::on_parameter_created_callback>(x);
    x->m_device->on_parameter_removing.connect<t_client, &t_client::on_parameter_deleted_callback>(x);
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
    eclass_addmethod(c, (method)obj_namespace, "namespace", A_NULL, 0);
    eclass_addmethod(c, (method)client_connect, "connect", A_GIMME, 0);
    eclass_addmethod(
        c, (method)Protocol_Settings::print_protocol_help, "help", A_NULL, 0);
    eclass_addmethod(c, (method) obj_preset, "preset", A_GIMME, 0);

  }

  auto& ossia_pd = ossia_pd::instance();
  ossia_pd.client = c;

}
} // pd namespace
} // ossia namespace
