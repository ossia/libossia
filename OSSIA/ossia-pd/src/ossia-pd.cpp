// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
/*
// Copyright (c) 2016 Antoine Villeret
// For information on usage and redistribution, and for a DISCLAIMER OF ALL
// WARRANTIES, see the file, "LICENSE.txt," in this distribution.
*/

#include "ossia-pd.hpp"
#include "client.hpp"
#include "device.hpp"
#include "model.hpp"
#include "ossia/editor/dataspace/dataspace_visitors.hpp"
#include "parameter.hpp"
#include "remote.hpp"
#include "utils.hpp"
#include "view.hpp"
#include <ossia_pd_export.h>

#include "ossia/network/oscquery/oscquery_server.hpp"
#include "ossia/network/osc/osc.hpp"

namespace ossia
{
namespace pd
{

using t_ossia = t_obj_base;

static void* ossia_new(t_symbol* name, int argc, t_atom* argv)
{
  auto& ossia_pd = ossia_pd::instance();
  t_ossia* x = (t_ossia*) eobj_new(ossia_pd.ossia);

  x->x_dumpout = outlet_new((t_object*)x, gensym("dumpout"));

  return (x);
}

static void ossia_free(t_ossia *x)
{
  outlet_free(x->x_dumpout);
}

static void ossia_expose(t_ossia* x, t_symbol*, int argc, t_atom* argv)
{

  if (argc && argv->a_type == A_SYMBOL)
  {
    auto& proto = static_cast<ossia::net::local_protocol&>(
        ossia_pd::instance().get_default_device()->get_protocol());
    std::string name = ossia_pd::instance().get_default_device()->get_name();
    std::string protocol = argv->a_w.w_symbol->s_name;
    if (protocol == "Minuit")
    {
      Protocol_Settings::minuit settings{};
      argc--;
      argv++;
      if (argc == 3
          && argv[0].a_type == A_SYMBOL
          && argv[1].a_type == A_FLOAT
          && argv[2].a_type == A_FLOAT)
      {
        settings.remoteip = atom_getsymbol(argv++)->s_name;
        settings.remoteport = atom_getfloat(argv++);
        settings.localport = atom_getfloat(argv++);
      }

      try
      {
        proto.expose_to(std::make_unique<ossia::net::minuit_protocol>(
            name, settings.remoteip, settings.remoteport,
            settings.localport));
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
      Protocol_Settings::oscquery settings{};
      argc--;
      argv++;
      if (argc == 2
          && argv[0].a_type == A_FLOAT
          && argv[1].a_type == A_FLOAT)
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
        pd_error(x, "%s", e.what());
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
      if (argc == 3
          && argv[0].a_type == A_SYMBOL
          && argv[1].a_type == A_FLOAT
          && argv[2].a_type == A_FLOAT)
      {
        settings.remoteip = atom_getsymbol(argv)->s_name;
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
        pd_error(x, "%s", e.what());
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

static void ossia_get_namespace(t_ossia *x){
  auto dev = ossia_pd::instance().get_default_device();
  get_namespace(x,dev->get_root_node());
}

static void ossia_name(t_ossia *x, t_symbol* s, int argc, t_atom* argv){
  auto dev = ossia_pd::instance().get_default_device();
  if( argc == 0 )
  {
    t_atom a;
    SETSYMBOL(&a,gensym(dev->get_name().c_str()));
    outlet_anything(x->x_dumpout,gensym("name"),1,&a);
  } else if ( argv[0].a_type == A_SYMBOL ) {
    t_symbol* name = argv[0].a_w.w_symbol;
    dev->set_name(name->s_name);
  } else {
    pd_error(x,"bad argument to message 'name'");
  }
}

extern "C" OSSIA_PD_EXPORT void ossia_setup(void)
{
  t_eclass* c = eclass_new(
      "ossia", (method)ossia_new, (method)ossia_free, sizeof(t_ossia),
      CLASS_DEFAULT, A_GIMME, 0);

  setup_ossia0x2eclient();
  setup_ossia0x2edevice();
  setup_ossia0x2emodel();
  setup_ossia0x2eparam();
  setup_ossia0x2eremote();
  setup_ossia0x2eview();

  eclass_addmethod(c, (method)ossia_expose, "expose", A_GIMME, 0);
  eclass_addmethod(c, (method)ossia_name, "name", A_GIMME, 0);
  eclass_addmethod(c, (method)ossia_get_namespace, "namespace", A_GIMME, 0);

  auto& ossia_pd = ossia_pd::instance();
  ossia_pd.ossia = c;

  post("Welcome to ossia library");
  post("build on %s at %s", __DATE__, __TIME__);
}

// ossia-pd constructor
ossia_pd::ossia_pd():
  m_localProtocol{new ossia::net::local_protocol},
  m_device{std::unique_ptr<ossia::net::protocol_base>(m_localProtocol), "ossia_pd_device"}
{
}

// ossia-pd library instance
ossia_pd& ossia_pd::instance()
{
  static ossia_pd library_instance;
  return library_instance;
}

void register_quarantinized()
{
  for (auto model : t_model::quarantine().copy())
  {
    obj_register<t_model>(model);
  }
  for (auto param : t_param::quarantine().copy())
  {
    obj_register<t_param>(param);
  }
  for (auto view : t_view::quarantine().copy())
  {
    obj_register<t_view>(view);
  }
  for (auto remote : t_remote::quarantine().copy())
  {
    obj_register<t_remote>(remote);
  }
}

std::vector<obj_hierachy> find_child_to_register(
    t_obj_base* x, t_gobj* start_list, const std::string& classname)
{
  std::string subclassname
      = classname == "ossia.model" ? "ossia.param" : "ossia.remote";

  t_gobj* list = start_list;
  std::vector<obj_hierachy> found;

  // 1: iterate object list and look for ossia.model / ossia.view object
  while (list && list->g_pd)
  {
    std::string current = list->g_pd->c_name->s_name;
    if (current == classname)
    {
      obj_hierachy oh;
      oh.hierarchy = 0;
      oh.x = (t_obj_base*)&list->g_pd;
      oh.classname = classname;
      if (x != oh.x && !oh.x->x_dead)
      {
        t_obj_base* o = oh.x;
        found.push_back(oh);
      }
    }
    list = list->g_next;
  }

  // 2: if there is no ossia.model / ossia.view in the current patch, look into
  // the subpatches

  if (found.empty())
  {
    list = start_list;
    while (list && list->g_pd)
    {
      std::string current = list->g_pd->c_name->s_name;
      if (current == "canvas")
      {
        t_canvas* canvas = (t_canvas*)&list->g_pd;
        if (!canvas_istable(canvas))
        {
          t_gobj* _list = canvas->gl_list;
          std::vector<obj_hierachy> found_tmp
              = find_child_to_register(x, _list, classname);
          for (auto obj : found_tmp)
          {
            obj.hierarchy++; // increase hierarchy of objects found in a
                             // subpatcher
            found.push_back(obj);
          }
        }
      }
      list = list->g_next;
    }

    // 3: finally look for ossia.param / ossia.remote in the same pather
    list = start_list;
    while (list && list->g_pd)
    {
      std::string current = list->g_pd->c_name->s_name;
      if (current == subclassname)
      {
        obj_hierachy oh;
        oh.hierarchy = 0;
        oh.x = (t_obj_base*)&list->g_pd;
        oh.classname = subclassname;
        if (x != oh.x)
        {
          found.push_back(oh);
        }
      }
      list = list->g_next;
    }
  }

  return found;
}
}
} // namespace
