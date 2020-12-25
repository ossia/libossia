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
#include <ossia/network/base/parameter_data.hpp>

#if defined(OSSIA_PROTOCOL_PHIDGETS)
#include <ossia/network/phidgets/phidgets_protocol.hpp>
#endif
#if defined(OSSIA_PROTOCOL_LEAPMOTION)
#include <ossia/network/leapmotion/leapmotion_device.hpp>
#endif

using namespace ossia::max;

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

#pragma mark -
#pragma mark ossia_device class methods

extern "C" void ossia_device_setup()
{
  // instantiate the ossia.client class
  t_class* c = class_new(
      "ossia.device", (method)device::create, (method)device::destroy,
      (short)sizeof(device), 0L, A_GIMME, 0);

  device::class_setup(c);
  class_register(CLASS_BOX, c);

  auto& ossia_library = ossia_max::instance();
  ossia_library.ossia_device_class = c;
}

namespace ossia
{
namespace max
{

void device::class_setup(t_class *c)
{
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
  class_addmethod(
      c, (method) device::send_raw_osc,
      "osc", A_GIMME, 0);
  class_addmethod(
      c, (method) device::resend_all_values,
      "send_all_values", A_GIMME, 0);
}

void* device::create(t_symbol*, long argc, t_atom* argv)
{
  auto x = make_ossia<device>();

  if (x)
  {
    critical_enter(0);
    auto& pat_desc = ossia_max::get_patcher_descriptor(x->m_patcher);
    if(!pat_desc.device && !pat_desc.client)
    {
      pat_desc.device= x;
    }
    else
    {
      error("You can put only one [ossia.device] or [ossia.client] per patcher");
      object_free(x);
      critical_exit(0);
      return nullptr;
    }

    // make outlets
    x->m_dumpout = outlet_new(x, NULL); // anything outlet to dump device state

    // parse arguments
    long attrstart = attr_args_offset(argc, argv);

    // check name argument
    x->m_name = gensym("Max");
    x->m_otype = object_class::device;

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

    x->m_device = std::make_shared<ossia::net::generic_device>(std::move(local_proto_ptr),
                                                               x->m_name->s_name);
    x->connect_slots();

    defer_low(x, (method) object_base::loadbang, nullptr, 0, nullptr);

    ossia_max::instance().devices.push_back(x);

    on_device_created(x);

    critical_exit(0);
  }

  return (x);
}

void device::destroy(device* x)
{
  critical_enter(0);
  x->m_dead = true;
  x->m_node_selection.clear();
  x->m_matchers.clear();

  // This is no more needed since all children
  // are connected to node.about_to_be_deleted
  //x->unregister_children();

  {
#if defined(OSSIA_PROTOCOL_PHIDGETS)
    auto& multiplex = static_cast<ossia::net::multiplex_protocol&>(
        x->m_device->get_protocol());
    auto& protos = multiplex.get_protocols();

    for(auto& proto : protos)
    {
      if(dynamic_cast<ossia::phidget_protocol*>(proto.get()))
      {
        if(phidgets_poll_clock)
        {
          clock_unset(phidgets_poll_clock);
          clock_free((t_object*)phidgets_poll_clock);
          phidgets_poll_clock = nullptr;
        }
        phidgets_exposed = false;
      }
    }
#endif
  }

  x->disconnect_slots();
  on_device_removing(x);

  x->m_device = nullptr;

  if(x->m_dumpout)
    outlet_delete(x->m_dumpout);
  ossia_max::instance().devices.remove_all(x);

  x->~device();
  critical_exit(0);
}

// TODO do we still need this function ?
void device::register_children(device* x)
{
  std::vector<std::shared_ptr<matcher>> matchers{std::make_shared<matcher>(&x->m_device->get_root_node(), x)};
  register_children_in_patcher_recursively(x->m_patcher, x);
  output_all_values(x->m_patcher, true);
}

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
        auto minuit_proto = std::make_unique<ossia::net::minuit_protocol>(
            x->m_name->s_name, settings.remoteip, settings.remoteport,
            settings.localport);

        t_atom a[4];
        A_SETSYM(a, gensym("minuit"));
        A_SETSYM(a, gensym(minuit_proto->get_ip().c_str()));
        A_SETLONG(a+2, minuit_proto->get_remote_port());
        A_SETLONG(a+3, minuit_proto->get_local_port());

        multiplex.expose_to(std::move(minuit_proto));

        outlet_anything(x->m_dumpout, gensym("expose"), 3, a);
        object_post(
            (t_object*)x,
            "Connected with Minuit protocol to %s on port %u and listening on "
            "port %u",
            settings.remoteip.c_str(), settings.remoteport, settings.localport);
      }
      catch (const std::exception& e)
      {
        object_error((t_object*)x, "can't connect, port might be already in use");
        object_error((t_object*)x, "libossia error: '%s'", e.what());
        return;
      }
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
        auto oscq_proto = std::make_unique<ossia::oscquery::oscquery_server_protocol>(
              settings.oscport, settings.wsport);
        oscq_proto->set_echo(true);

        t_atom a[3];
        A_SETSYM(a, gensym("oscquery"));
        A_SETLONG(a+1, oscq_proto->get_osc_port());
        A_SETLONG(a+2, oscq_proto->get_ws_port());

        multiplex.expose_to(std::move(oscq_proto));

