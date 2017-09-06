// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "client.hpp"
#include "device.hpp"
#include "model.hpp"
#include "parameter.hpp"
#include "remote.hpp"
#include "view.hpp"
#include "utils.hpp"

#include <ossia/network/osc/osc.hpp>
#include <ossia/network/oscquery/oscquery_mirror.hpp>
#include <ossia/network/minuit/minuit.hpp>
#include <ossia/network/zeroconf/zeroconf.hpp>

#include <functional>
#include <iostream>
#include <memory>
#include <sstream>

using namespace ossia::max;

#pragma mark -
#pragma mark ossia_client class methods

extern "C" void ossia_client_setup()
{
  auto& ossia_library = ossia_max::instance();

  // instantiate the ossia.client class
  t_class* c = class_new(
      "ossia.client", (method)client::create, (method)client::destroy,
      (short)sizeof(client), 0L, A_GIMME, 0);

  class_addmethod(
      c, (method)client::register_children,
      "register", A_NOTHING, 0);

  class_addmethod(
      c, (method)client::update,
      "update", A_NOTHING, 0);

  class_addmethod(
      c, (method)client::loadbang,
      "loadbang", A_NOTHING, 0);

  class_addmethod(
      c, (method)client::connect,
      "connect", A_GIMME, 0);

  class_addmethod(
      c, (method)client::disconnect,
      "disconnect", A_GIMME, 0);

  class_addmethod(
      c, (method)protocol_settings::print_protocol_help,
      "help", A_NOTHING, 0);

  class_addmethod(
      c, (method)client::assist,
      "assist", A_NOTHING, 0);

  class_register(CLASS_BOX, c);
  ossia_library.ossia_client_class = c;

}


