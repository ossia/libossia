// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <ossia-pd/src/client.hpp>
#include <ossia-pd/src/ossia-pd.hpp>
#include <ossia-pd/src/utils.hpp>

#include <ossia/network/osc/osc.hpp>
#include <ossia/network/oscquery/oscquery_mirror.hpp>
#include <ossia/network/minuit/minuit.hpp>
#include <ossia/network/zeroconf/zeroconf.hpp>

#include <functional>
#include <iostream>
#include <memory>
#include <sstream>
namespace ossia
{
namespace pd
{

client::client():
  device_base{ossia_pd::client_class}
{ }

void client::destroy(client* x)
{
  x->m_dead = true;
  x->m_matchers.clear();
  x->unregister_children();
  if (x->m_device)
    delete (x->m_device);
  x->m_device = nullptr;
  ossia_pd::instance().clients.remove_all(x);
  outlet_free(x->m_dumpout);
  register_quarantinized();

  x->~client();
}

void client::poll_message(client* x)
{
  if (x->m_oscq_protocol)
  {
    x->m_oscq_protocol->run_commands();
    clock_delay(x->m_poll_clock, x->m_rate);
  }
}

void* client::create(t_symbol* name, int argc, t_atom* argv)
{
  auto& ossia_pd = ossia_pd::instance();
  ossia::pd::client* x = new ossia::pd::client();

  // TODO SANITIZE : Direct leak
  t_binbuf* d = binbuf_via_atoms(argc, argv);

  if (x && d)
  {
    ossia_pd.clients.push_back(x);
    x->m_otype = object_class::client;

    x->m_name = gensym("Pd");
    x->m_dumpout = outlet_new((t_object*)x, gensym("dumpout"));
    x->m_poll_clock = clock_new((t_object*)x, (t_method) client::poll_message);

    if (argc != 0 && argv[0].a_type == A_SYMBOL)
    {
      x->m_name = atom_getsymbol(argv);
    }

    x->m_rate = 100;

    ebox_attrprocess_viabinbuf(x, d);

    if (find_peer(x))
    {
      error(
            "Only one [ø.device]/[ø.client] intance per patcher is allowed.");
      client::destroy(x);
      free(x);
      x = nullptr;
    }
  }

  return (x);
}

void client::loadbang(client* x, t_float type)
{
  if (LB_LOAD == (int)type)
  {
    register_children(x);
  }
}

void client::register_children(client* x)
{

  std::vector<object_base*> viewnodes
      = find_child_to_register(x, x->m_obj.o_canvas->gl_list, "ossia.view");
  for (auto v : viewnodes)
  {
    if (v->m_otype == object_class::view)
    {
      ossia::pd::view* view = (ossia::pd::view*)v;
      view->register_node(x->m_nodes);
    }
    else if (v->m_otype == object_class::remote)
    {
      ossia::pd::remote* remote = (ossia::pd::remote*)v;
      remote->register_node(x->m_nodes);
    }
  }
}

void client::unregister_children()
{
  std::vector<object_base*> viewnode
      = find_child_to_register(this, m_obj.o_canvas->gl_list, "ossia.view");
  for (auto v : viewnode)
  {
    if (v->m_otype == object_class::view)
    {
      ossia::pd::view* view = (ossia::pd::view*)v;
      view->unregister();
    }
    else if (v->m_otype == object_class::remote)
    {
      ossia::pd::remote* remote = (ossia::pd::remote*)v;
      remote->unregister();
    }
  }
}



void client::update(client* x)
{
  // TODO use ossia::net::oscquery::oscquery_mirror_protocol::run_commands()
  // for OSC Query

  if (x->m_device)
  {
    x->m_device->get_protocol().update(*x->m_device);
    x->m_nodes = {&x->m_device->get_root_node()};

    client::register_children(x);
  }
}

void client::disconnect(client* x)
{
  if (x->m_device)
  {
    x->m_oscq_protocol = nullptr;
    x->disconnect_slots();
    x->unregister_children();
    delete x->m_device;
    x->m_device = nullptr;
  }
}

void client::connect(client* x, t_symbol*, int argc, t_atom* argv)
{

  client::disconnect(x);

  ossia::net::minuit_connection_data minuit_settings;
  minuit_settings.name = "Pd";
  minuit_settings.host = "127.0.0.1";
  minuit_settings.remote_port = 6666;
  minuit_settings.local_port = 9999;

  ossia::net::oscquery_connection_data oscq_settings;
  oscq_settings.name = "Pd";
  oscq_settings.host = "127.0.0.1";
  oscq_settings.port = 5678;

  t_atom connection_status[6];

  if (argc && argv->a_type == A_SYMBOL)
  {
    std::string protocol_name = argv->a_w.w_symbol->s_name;

    if ( argc == 1
         && protocol_name != "oscquery"
         && protocol_name != "Minuit" )
    {
      std::string name;

      if ( x->m_looking_for )
        name = x->m_looking_for->s_name;
      else
        name = argv->a_w.w_symbol->s_name;

      protocol_name = "";
      for ( auto dev : x->m_oscq_devices )
      {
        if ( dev.name == name )
        {
          protocol_name = "oscquery";
          oscq_settings = dev;
          break;
        }
      }

      if ( protocol_name == "" )
      {
        for ( auto dev : x->m_minuit_devices )
        {
          if ( dev.name == name )
          {
            protocol_name = "Minuit";
            minuit_settings = dev;
            break;
          }
        }
      }

      if ( protocol_name == "" ) // can't find any device in the list that matches the name
      {
        if ( !x->m_done )
        {
          pd_error(x, "it seems that I'am already looking for something, please wait a bit...");
          return;
        }
        else
        {
          if ( x->m_looking_for )
          {
            pd_error(x, "sorry I can't find device %s, try again later...", x->m_looking_for->s_name);
            t_atom connection_status[2];
            SETFLOAT(connection_status, 0);
            SETSYMBOL(connection_status+1, x->m_looking_for);

            outlet_anything(x->m_dumpout, gensym("connect"), 2, connection_status);
            x->m_looking_for = nullptr;

            return;
          }
          x->m_looking_for = gensym(name.c_str());
          client::get_devices(x);
          return;
        }
      } else {
        x->m_looking_for = nullptr;
      }
    }

    if (protocol_name == "Minuit")
    {
      argc--;
      argv++;
      if (argc == 4
          && argv[0].a_type == A_SYMBOL
          && argv[1].a_type == A_SYMBOL
          && argv[2].a_type == A_FLOAT
          && argv[3].a_type == A_FLOAT)
      {
        minuit_settings.name = atom_getsymbol(argv++)->s_name;
        minuit_settings.host = atom_getsymbol(argv++)->s_name;
        minuit_settings.remote_port = atom_getfloat(argv++);
        minuit_settings.local_port = atom_getfloat(argv++);
      }

      SETSYMBOL(connection_status+1, gensym("minuit"));
      SETSYMBOL(connection_status+2, gensym(minuit_settings.name.c_str()));
      SETSYMBOL(connection_status+3, gensym(minuit_settings.host.c_str()));
      SETFLOAT(connection_status+4, minuit_settings.remote_port);
      SETFLOAT(connection_status+5, minuit_settings.local_port);

      try
      {
        x->m_device = new ossia::net::generic_device{
            std::make_unique<ossia::net::minuit_protocol>(
              minuit_settings.name, minuit_settings.host,
              minuit_settings.remote_port, minuit_settings.local_port),
            x->m_name->s_name};
        SETFLOAT(connection_status,1);
      }
      catch (const std::exception& e)
      {
        pd_error(x, "%s", e.what());
        SETFLOAT(connection_status,0);
      }

      outlet_anything(x->m_dumpout, gensym("connect"),6, connection_status);
    }
    else if (protocol_name == "oscquery")
    {
      argc--;
      argv++;
      std::string wsurl = "ws://127.0.0.1:5678";
      if (argc == 1
          && argv[0].a_type == A_SYMBOL)
      {
        wsurl = atom_getsymbol(argv)->s_name;
      }

      SETSYMBOL(connection_status+1, gensym("oscquery"));
      SETSYMBOL(connection_status+2, gensym(oscq_settings.name.c_str()));
      SETSYMBOL(connection_status+3, gensym(wsurl.c_str()));

      try
      {
        x->m_oscq_protocol = new ossia::oscquery::oscquery_mirror_protocol{wsurl};
        x->m_device = new ossia::net::generic_device{
            std::unique_ptr<ossia::net::protocol_base>(x->m_oscq_protocol), oscq_settings.name};

        clock_set(x->m_poll_clock, 1);
        SETFLOAT(connection_status,1);
      }
      catch (const std::exception& e)
      {
        pd_error(x, "%s", e.what());
        SETFLOAT(connection_status,0);
      }

      outlet_anything(x->m_dumpout, gensym("connect"),4, connection_status);
    }
    else
    {
      pd_error((t_object*)x, "Unknown protocol: %s", protocol_name.c_str());
    }
  }
  else
  {
    client::print_protocol_help();
  }

  x->connect_slots();
  client::update(x);
}

void client::check_thread_status(client* x)
{
  if ( x->m_done )
  {
    x->m_async_thread->join();
    delete x->m_async_thread;
    x->m_async_thread = nullptr;

    x->m_done = false;

    clock_free(x->m_clock);
    x->m_clock = nullptr;

    t_atom a;
    float num = x->m_minuit_devices.size() + x->m_oscq_devices.size();
    SETFLOAT(&a, num);
    outlet_anything(x->m_dumpout, gensym("devices"), 1, &a);

    t_atom av[5];
    SETSYMBOL(av, gensym("minuit"));
    for (auto dev : x->m_minuit_devices)
    {
      SETSYMBOL(av+1, gensym(dev.name.c_str()));
      SETSYMBOL(av+2, gensym(dev.host.c_str()));
      SETFLOAT(av+3, dev.local_port);
      SETFLOAT(av+4, dev.remote_port);

      outlet_anything(x->m_dumpout, gensym("device"), 5, av);
    }

    SETSYMBOL(av, gensym("oscquery"));
    for (auto dev : x->m_oscq_devices)
    {
      SETSYMBOL(av+1, gensym(dev.name.c_str()));
      std::stringstream ss;
      ss << "ws://" << dev.host << ":" << dev.port;
      SETSYMBOL(av+2, gensym(ss.str().c_str()));

      outlet_anything(x->m_dumpout, gensym("device"), 3, av);
    }

    if ( x->m_looking_for )
    {
      t_atom a;
      SETSYMBOL(&a, x->m_looking_for);
      client::connect(x, gensym("connect"), 1, &a);
    }

  } else {
    clock_delay(x->m_clock,10);
  }
}

void client::find_devices_async(client* x)
{
  x->m_done = false;
  x->m_minuit_devices.clear();
  x->m_oscq_devices.clear();

  x->m_minuit_devices =  ossia::net::list_minuit_devices();
  x->m_oscq_devices = ossia::net::list_oscquery_devices();

  x->m_done = true;
}

void client::get_devices(client* x)
{
  if (x->m_async_thread)
  {
    pd_error(x, "already scanning network for device, please wait a bit.");
  } else {
    x->m_async_thread = new std::thread(client::find_devices_async,x);
    x->m_clock = clock_new(x, (t_method)client::check_thread_status);
    clock_delay(x->m_clock,1000);
  }
}

void client::get_mess_cb(client* x, t_symbol* s)
{
  if ( s == gensym("unit") )
    client::get_devices(x);
  else
    device_base::get_mess_cb(x,s);
}

extern "C" void setup_ossia0x2eclient(void)
{
  t_eclass* c = eclass_new(
      "ossia.client", (method)client::create, (method)client::destroy,
      (short)sizeof(client), CLASS_DEFAULT, A_GIMME, 0);

  if (c)
  {
    class_addcreator((t_newmethod)client::create,gensym("ø.client"), A_GIMME, 0);

    device_base::class_setup(c);

    eclass_addmethod(
        c, (method)client::register_children, "register", A_NULL, 0);
    eclass_addmethod(c, (method)client::update, "update", A_NULL, 0);
    eclass_addmethod(c, (method)client::loadbang, "loadbang", A_NULL, 0);
    eclass_addmethod(c, (method)client::connect, "connect", A_GIMME, 0);
    eclass_addmethod(c, (method)client::disconnect, "disconnect", A_NULL, 0);

    eclass_addmethod(c, (method)client::get_mess_cb, "get", A_SYMBOL, 0);

  }

  ossia_pd::client_class = c;

}
} // pd namespace
} // ossia namespace
