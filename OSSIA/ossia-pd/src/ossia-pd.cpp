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

static t_class* ossia_class;

namespace ossia
{
namespace pd
{

typedef struct t_ossia
{
  t_object m_obj; // pd object - always placed first in the object's struct

} t_ossia;

static void* ossia_new(t_symbol* name, int argc, t_atom* argv)
{
  t_ossia* x = (t_ossia*)pd_new(ossia_class);
  return (x);
}

extern "C" OSSIA_PD_EXPORT void ossia_setup(void)
{
  t_class* c = class_new(
      gensym("ossia"), (t_newmethod)ossia_new, nullptr, sizeof(t_ossia),
      CLASS_DEFAULT, A_GIMME, 0);
  post("Welcome to ossia library");
  ossia_class = c;

  setup_ossia0x2eclient();
  setup_ossia0x2edevice();
  setup_ossia0x2emodel();
  setup_ossia0x2eparam();
  setup_ossia0x2eremote();
  setup_ossia0x2eview();
}

// ossia-pd constructor
ossia_pd::ossia_pd():
  m_localProtocol{new ossia::net::local_protocol},
  m_device{std::unique_ptr<ossia::net::protocol_base>(m_localProtocol), "ossia_device"}
{
  //m_localProtocol->exposeTo(std::make_unique<ossia::oscquery::oscquery_server_protocol>(1234, 5678));
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
      // TODO check if object is dying
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
