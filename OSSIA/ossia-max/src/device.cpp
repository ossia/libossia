// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "device.hpp"
#include "model.hpp"
#include "parameter.hpp"
#include "remote.hpp"
#include "view.hpp"
#include "utils.hpp"
#include "ossia-max.hpp"
#include <ossia/detail/config.hpp>
#include "ossia/network/osc/osc.hpp"
#include "ossia/network/oscquery/oscquery_server.hpp"
#include "ossia/network/minuit/minuit.hpp"
#include <ossia/network/local/local.hpp>
#include <boost/algorithm/string/case_conv.hpp>

#if defined(OSSIA_PROTOCOL_PHIDGETS)
#include <ossia/network/phidgets/phidgets_protocol.hpp>
#endif
#if defined(OSSIA_PROTOCOL_LEAPMOTION)
#include <ossia/network/leapmotion/leapmotion_device.hpp>
#endif

using namespace ossia::max;

#pragma mark -
#pragma mark ossia_device class methods

extern "C" void ossia_device_setup()
{
  // instantiate the ossia.client class
  t_class* c = class_new(
      "ossia.device", (method)device::create, (method)device::destroy,
      (short)sizeof(device), 0L, A_GIMME, 0);

  device_base::class_setup(c);

  class_addmethod(
      c, (method)device::register_children,
      "register", A_NOTHING, 0);
  class_addmethod(
      c, (method)device::expose,
      "expose", A_GIMME, 0);
  class_addmethod(
      c, (method)protocol_settings::print_protocol_help,
      "help", A_NOTHING, 0);
  class_addmethod(
      c, (method)device::name,
      "name", A_GIMME, 0);
  class_addmethod(
        c, (method) device::stop_expose,
        "stop", A_LONG, 0);
  class_addmethod(
        c, (method) device::get_mess_cb,
        "get", A_SYM, 0);
  class_addmethod(
      c, (method) device::notify,
      "notify", A_CANT, 0);

  class_register(CLASS_BOX, c);

  auto& ossia_library = ossia_max::instance();
  ossia_library.ossia_device_class = c;
}

