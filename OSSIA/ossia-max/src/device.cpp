// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "device.hpp"
#include "model.hpp"
#include "parameter.hpp"
#include "remote.hpp"
#include "view.hpp"
#include "utils.hpp"

#include "ossia/network/osc/osc.hpp"
#include "ossia/network/oscquery/oscquery_server.hpp"

using namespace ossia::max;

#pragma mark -
#pragma mark ossia_device class methods

extern "C" void ossia_device_setup()
{
  // instantiate the ossia.client class
  t_class* c = class_new(
      "ossia.device", (method)ossia_device_new, (method)ossia_device_free,
      (short)sizeof(t_device), 0L, A_GIMME, 0);

  class_addmethod(
      c, (method)t_device::register_children,
      "register", A_NOTHING, 0);
  class_addmethod(
      c, (method)t_object_base::getnamespace, "namespace",
      A_NOTHING, 0);
  class_addmethod(
      c, (method)ossia_device_expose, "expose",
      A_GIMME, 0);
  class_addmethod(
      c,
      (method)protocol_settings::print_protocol_help, "help", A_NOTHING, 0);
  class_addmethod(c, (method)ossia_device_name, "name", A_GIMME, 0);


  class_register(CLASS_BOX, c);

  auto& ossia_library = ossia_max::instance();
  ossia_library.ossia_device_class = c;
}

extern "C" void* ossia_device_new(t_symbol* name, long argc, t_atom* argv)
{
  auto& ossia_library = ossia_max::instance();
  t_device* x = (t_device*)object_alloc(ossia_library.ossia_device_class);

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
      ossia_device_free(x);
      return nullptr;
    }

    if (attrstart && argv)
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
    x->m_node = &x->m_device->get_root_node();

    ossia_library.devices.push_back(x);
    t_device::register_children(x);
  }

  return (x);
}

extern "C" void ossia_device_free(t_device* x)
{
  x->m_dead = true;
  x->unregister_children();
  if (x->m_device)
    delete (x->m_device);
  outlet_delete(x->m_dumpout);
  ossia_max::instance().devices.remove_all(x);
  register_quarantinized();

}

extern "C" void
ossia_device_expose(t_device* x, t_symbol*, long argc, t_atom* argv)
{
  if (argc && argv->a_type == A_SYM)
  {
    auto& proto = static_cast<ossia::net::local_protocol&>(
        x->m_device->get_protocol());
    std::string protocol = atom_getsym(argv)->s_name;

    if (protocol == "Minuit")
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
        proto.expose_to(std::make_unique<ossia::net::minuit_protocol>(
            x->m_name->s_name, settings.remoteip, settings.remoteport,
            settings.localport));
      }
      catch (const std::exception& e)
      {
        object_error((t_object*)x, "%s", e.what());
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
        settings.oscport = atom_getfloat(argv++);
        settings.wsport = atom_getfloat(argv++);
      }

      try
      {
        proto.expose_to(
            std::make_unique<ossia::oscquery::oscquery_server_protocol>(
                settings.oscport, settings.wsport));
      }
      catch (const std::exception& e)
      {
        object_error((t_object*)x, "%s", e.what());
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
        settings.remoteip = atom_getsym(argv)->s_name;
        settings.remoteport = atom_getfloat(argv++);
        settings.localport = atom_getfloat(argv++);
      }

      try
      {
        proto.expose_to(std::make_unique<ossia::net::osc_protocol>(
            settings.remoteip, settings.remoteport, settings.localport));
      }
      catch (const std::exception& e)
      {
        object_error((t_object*)x, "%s", e.what());
        return;
      }

      object_post(
          (t_object*)x,
          "Connected with OSC protocol to %s on port %u and listening on port "
          "%u",
          settings.remoteip.c_str(), settings.remoteport, settings.localport);
    }
    else
    {
      object_error((t_object*)x, "Unknown protocol: %s", protocol.c_str());
    }
  }
  else
    protocol_settings::print_protocol_help();
}

extern "C" void ossia_device_name(t_device *x, t_symbol* s, long argc, t_atom* argv){
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

namespace ossia
{
namespace max
{

#pragma mark -
#pragma mark t_device structure functions

void t_device::register_children(t_device* x)
{
  std::vector<t_object_base*> children_model = find_children_to_register(
      &x->m_object, get_patcher(&x->m_object), gensym("ossia.model"));

  for (auto child : children_model)
  {
    if (child->m_otype == object_class::model)
    {
      t_model* model = (t_model*)child;
      model->register_node(x->m_node);
    }
    else if (child->m_otype == object_class::param)
    {
      t_parameter* parameter = (t_parameter*)child;
      parameter->register_node(x->m_node);
    }
  }

  std::vector<t_object_base*> children_view = find_children_to_register(
      &x->m_object, get_patcher(&x->m_object), gensym("ossia.view"));

  for (auto child : children_view)
  {
    if (child->m_otype == object_class::view)
    {
      t_view* view = (t_view*)child;
      view->register_node(x->m_node);
    }
    else if (child->m_otype == object_class::remote)
    {
      t_remote* remote = (t_remote*)child;
      remote->register_node(x->m_node);
    }
  }
}

void t_device::unregister_children()
{
  std::vector<t_object_base*> children_model = find_children_to_register(
      &m_object, get_patcher(&m_object), gensym("ossia.model"));

  for (auto child : children_model)
  {
    if (child->m_otype == object_class::model)
    {
      t_model* model = (t_model*)child;
      model->unregister();
    }
    else if (child->m_otype == object_class::param)
    {
      t_parameter* parameter = (t_parameter*)child;
      parameter->unregister();
    }
  }

  std::vector<t_object_base*> children_view = find_children_to_register(
      &m_object, get_patcher(&m_object), gensym("ossia.view"));

  for (auto child : children_view)
  {
    if (child->m_otype == object_class::view)
    {
      t_view* view = (t_view*)child;
      view->unregister();
    }
    else if (child->m_otype == object_class::remote)
    {
      t_remote* remote = (t_remote*)child;
      remote->unregister();
    }
  }
}

} // max namespace
} // ossia namespace