        outlet_anything(x->m_dumpout, gensym("expose"), 3, a);
        object_post(
            (t_object*)x,
            "Connected with oscquery protocol with OSC port %u and WS port %u, "
            "listening on port %u",
            settings.oscport, settings.wsport, settings.oscport);
      }
      catch (const std::exception& e)
      {
        object_error((t_object*)x, "can't connect, port might be already in use");
        object_error((t_object*)x, "libossia error: '%s'", e.what());
        return;
      }
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
        auto proto = std::make_unique<ossia::net::osc_protocol>(
            settings.remoteip, settings.remoteport, settings.localport);

        t_atom a[4];
        A_SETSYM(a, gensym("osc"));
        A_SETSYM(a+1,gensym(proto->get_ip().c_str()));
        A_SETLONG(a+2, proto->get_remote_port());
        A_SETLONG(a+3, proto->get_local_port());

        multiplex.expose_to(std::move(proto));

        outlet_anything(x->m_dumpout, gensym("expose"),4, a);
        object_post(
            (t_object*)x,
            "Connected with OSC protocol to %s on port %u and listening on port "
            "%u",
            settings.remoteip.c_str(), settings.remoteport, settings.localport);
      }
      catch (const std::exception& e)
      {
        object_error((t_object*)x, "can't connect, port might be already in use");
        object_error((t_object*)x, "libossia error: '%s'", e.what());
        return;
      }
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
            return;
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
  auto& protocols = static_cast<ossia::net::multiplex_protocol&>(
                        x->m_device->get_protocol()).get_protocols();

  int index = 0;

  t_atom a;
  A_SETLONG(&a,protocols.size());
  outlet_anything(x->m_dumpout,gensym("protocols"),1,&a);

  for(auto& p : protocols)
  {
    std::vector<t_atom> vec;
    p.get();
    if(auto osc = dynamic_cast<const ossia::net::osc_protocol*>(p.get()))
    {
      vec.resize(5);
      auto data=vec.begin();
      A_SETLONG(data++, index++);
      A_SETSYM(data++, gensym("osc"));
      A_SETSYM(data++, gensym(osc->get_ip().c_str()));
      A_SETLONG(data++, osc->get_remote_port());
      A_SETLONG(data++, osc->get_local_port());
    }
    else if(auto oscq = dynamic_cast<const ossia::oscquery::oscquery_server_protocol*>(p.get()))
    {
      vec.resize(4);
      auto data=vec.begin();
      A_SETLONG(data++, index++);
      A_SETSYM(data++, gensym("oscquery"));
      A_SETLONG(data++, oscq->get_osc_port());
      A_SETLONG(data++, oscq->get_ws_port());
    }
    else if(auto minuit = dynamic_cast<const ossia::net::minuit_protocol*>(p.get()))
    {
      vec.resize(5);
      auto data=vec.begin();
      A_SETLONG(data++, index++);
      A_SETSYM(data++, gensym("minuit"));
      A_SETSYM(data++, gensym(minuit->get_ip().c_str()));
      A_SETLONG(data++, minuit->get_remote_port());
      A_SETLONG(data++, minuit->get_local_port());
    }

    outlet_anything(x->m_dumpout, gensym("protocol"), vec.size(), vec.data());
  }
}

void device::get_mess_cb(device* x, t_symbol* s)
{
  if ( s == gensym("protocols") )
    device::get_protocols(x);
  else
    device_base::get_mess_cb(x,s);
}

void device::stop_expose(device* x, int index)
{
  auto& multiplex = static_cast<ossia::net::multiplex_protocol&>(
      x->m_device->get_protocol());
  auto& protos = multiplex.get_protocols();

  if ( index < protos.size() )
  {
#if defined(OSSIA_PROTOCOL_PHIDGETS)
      if(dynamic_cast<ossia::phidget_protocol*>(protos[index].get()))
      {
          clock_unset(phidgets_poll_clock);
          clock_free((t_object*)phidgets_poll_clock);
          phidgets_poll_clock = nullptr;
          phidgets_exposed = false;
      }
#endif
    multiplex.stop_expose_to(*protos[index]);
    t_atom a;
    A_SETLONG(&a, index);
    outlet_anything(x->m_dumpout, gensym("stop_expose"), 1, &a);
  }
  else
    object_error((t_object*)x, "Index %d out of bound.", index);
}

void device::assist(device* x, void*, long m, long a, char* s)
{
  if (m == ASSIST_INLET)
  {
    sprintf(s, "Device messages input");
  }
  else
  {
    sprintf(s, "Dumpout");
  }
}

void device::send_raw_osc(device *x, t_symbol *s, int argc, t_atom *argv)
{
  if(argc < 1)
  {
    object_error(&x->m_object, "osc message need at least an address as symbol argument.");
    return;
  }

  auto& protocols = static_cast<ossia::net::multiplex_protocol&>(
                       x->m_device->get_protocol()).get_protocols();

  ossia::net::full_parameter_data fpd;
  fpd.address = argv->a_w.w_sym->s_name;
  argc--; argv++;
  fpd.set_value(atom2value(nullptr, argc, argv));

  for(auto& p : protocols)
  {
    p->push_raw(fpd);
  }
}

void device::resend_all_values(device *x, t_symbol *s)
{
  auto children = ossia::net::list_all_children(&x->m_device->get_root_node());

  auto& protocols = static_cast<ossia::net::multiplex_protocol&>(
                        x->m_device->get_protocol()).get_protocols();

  for(auto& p : protocols)
  {
    p.get();
    if(dynamic_cast<const ossia::net::osc_protocol*>(p.get()))
    {
      for(const auto& c : children)
      {
        auto param = c->get_parameter();
        if(param)
        {
          p->push(*param, param->value());
        }
      }
    }
  }
}

} // max namespace
} // ossia namespace