namespace ossia
{
namespace max
{

void* client::create(t_symbol* name, long argc, t_atom* argv)
{
  auto& ossia_library = ossia_max::instance();
  auto place = object_alloc(ossia_library.ossia_client_class);

  t_object tmp;
  memcpy(&tmp, place, sizeof(t_object));
  client* x = new(place) client();
  memcpy(x, &tmp, sizeof(t_object));

  if (x)
  {
    // make outlets
    x->m_dumpout
        = outlet_new(x, NULL); // anything outlet to dump client state

    x->m_device = 0;

    x->m_otype = object_class::client;

    if (ossia::max::find_peer(x))
    {
      error("You can have only one [ossia.device] or [ossia.client] per patcher.");
      client::destroy(x);
      return nullptr;
    }

    // parse arguments
    long attrstart = attr_args_offset(argc, argv);

    // check name argument
    x->m_name = gensym("Max");
    if (attrstart && argv)
    {
      if (atom_gettype(argv) == A_SYM)
      {
        x->m_name = atom_getsym(argv);
      }
    }

    // process attr args, if any
    attr_args_process(x, argc - attrstart, argv + attrstart);

    ossia_library.clients.push_back(x);
  }

  return (x);
}

void client::destroy(client* x)
{
  x->m_dead = true;
  x->unregister_children();

  if (x->m_device)
    delete (x->m_device);
  x->m_device = nullptr;
  outlet_delete(x->m_dumpout);
  ossia_max::instance().clients.remove_all(x);
  register_quarantinized();
  x->~client();
}

#pragma mark -
#pragma mark t_client structure functions

void client::assist(client*, void*, long m, long a, char* s)
{
  if (m == ASSIST_INLET)
  {
    sprintf(s, "All purpose input");
  }
  else
  {
    switch(a)
    {
      case 0:
        sprintf(s, "Dumpout");
        break;
      default:
        ;
    }
  }
}

void client::connect(client* x, t_symbol*, int argc, t_atom* argv)
{

  disconnect(x);

  ossia::net::minuit_connection_data minuit_settings;
  minuit_settings.name = "Pd";
  minuit_settings.host = "127.0.0.1";
  minuit_settings.remote_port = 6666;
  minuit_settings.local_port = 9999;

  ossia::net::oscquery_connection_data oscq_settings;
  oscq_settings.name = "Max";
  oscq_settings.host = "127.0.0.1";
  oscq_settings.port = 5678;

  t_atom connection_status[6];

  if (argc && argv->a_type == A_SYM)
  {
    std::string protocol_name = argv->a_w.w_sym->s_name;

    if ( argc == 1
         && protocol_name != "oscquery"
         && protocol_name != "Minuit" )
    {
      std::string name;

      if ( x->m_looking_for )
        name = x->m_looking_for->s_name;
      else
        name = argv->a_w.w_sym->s_name;

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
          object_error((t_object*)x, "it seems that I'am already looking for something, please wait a bit...");
          return;
        }
        else
        {
          if ( x->m_looking_for )
          {
            object_error((t_object*)x, "sorry I can't find device %s, try again later...", x->m_looking_for->s_name);
            t_atom connection_status[2];
            A_SETFLOAT(connection_status, 0);
            (connection_status+1, x->m_looking_for);

            outlet_anything(x->m_dumpout, gensym("connect"), 2, connection_status);
            x->m_looking_for = nullptr;

            return;
          }
          x->m_looking_for = gensym(name.c_str());
          client::getdevices(x);
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
          && argv[0].a_type == A_SYM
          && argv[1].a_type == A_SYM
          && argv[2].a_type == A_FLOAT
          && argv[3].a_type == A_FLOAT)
      {
        minuit_settings.name = atom_getsym(argv++)->s_name;
        minuit_settings.host = atom_getsym(argv++)->s_name;
        minuit_settings.remote_port = atom_getfloat(argv++);
        minuit_settings.local_port = atom_getfloat(argv++);
      }

      (connection_status+1, gensym("minuit"));
      (connection_status+2, gensym(minuit_settings.name.c_str()));
      (connection_status+3, gensym(minuit_settings.host.c_str()));
      A_SETFLOAT(connection_status+4, minuit_settings.remote_port);
      A_SETFLOAT(connection_status+5, minuit_settings.local_port);

      try
      {
        x->m_device = new ossia::net::generic_device{
            std::make_unique<ossia::net::minuit_protocol>(
              minuit_settings.name, minuit_settings.host,
              minuit_settings.remote_port, minuit_settings.local_port),
            x->m_name->s_name};
        A_SETFLOAT(connection_status,1);
      }
      catch (const std::exception& e)
      {
        object_error((t_object*)x, "%s", e.what());
        A_SETFLOAT(connection_status,0);
      }

      outlet_anything(x->m_dumpout, gensym("connect"),6, connection_status);
    }
    else if (protocol_name == "oscquery")
    {
      argc--;
      argv++;
      std::string wsurl = "ws://127.0.0.1:5678";
      if (argc == 1
          && argv[0].a_type == A_SYM)
      {
        wsurl = atom_getsym(argv)->s_name;
      }

      A_SETSYM(connection_status+1, gensym("oscquery"));
      A_SETSYM(connection_status+2, gensym(oscq_settings.name.c_str()));
      A_SETSYM(connection_status+3, gensym(wsurl.c_str()));

      try
      {
        auto protocol = new ossia::oscquery::oscquery_mirror_protocol{wsurl};
        x->m_device = new ossia::net::generic_device{
            std::unique_ptr<ossia::net::protocol_base>(protocol), oscq_settings.name};

        std::cout << "connected to device " << x->m_device->get_name()
                  << " on " << wsurl << std::endl;
        A_SETFLOAT(connection_status,1);
      }
      catch (const std::exception& e)
      {
        object_error((t_object*)x, "%s", e.what());
        A_SETFLOAT(connection_status,0);
      }

      outlet_anything(x->m_dumpout, gensym("connect"),4, connection_status);
    }
    else
    {
      object_error((t_object*)(t_object*)x, "Unknown protocol: %s", protocol_name.c_str());
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

    clock_free((t_object*)x->m_clock);
    x->m_clock = nullptr;

    t_atom a;
    float num = x->m_minuit_devices.size() + x->m_oscq_devices.size();
    A_SETFLOAT(&a, num);
    outlet_anything(x->m_dumpout, gensym("devices"), 1, &a);

    t_atom av[5];
    A_SETSYM(av, gensym("minuit"));
    for (auto dev : x->m_minuit_devices)
    {
      (av+1, gensym(dev.name.c_str()));
      (av+2, gensym(dev.host.c_str()));
      A_SETFLOAT(av+3, dev.local_port);
      A_SETFLOAT(av+4, dev.remote_port);

      outlet_anything(x->m_dumpout, gensym("device"), 5, av);
    }

    (av, gensym("oscquery"));
    for (auto dev : x->m_oscq_devices)
    {
      A_SETSYM(av+1, gensym(dev.name.c_str()));
      std::stringstream ss;
      ss << "ws://" << dev.host << ":" << dev.port;
      A_SETSYM(av+2, gensym(ss.str().c_str()));

      outlet_anything(x->m_dumpout, gensym("device"), 3, av);
    }

    if ( x->m_looking_for )
    {
      t_atom a;
      (&a, x->m_looking_for);
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

void client::getdevices(client* x)
{
  if (x->m_async_thread)
  {
    object_error((t_object*)x, "already scanning network for device, please wait a bit.");
  } else {
    x->m_async_thread = new std::thread(client::find_devices_async,x);
    x->m_clock = clock_new(x, (method)client::check_thread_status);
    clock_delay(x->m_clock,1000);
  }

}

void client::register_children(client* x)
{
  std::vector<object_base*> children_view = find_children_to_register(
      &x->m_object, get_patcher(&x->m_object), gensym("ossia.view"));

  for (auto child : children_view)
  {
    if (child->m_otype == object_class::view)
    {
      ossia::max::view* view = (ossia::max::view*)child;
      view->register_node(x->m_nodes);
    }
    else if (child->m_otype == object_class::remote)
    {
      ossia::max::remote* remote = (ossia::max::remote*)child;
      remote->register_node(x->m_nodes);
    }
  }
}

void client::unregister_children()
{

  std::vector<object_base*> children_view = find_children_to_register(
      &m_object, get_patcher(&m_object), gensym("ossia.view"));

  for (auto child : children_view)
  {
    if (child->m_otype == object_class::view)
    {
      ossia::max::view* view = (ossia::max::view*)child;
      view->unregister();
    }
    else if (child->m_otype == object_class::remote)
    {
      ossia::max::remote* remote = (ossia::max::remote*)child;
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
    x->unregister_children();
    delete x->m_device;
    x->m_device = nullptr;
  }
}

void client::loadbang(client* x)
{
  register_children(x);
}

} // max namespace
} // ossia namespace
