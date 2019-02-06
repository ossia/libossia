// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia-pd/src/device.hpp>
#include <ossia-pd/src/utils.hpp>

#include <ossia/network/osc/osc.hpp>
#include <ossia/network/minuit/minuit.hpp>
#include <ossia/network/oscquery/oscquery_server.hpp>

#include <boost/algorithm/string.hpp>

namespace ossia
{
namespace pd
{

device::device():
  device_base{ossia_pd::device_class}
{ }

void device::destroy(device* x)
{
  x->m_dead = true;
  x->m_matchers.clear();

  // TODO why is this no more necessary since all children
  // should have register to node.about_to_be_deleted() signal
  // x->unregister_children();

  x->disconnect_slots();

  delete x->m_device;
  x->m_device = nullptr;

  ossia_pd::instance().devices.remove_all(x);
  outlet_free(x->m_dumpout);
  register_quarantinized();

  x->~device();
}

void* device::create(t_symbol* name, int argc, t_atom* argv)
{
  auto& ossia_pd = ossia_pd::instance();
  ossia::pd::device* x = new ossia::pd::device();
  // TODO SANITIZE : Direct leak
  t_binbuf* d = binbuf_via_atoms(argc, argv);

  if (x && d)
  {
    ossia_pd.devices.push_back(x);
    x->m_otype = object_class::device;

    x->m_name = gensym("Pd");
    x->m_dumpout = outlet_new((t_object*)x, gensym("dumpout"));

    if (argc != 0 && argv[0].a_type == A_SYMBOL)
    {
      x->m_name = atom_getsymbol(argv);
    }

    auto local_proto_ptr = std::make_unique<ossia::net::local_protocol>();
    x->m_device = new ossia::net::generic_device{std::move(local_proto_ptr),
                                                 x->m_name->s_name};

    ebox_attrprocess_viabinbuf(x, d);

    if (find_peer(x))
    {
      error(
            "Only one [ø.device]/[ø.client] instance per patcher is allowed.");
      device::destroy(x);
      free(x);
      x = nullptr;
      return x;
    }

    auto& map = ossia_pd::instance().m_root_patcher;
    auto it = map.find(x->m_patcher_hierarchy.back());

    // register object only if root patcher have been loadbanged
    // else the patcher itself will trig a registration on loadbang
    if(it != map.end() && it->second.is_loadbanged)
        register_children(x);

    x->connect_slots();
  }

  return (x);
}

void device::register_children(device* x)
{
  std::string device_name(x->m_name->s_name);
  std::vector<object_base*> modelnodes
      = find_child_to_register(x, x->m_obj.o_canvas->gl_list, ossia_pd::o_sym_model);
  for (auto v : modelnodes)
  {
    if (v->m_otype == object_class::model)
    {
      ossia::pd::model* model = (ossia::pd::model*)v;
      std::string name(model->m_name->s_name);
      if(model->m_addr_scope == ossia::net::address_scope::global
        && !boost::starts_with(name, device_name))
        continue;
      model->register_node(x->m_matchers);
    }
    else if (v->m_otype == object_class::param)
    {
      parameter* param = (parameter*)v;
      std::string name(param->m_name->s_name);
      if(param->m_addr_scope == ossia::net::address_scope::global
        && !boost::starts_with(name, device_name))
        continue;
      param->register_node(x->m_matchers);
    }
  }

  std::vector<object_base*> viewnodes
      = find_child_to_register(x, x->m_obj.o_canvas->gl_list, ossia_pd::o_sym_view);
  for (auto v : viewnodes)
  {
    if (v->m_otype == object_class::view)
    {
      ossia::pd::view* view = (ossia::pd::view*)v;
      std::string name(view->m_name->s_name);
      if(view->m_addr_scope == ossia::net::address_scope::global
        && !boost::starts_with(name, device_name))
        continue;
      view->register_node(x->m_matchers);
    }
    else if (v->m_otype == object_class::remote)
    {
      ossia::pd::remote* remote = (ossia::pd::remote*)v;
      std::string name(remote->m_name->s_name);
      if(remote->m_addr_scope == ossia::net::address_scope::global
        && !boost::starts_with(name, device_name))
        continue;
      remote->register_node(x->m_matchers);
    }
  }

  // then go through all remote objects with pattern matching name
  // to register them to device's address creation callback
  for (auto x : ossia_pd::instance().remotes.copy()){
    if (x->m_is_pattern)
      ossia_register<remote>(x);
  }
}

void device::unregister_children()
{
  std::vector<object_base*> node
      = find_child_to_register(this, m_obj.o_canvas->gl_list, ossia_pd::o_sym_model);

  for (auto v : node)
  {
    if (v->m_otype == object_class::model)
    {
      ossia::pd::model* model = (ossia::pd::model*)v;
      model->unregister();
    }
    else if (v->m_otype == object_class::param)
    {
      ossia::pd::parameter* param = (ossia::pd::parameter*)v;
      param->unregister();
    }
  }

  std::vector<object_base*> viewnode
      = find_child_to_register(this, m_obj.o_canvas->gl_list, ossia_pd::o_sym_view);
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

void device::expose(device* x, t_symbol*, int argc, t_atom* argv)
{

  if (argc && argv->a_type == A_SYMBOL)
  {
    auto& multiplex = static_cast<ossia::net::multiplex_protocol&>(
        x->m_device->get_protocol());
    std::string protocol = argv->a_w.w_symbol->s_name;
    if (protocol == "Minuit")
    {
      Protocol_Settings::minuit settings{};
      argc--;
      argv++;
      if (argc == 3 && argv[0].a_type == A_SYMBOL && argv[1].a_type == A_FLOAT
          && argv[2].a_type == A_FLOAT)
      {
        settings.remoteip = atom_getsymbol(argv++)->s_name;
        settings.remoteport = atom_getfloat(argv++);
        settings.localport = atom_getfloat(argv++);
      }

      try
      {
        multiplex.expose_to(std::make_unique<ossia::net::minuit_protocol>(
            x->m_name->s_name, settings.remoteip, settings.remoteport,
            settings.localport));
        std::vector<t_atom> a;
        a.resize(4);
        SETSYMBOL(&a[0], gensym("Minuit"));
        SETSYMBOL(&a[1], gensym(settings.remoteip.c_str()));
        SETFLOAT(&a[2], settings.remoteport);
        SETFLOAT(&a[3], settings.localport);
        x->m_protocols.push_back(a);
      }
      catch (const std::exception& e)
      {
        pd_error(x, "can't connect, port might be already in use");
        pd_error(x, "libossia error: '%s'", e.what());
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
      Protocol_Settings::oscquery settings{};
      argc--;
      argv++;
      if (argc == 2 && argv[0].a_type == A_FLOAT && argv[1].a_type == A_FLOAT)
      {
        settings.oscport = atom_getfloat(argv++);
        settings.wsport = atom_getfloat(argv++);
      }

      try
      {
        auto oscq_proto = std::make_unique<ossia::oscquery::oscquery_server_protocol>(
              settings.oscport, settings.wsport);
        oscq_proto->set_echo(true);

        multiplex.expose_to(std::move(oscq_proto));

        std::vector<t_atom> a;
        a.resize(3);
        SETSYMBOL(&a[0], gensym("oscquery"));
        SETFLOAT(&a[1], settings.oscport);
        SETFLOAT(&a[2], settings.wsport);
        x->m_protocols.push_back(a);
      }
      catch (const std::exception& e)
      {
        pd_error(x, "can't connect, port might be already in use");
        pd_error(x, "libossia error: '%s'", e.what());
        return;
      }
      logpost(
          x, 3,
          "New 'oscquery' protocol with OSC port %u and WS port %u, listening "
          "on port %u",
          settings.oscport, settings.wsport, settings.oscport);
    }
    else if (protocol == "osc")
    {
      Protocol_Settings::osc settings{};
      argc--;
      argv++;
      if (argc == 3 && argv[0].a_type == A_SYMBOL && argv[1].a_type == A_FLOAT
          && argv[2].a_type == A_FLOAT)
      {
        settings.remoteip = atom_getsymbol(argv)->s_name;
        settings.remoteport = atom_getfloat(argv++);
        settings.localport = atom_getfloat(argv++);
      }

      try
      {
        multiplex.expose_to(std::make_unique<ossia::net::osc_protocol>(
            settings.remoteip, settings.remoteport, settings.localport));
        std::vector<t_atom> a;
        a.resize(4);
        SETSYMBOL(&a[0], gensym("osc"));
        SETSYMBOL(&a[1], gensym(settings.remoteip.c_str()));
        SETFLOAT(&a[2], settings.remoteport);
        SETFLOAT(&a[3], settings.localport);
        x->m_protocols.push_back(a);
      }
      catch (const std::exception& e)
      {
        pd_error(x, "can't connect, port might be already in use");
        pd_error(x, "libossia error: '%s'", e.what());
        return;
      }
      logpost(
          x, 3,
          "New 'OSC' protocol connect to %s on port %u and listening on port "
          "%u",
          settings.remoteip.c_str(), settings.remoteport, settings.localport);
    }
    else
    {
      pd_error((t_object*)x, "Unknown protocol: %s", protocol.c_str());
    }
  }
  else
    Protocol_Settings::print_protocol_help();
}

void device::name(device *x, t_symbol* s, int argc, t_atom* argv){
  if( argc == 0 )
  {
    t_atom a;
    SETSYMBOL(&a,gensym(x->m_device->get_name().c_str()));
    outlet_anything(x->m_dumpout,gensym("name"),1,&a);
  } else if ( argv[0].a_type == A_SYMBOL ) {
    t_symbol* name = argv[0].a_w.w_symbol;
    x->m_device->set_name(name->s_name);
  } else {
    pd_error(x,"bad argument to message 'name'");
  }
}

void device::get_protocols(device* x)
{
  t_atom a;
  SETFLOAT(&a,x->m_protocols.size());
  outlet_anything(x->m_dumpout,gensym("protocols"),1,&a);

  int j=0;
  for (auto& v : x->m_protocols)
  {
    t_atom ar[5];
    SETFLOAT(ar,j);
    for (unsigned int i = 0 ; i<v.size() ; i++)
      ar[i+1] = v[i];

    outlet_anything(x->m_dumpout, gensym("protocol"), v.size()+1, ar);
    j++;
  }
}

void device::get_mess_cb(device* x, t_symbol* s)
{
  if ( s == gensym("protocols") )
    device::get_protocols(x);
  else
    device_base::get_mess_cb(x,s);
}

void device::stop_expose(device*x, float f)
{
  unsigned int index = static_cast<unsigned int>(f);
  auto& multiplex = static_cast<ossia::net::multiplex_protocol&>(
      x->m_device->get_protocol());
  auto& protos = multiplex.get_protocols();

  if ( index < x->m_protocols.size() && index < protos.size() )
  {
    multiplex.stop_expose_to(*protos[index]);
    x->m_protocols.erase(x->m_protocols.begin() + index);
  }
  else
    pd_error(x, "Index %d out of bound.", index);
}


extern "C" void setup_ossia0x2edevice(void)
{
  t_eclass* c = eclass_new(
      "ossia.device", (method)device::create, (method)device::destroy,
      (short)sizeof(device), CLASS_DEFAULT, A_GIMME, 0);

  if (c)
  {
    class_addcreator((t_newmethod)device::create,gensym("ø.device"), A_GIMME, 0);

    device_base::class_setup(c);

    // TODO delete register method (only for debugging purpose)
    eclass_addmethod(
          c, (method)device::register_children,"register", A_NULL, 0);
    eclass_addmethod(c, (method) device::expose, "expose", A_GIMME, 0);
    eclass_addmethod(
          c, (method)Protocol_Settings::print_protocol_help, "help", A_NULL, 0);
    eclass_addmethod(c, (method) device::name, "name", A_GIMME, 0);

    eclass_addmethod(c, (method) device::stop_expose, "stop", A_FLOAT, 0);
    eclass_addmethod(c, (method) device::get_mess_cb, "get", A_SYMBOL, 0);
    eclass_addmethod(c, (method) device::notify,    "notify",   A_NULL,  0);

    eclass_register(CLASS_OBJ, c);
  }

  ossia_pd::device_class = c;
}
} // pd namespace
} // ossia namespace