namespace ossia
{
namespace max
{

void* device::create(t_symbol* name, long argc, t_atom* argv)
{
  auto& ossia_library = ossia_max::instance();
  auto x = make_ossia<device>();

  if (x)
  {
    // make outlets
    x->m_dumpout
        = outlet_new(x, NULL); // anything outlet to dump device state

    // parse arguments
    long attrstart = attr_args_offset(argc, argv);

    // check name argument
    x->m_name = gensym("Max");
    x->m_otype = object_class::device;

    if (ossia::max::find_peer(x))
    {
      error("You can have only one [ossia.device] or [ossia.client] per patcher.");
      device::destroy(x);
      return nullptr;
    }

    if (argc && argv)
    {
      if (atom_gettype(argv) == A_SYM)
      {
        x->m_name = atom_getsym(argv);
      }
    }

    // process attr args, if any
    attr_args_process(x, argc - attrstart, argv + attrstart);

    auto local_proto_ptr = std::make_unique<ossia::net::local_protocol>();

    x->m_device = new ossia::net::generic_device{std::move(local_proto_ptr),
                                                 x->m_name->s_name};
    x->connect_slots();

    x->get_hierarchy();
    auto& map = ossia_max::instance().root_patcher;
    //auto it = map.find(x->m_patcher_hierarchy.back());

    // register object only if root patcher have been loadbanged
    // else the patcher itself will trig a registration on loadbang
    // if(it != map.end() && it->second)
    //  device::register_children(x);

    ossia_library.devices.push_back(x);
  }

  return (x);
}

void device::destroy(device* x)
{
  x->m_dead = true;
  x->m_matchers.clear();

  // This is no more needed since all children
  // are connected to node.about_to_be_deleted
  //x->unregister_children();

  x->disconnect_slots();

  delete x->m_device;
  x->m_device = nullptr;

  outlet_delete(x->m_dumpout);
  ossia_max::instance().devices.remove_all(x);
  register_quarantinized();
  x->~device();
}

void device::register_children(device* x)
{
  std::vector<object_base*> children_model = find_children_to_register(
      &x->m_object, get_patcher(&x->m_object), gensym("ossia.model"));

  for (auto child : children_model)
  {
    if (child->m_otype == object_class::model)
    {
      ossia::max::model* model = (ossia::max::model*)child;
      model->register_node(x->m_matchers);
    }
    else if (child->m_otype == object_class::param)
    {
      ossia::max::parameter* parameter = (ossia::max::parameter*)child;
      parameter->register_node(x->m_matchers);
    }
  }

  std::vector<object_base*> children_view = find_children_to_register(
      &x->m_object, get_patcher(&x->m_object), gensym("ossia.view"));

  for (auto child : children_view)
  {
    if (child->m_otype == object_class::view)
    {
      ossia::max::view* view = (ossia::max::view*)child;
      view->register_node(x->m_matchers);
    }
    else if (child->m_otype == object_class::remote)
    {
      ossia::max::remote* remote = (ossia::max::remote*)child;
      remote->register_node(x->m_matchers);
    }
  }
}

void device::unregister_children()
{
  std::vector<object_base*> children_model = find_children_to_register(
      &m_object, get_patcher(&m_object), gensym("ossia.model"));


  for (auto child : children_model)
  {
    if (child->m_otype == object_class::model)
    {
      ossia::max::model* model = (ossia::max::model*)child;
      model->unregister();
    }
    else if (child->m_otype == object_class::param)
    {
      ossia::max::parameter* parameter = (ossia::max::parameter*)child;
      parameter->unregister();
    }
  }

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
#if defined(OSSIA_PROTOCOL_PHIDGETS)
static void* phidgets_poll_clock = {};
static bool phidgets_exposed = false;
static void update_phidgets(void* x)
{
    auto dev = reinterpret_cast<device*>(x);
    ossia::net::device_base* d = dev->m_device;
    if(auto prot = dynamic_cast<ossia::net::multiplex_protocol*>(&d->get_protocol()))
    {
        for(auto& e : prot->get_protocols())
        {
            if(auto ph = dynamic_cast<ossia::phidget_protocol*>(e.get()))
            {
                ph->run_commands();
            }
        }
    }

    clock_delay(phidgets_poll_clock, 2000);
}
#endif

void device::expose(device* x, t_symbol*, long argc, t_atom* argv)
{
  if (argc && argv->a_type == A_SYM)
  {
    auto& multiplex = static_cast<ossia::net::multiplex_protocol&>(
          x->m_device->get_protocol());
    const auto protocol = boost::to_lower_copy(std::string(atom_getsym(argv)->s_name));

    if (protocol == "minuit")
    {
      protocol_settings::minuit settings{};

      argc--;
      argv++;

      if (argc == 3 && argv[0].a_type == A_SYM && argv[1].a_type == A_LONG
          && argv[2].a_type == A_LONG)
      {
        settings.remoteip = atom_getsym(argv++)->s_name;
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
        A_SETSYM(&a[0], gensym("Minuit"));
        A_SETSYM(&a[1], gensym(settings.remoteip.c_str()));
        A_SETFLOAT(&a[2], settings.remoteport);
        A_SETFLOAT(&a[3], settings.localport);
        x->m_protocols.push_back(a);
      }
      catch (const std::exception& e)
      {
        object_error((t_object*)x, "can't connect, port might be already in use");
        object_error((t_object*)x, "libossia error: '%s'", e.what());
        return;
      }

      object_post(
          (t_object*)x,
          "Connected with Minuit protocol to %s on port %u and listening on "
          "port %u",
          settings.remoteip.c_str(), settings.remoteport, settings.localport);
    }
    else if (protocol == "oscquery")
    {
      protocol_settings::oscquery settings{};

      argc--;
      argv++;

      if (argc == 2 && argv[0].a_type == A_LONG && argv[1].a_type == A_LONG)
      {
        settings.oscport = atom_getlong(argv++);
        settings.wsport = atom_getlong(argv++);
      }

      try
      {
        multiplex.expose_to(
            std::make_unique<ossia::oscquery::oscquery_server_protocol>(
                settings.oscport, settings.wsport));

        std::vector<t_atom> a;
        a.resize(3);
        A_SETSYM(&a[0], gensym("oscquery"));
        A_SETFLOAT(&a[1], settings.oscport);
        A_SETFLOAT(&a[2], settings.wsport);
        x->m_protocols.push_back(a);
      }
      catch (const std::exception& e)
      {
        object_error((t_object*)x, "can't connect, port might be already in use");
        object_error((t_object*)x, "libossia error: '%s'", e.what());
        return;
      }

      object_post(
          (t_object*)x,
          "Connected with oscquery protocol with OSC port %u and WS port %u, "
          "listening on port %u",
          settings.oscport, settings.wsport, settings.oscport);
    }
    else if (protocol == "osc")
    {
      protocol_settings::osc settings{};

      argc--;
      argv++;

      if (argc == 3 && argv[0].a_type == A_SYM && argv[1].a_type == A_LONG
          && argv[2].a_type == A_LONG)
      {
        settings.remoteip = atom_getsym(argv++)->s_name;
        settings.remoteport = atom_getlong(argv++);
        settings.localport = atom_getlong(argv++);
      }

      try
      {
        multiplex.expose_to(std::make_unique<ossia::net::osc_protocol>(
            settings.remoteip, settings.remoteport, settings.localport));

        std::vector<t_atom> a;
        a.resize(4);
        A_SETSYM(&a[0], gensym("osc"));
        A_SETSYM(&a[1], gensym(settings.remoteip.c_str()));
        A_SETFLOAT(&a[2], settings.remoteport);
        A_SETFLOAT(&a[3], settings.localport);
        x->m_protocols.push_back(a);
      }
      catch (const std::exception& e)
      {
        object_error((t_object*)x, "can't connect, port might be already in use");
        object_error((t_object*)x, "libossia error: '%s'", e.what());
        return;
      }

      object_post(
          (t_object*)x,
          "Connected with OSC protocol to %s on port %u and listening on port "
          "%u",
          settings.remoteip.c_str(), settings.remoteport, settings.localport);
    }

#if defined(OSSIA_PROTOCOL_PHIDGETS)
    else if(protocol == "phidgets")
    {
        if(!phidgets_exposed)
        {
            multiplex.expose_to(std::make_unique<ossia::phidget_protocol>());
            std::vector<t_atom> a;
            a.resize(1);
            A_SETSYM(&a[0], gensym("phidgets"));
            x->m_protocols.push_back(a);

            phidgets_poll_clock = clock_new(x, (method)update_phidgets);

            clock_delay(phidgets_poll_clock, 1000);
            phidgets_exposed = true;
        }
        else
        {
            error("Only a single Phidget device can be active at a given time");
        }
    }
#endif

#if defined(OSSIA_PROTOCOL_LEAPMOTION)
    else if(protocol == "leapmotion")
    {
        multiplex.expose_to(std::make_unique<ossia::leapmotion_protocol>());
        std::vector<t_atom> a;
        a.resize(1);
        A_SETSYM(&a[0], gensym("LeapMotion"));
        x->m_protocols.push_back(a);
    }
#endif

    else
    {
      object_error((t_object*)x, "Unknown protocol: %s", protocol.c_str());
    }
  }
  else
    protocol_settings::print_protocol_help();
}

void device::name(device *x, t_symbol* s, long argc, t_atom* argv){
  if( argc == 0 )
  {
    t_atom a;
    atom_setsym(&a,gensym(x->m_device->get_name().c_str()));
    outlet_anything(x->m_dumpout,gensym("name"),1,&a);
  } else if ( argv[0].a_type == A_SYM ) {
    x->m_name = argv[0].a_w.w_sym;
    x->m_device->set_name(x->m_name->s_name);
  } else {
    object_error(&x->m_object,"bad argument to message 'name'");
  }
}

void device::get_protocols(device* x)
{
  t_atom a;
  A_SETLONG(&a,x->m_protocols.size());
  outlet_anything(x->m_dumpout,gensym("protocols"),1,&a);

  int j=0;
  for (auto& v : x->m_protocols)
  {
    t_atom ar[5];
    A_SETLONG(ar,j);
    for (int i = 0 ; i<v.size() ; i++)
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

void device::stop_expose(device*x, int index)
{
  auto& multiplex = static_cast<ossia::net::multiplex_protocol&>(
      x->m_device->get_protocol());
  auto& protos = multiplex.get_protocols();

  if ( index < x->m_protocols.size() && index < protos.size() )
  {
#if defined(OSSIA_PROTOCOL_PHIDGETS)
      if(dynamic_cast<ossia::phidget_protocol*>(protos[index].get()))
      {
          clock_unset(phidgets_poll_clock);
          clock_free((t_object*)phidgets_poll_clock);
          phidgets_exposed = false;
      }
#endif
    multiplex.stop_expose_to(*protos[index]);
    x->m_protocols.erase(x->m_protocols.begin() + index);
  }
  else
    object_error((t_object*)x, "Index %d out of bound.", index);
}

} // max namespace
} // ossia namespace
