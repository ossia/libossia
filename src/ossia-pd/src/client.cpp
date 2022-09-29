// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <ossia/network/minuit/minuit.hpp>
#include <ossia/network/osc/osc.hpp>
#include <ossia/network/oscquery/oscquery_mirror.hpp>
#include <ossia/network/zeroconf/zeroconf.hpp>

#include <ossia-pd/src/client.hpp>
#include <ossia-pd/src/ossia-pd.hpp>
#include <ossia-pd/src/utils.hpp>

#include <boost/algorithm/string.hpp>

#include <functional>
#include <iostream>
#include <memory>
#include <sstream>

namespace ossia::pd
{

client::client()
    : device_base{ossia_pd::client_class}
{
}

void client::destroy(client* x)
{
  x->m_dead = true;
  x->m_matchers.clear();

  client::disconnect(x);

  ossia_pd::instance().clients.remove_all(x);

  outlet_free(x->m_dumpout);
  clock_free(x->m_poll_clock);
  clock_free(x->m_clock);

  register_quarantinized();

  x->~client();
}

void client::poll_message(client* x)
{
  if(x->m_oscq_protocol)
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

  if(x && d)
  {
    ossia_pd.clients.push_back(x);
    x->m_otype = object_class::client;

    x->m_name = gensym("Pd");
    x->m_dumpout = outlet_new((t_object*)x, gensym("dumpout"));
    x->m_poll_clock = clock_new((t_object*)x, (t_method)client::poll_message);
    x->m_clock = clock_new((t_object*)x, (t_method)client::connect);

    x->m_rate = 100;

    ebox_attrprocess_viabinbuf(x, d);

    if(find_peer(x))
    {
      error("Only one [ø.device]/[ø.client] instance per patcher is allowed.");
      client::destroy(x);
      free(x);
      x = nullptr;
    }

    if(argc != 0 && argv[0].a_type == A_SYMBOL)
    {
      x->m_name = atom_getsymbol(argv);
      bool autoconnect = true;
      if(argc > 1 && argv[1].a_type == A_FLOAT)
      {
        float f = atom_getfloat(argv + 1);
        autoconnect = f > 0.;
        if(autoconnect)
          connect_mess_cb(x, nullptr, 1, argv);
      }
      else
        connect_mess_cb(x, nullptr, argc, argv);
    }
  }

  return (x);
}

void client::register_children(client* x)
{
  std::string model_name(x->m_name->s_name);
  std::vector<object_base*> modelnodes
      = find_child_to_register(x, x->m_obj.o_canvas->gl_list, ossia_pd::o_sym_model);
  for(auto m : modelnodes)
  {
    if(m->m_otype == object_class::model)
    {
      ossia::pd::model* model = (ossia::pd::model*)m;
      std::string name(model->m_name->s_name);
      if(model->m_addr_scope == ossia::net::address_scope::global)
        model->register_node(x->m_matchers);
    }
    else if(m->m_otype == object_class::param)
    {
      ossia::pd::parameter* param = (ossia::pd::parameter*)m;
      std::string name(param->m_name->s_name);
      if(param->m_addr_scope == ossia::net::address_scope::global)
        param->register_node(x->m_matchers);
    }
  }

  std::vector<object_base*> viewnodes
      = find_child_to_register(x, x->m_obj.o_canvas->gl_list, ossia_pd::o_sym_view);
  for(auto v : viewnodes)
  {
    if(v->m_otype == object_class::view)
    {
      ossia::pd::view* view = (ossia::pd::view*)v;
      std::string name(view->m_name->s_name);
      if(view->m_addr_scope != ossia::net::address_scope::global)
        view->register_node(x->m_matchers);
    }
    else if(v->m_otype == object_class::remote)
    {
      ossia::pd::remote* remote = (ossia::pd::remote*)v;
      if(remote->m_addr_scope == ossia::net::address_scope::global)
        remote->register_node(x->m_matchers);
    }
  }

  // register objects with global addresses, they might be in upper patchers
  for(auto r : ossia_pd::instance().parameters.copy())
  {
    if(r->m_addr_scope == ossia::net::address_scope::global)
    {
      std::string name(r->m_name->s_name);
      if(boost::starts_with(name, model_name))
      {
        r->register_node(x->m_matchers);
      }
    }
  }

  for(auto r : ossia_pd::instance().models.copy())
  {
    if(r->m_addr_scope == ossia::net::address_scope::global)
    {
      std::string name(r->m_name->s_name);
      if(boost::starts_with(name, model_name))
      {
        r->register_node(x->m_matchers);
      }
    }
  }

  for(auto v : ossia_pd::instance().views.copy())
  {
    if(v->m_addr_scope == ossia::net::address_scope::global)
    {
      std::string name(v->m_name->s_name);
      if(boost::starts_with(name, model_name))
      {
        v->register_node(x->m_matchers);
      }
    }
  }

  for(auto r : ossia_pd::instance().remotes.copy())
  {
    if(r->m_addr_scope == ossia::net::address_scope::global)
    {
      std::string name(r->m_name->s_name);
      if(boost::starts_with(name, model_name))
      {
        r->register_node(x->m_matchers);
      }
    }
  }

  // after all, try to register quarantinized objects
  // needed to register objects with global address
  register_quarantinized();
}

void client::unregister_children()
{
  std::vector<object_base*> viewnode
      = find_child_to_register(this, m_obj.o_canvas->gl_list, ossia_pd::o_sym_view);
  for(auto v : viewnode)
  {
    if(v->m_otype == object_class::view)
    {
      ossia::pd::view* view = (ossia::pd::view*)v;
      view->unregister();
    }
    else if(v->m_otype == object_class::remote)
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

  if(x->m_device)
  {
    x->m_device->get_protocol().update(*x->m_device);

    auto& map = ossia_pd::instance().m_root_patcher;
    auto it = map.find(x->m_patcher_hierarchy.back());

    // register children only if root patcher have been loadbanged
    // else the patcher itself will trigger a registration on loadbang
    if(it != map.end() && it->second.is_loadbanged)
      client::register_children(x);
  }
}

void client::disconnect(client* x)
{
  if(x->m_device)
  {
    x->m_oscq_protocol = nullptr;
    x->disconnect_slots();
    x->unregister_children();
    if(!x->is_zeroconf())
      delete x->m_device;
    x->m_device = nullptr;
  }
  clock_unset(x->m_clock); // avoid automatic reconnection
}

void client::connect_mess_cb(client* x, t_symbol*, int argc, t_atom* argv)
{
  if(x->m_argv)
    delete x->m_argv;

  x->m_argc = argc;
  x->m_argv = new t_atom[argc];

  memcpy(x->m_argv, argv, argc * sizeof(t_atom));

  client::connect(x);
}

void client::connect(client* x)
{

  int argc = x->m_argc;
  t_atom* argv = x->m_argv;

  client::disconnect(x);

  ossia::net::minuit_connection_data minuit_settings;
  minuit_settings.name = x->m_name->s_name;
  minuit_settings.host = "127.0.0.1";
  minuit_settings.remote_port = 6666;
  minuit_settings.local_port = 9999;

  ossia::net::oscquery_connection_data oscq_settings;
  oscq_settings.name = x->m_name->s_name;
  oscq_settings.host = "127.0.0.1";
  oscq_settings.port = 5678;

  ossia::net::osc_connection_data osc_settings;
  osc_settings.name = x->m_name->s_name;
  osc_settings.host = "127.0.0.1";
  osc_settings.remote_port = 6666;
  osc_settings.local_port = 9999;

  t_atom connection_status[6];
  int count = 0;

  if(argc && argv->a_type == A_SYMBOL)
  {
    std::string protocol_name = argv->a_w.w_symbol->s_name;
    std::string name = protocol_name;
    boost::algorithm::to_lower(protocol_name);

    x->m_zeroconf = false;

    if(protocol_name == "minuit")
    {
      argc--;
      argv++;
      if(argc == 4 && argv[0].a_type == A_SYMBOL && argv[1].a_type == A_SYMBOL
         && argv[2].a_type == A_FLOAT && argv[3].a_type == A_FLOAT)
      {
        minuit_settings.name = atom_getsymbol(argv++)->s_name;
        minuit_settings.host = atom_getsymbol(argv++)->s_name;
        minuit_settings.remote_port = atom_getfloat(argv++);
        minuit_settings.local_port = atom_getfloat(argv++);
      }

      SETSYMBOL(connection_status + 1, gensym("minuit"));
      SETSYMBOL(connection_status + 2, gensym(minuit_settings.name.c_str()));
      SETSYMBOL(connection_status + 3, gensym(minuit_settings.host.c_str()));
      SETFLOAT(connection_status + 4, minuit_settings.remote_port);
      SETFLOAT(connection_status + 5, minuit_settings.local_port);

      try
      {
        x->m_device = new ossia::net::generic_device{
            std::make_unique<ossia::net::minuit_protocol>(
                minuit_settings.name, minuit_settings.host, minuit_settings.remote_port,
                minuit_settings.local_port),
            x->m_name->s_name};
      }
      catch(const std::exception& e)
      {
        pd_error(x, "can't connect, port might be already in use");
        pd_error(x, "libossia error: '%s'", e.what());
      }

      count = 6;
    }
    else if(protocol_name == "oscquery")
    {
      argc--;
      argv++;
      std::string wsurl
          = "ws://" + oscq_settings.host + ":" + std::to_string(oscq_settings.port);
      if(argc == 1 && argv[0].a_type == A_SYMBOL)
      {
        wsurl = atom_getsymbol(argv)->s_name;
      }

      SETSYMBOL(connection_status + 1, gensym("oscquery"));
      SETSYMBOL(connection_status + 2, gensym(oscq_settings.name.c_str()));
      SETSYMBOL(connection_status + 3, gensym(wsurl.c_str()));

      try
      {
        x->m_oscq_protocol = new ossia::oscquery::oscquery_mirror_protocol{wsurl};
        x->m_oscq_protocol->set_zombie_on_remove(false);
        x->m_device = new ossia::net::generic_device{
            std::unique_ptr<ossia::net::protocol_base>(x->m_oscq_protocol),
            oscq_settings.name};

        clock_set(x->m_poll_clock, 1);
      }
      catch(const std::exception& e)
      {
        pd_error(x, "can't connect, port might be already in use");
        pd_error(x, "libossia error: '%s'", e.what());
      }
      count = 4;
    }
    else if(protocol_name == "osc")
    {
      argc--;
      argv++;
      if(argc == 4 && argv[0].a_type == A_SYMBOL && argv[1].a_type == A_SYMBOL
         && (argv[2].a_type == A_FLOAT) && (argv[3].a_type == A_FLOAT))
      {
        osc_settings.name = atom_getsym(argv++)->s_name;
        osc_settings.host = atom_getsym(argv++)->s_name;
        osc_settings.remote_port = atom_getfloat(argv++);
        osc_settings.local_port = atom_getfloat(argv++);
      }

      SETSYMBOL(connection_status + 1, gensym("osc"));
      SETSYMBOL(connection_status + 2, gensym(osc_settings.name.c_str()));
      SETSYMBOL(connection_status + 3, gensym(osc_settings.host.c_str()));
      SETFLOAT(connection_status + 4, osc_settings.remote_port);
      SETFLOAT(connection_status + 5, osc_settings.local_port);

      try
      {
        x->m_device = new ossia::net::generic_device{
            std::make_unique<ossia::net::osc_protocol>(
                osc_settings.host, osc_settings.remote_port, osc_settings.local_port,
                osc_settings.name),
            x->m_name->s_name};
      }
      catch(const std::exception& e)
      {
        pd_error(x, "%s", e.what());
      }
      count = 6;
    }
    else
    {
      // Connect by device name : retrieve connection info
      x->m_zeroconf = true;
      x->m_device = ZeroconfOscqueryListener::find_device(name);
      if(!x->m_device)
      {
        if(auto data = ZeroconfMinuitListener::find_device(name))
        {
          x->m_device = new ossia::net::generic_device(
              std::make_unique<ossia::net::minuit_protocol>(
                  data->name, data->host, data->remote_port, (rand() + 1024) % 65536),
              data->name);
        }
      }
      SETSYMBOL(connection_status + 1, gensym(name.c_str()));
      count = 2;
    }
  }
  else
  {
    client::print_protocol_help();
    return;
  }

  if(x->m_device)
  {
    SETFLOAT(connection_status, 1);

    outlet_anything(x->m_dumpout, gensym("connect"), count, connection_status);

    x->connect_slots();
    client::update(x);
    clock_unset(x->m_clock);
  }
  else
  {
    SETFLOAT(connection_status, 0);

    outlet_anything(x->m_dumpout, gensym("connect"), count, connection_status);

    clock_delay(x->m_clock, 1000); // hardcoded reconnection delay
  }
}

void client::get_mess_cb(client* x, t_symbol* s)
{
  if(s == gensym("devices"))
  {
    // TODO post all devices
  }
  else
    device_base::get_mess_cb(x, s);
}

extern "C" void setup_ossia0x2eclient(void)
{
  t_eclass* c = eclass_new(
      "ossia.client", (method)client::create, (method)client::destroy,
      (short)sizeof(client), CLASS_DEFAULT, A_GIMME, 0);

  if(c)
  {
    class_addcreator((t_newmethod)client::create, gensym("ø.client"), A_GIMME, 0);

    device_base::class_setup(c);

    eclass_addmethod(c, (method)client::register_children, "register", A_NULL, 0);
    eclass_addmethod(c, (method)client::update, "update", A_NULL, 0);
    eclass_addmethod(c, (method)client::connect_mess_cb, "connect", A_GIMME, 0);
    eclass_addmethod(c, (method)client::disconnect, "disconnect", A_NULL, 0);

    eclass_addmethod(c, (method)client::get_mess_cb, "get", A_SYMBOL, 0);
    eclass_addmethod(c, (method)view::notify, "notify", A_NULL, 0);

#ifndef PURR_DATA
    eclass_register(CLASS_OBJ, c);
#endif
  }

  ossia_pd::client_class = c;
}
} // ossia namespace
