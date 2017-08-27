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

using t_ossia = t_device;

static void* ossia_new(t_symbol* name, int argc, t_atom* argv)
{
  auto& ossia_pd = ossia_pd::instance();
  t_ossia* x = (t_ossia*) eobj_new(ossia_pd.ossia);

  x->m_dumpout = outlet_new((t_object*)x, gensym("dumpout"));
  x->m_device = ossia_pd.get_default_device();

  x->m_device->on_parameter_created.connect<t_device, &t_device::on_parameter_created_callback>(x);
  x->m_device->on_parameter_removing.connect<t_device, &t_device::on_parameter_deleted_callback>(x);

  x->m_node = &x->m_device->get_root_node();

  if (argc > 0 && argv[0].a_type == A_SYMBOL){
      std::string name = argv[0].a_w.w_symbol->s_name;
      x->m_device->set_name(name);
    }

  return (x);
}

static void ossia_free(t_ossia *x)
{
  outlet_free(x->m_dumpout);

  x->m_device->on_parameter_created.disconnect<t_device, &t_device::on_parameter_created_callback>(x);
  x->m_device->on_parameter_removing.disconnect<t_device, &t_device::on_parameter_deleted_callback>(x);
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

  class_addcreator((t_newmethod)ossia_new,gensym("ø"), A_GIMME, 0);

  eclass_addmethod(c, (method)device_expose, "expose", A_GIMME, 0);
  eclass_addmethod(c, (method)device_name, "name", A_GIMME, 0);
  eclass_addmethod(c, (method)obj_namespace, "namespace", A_GIMME, 0);

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

ossia_pd::~ossia_pd()
{
  for (auto x : views.copy()){
    if (x->m_node) x->m_node->about_to_be_deleted.disconnect<t_view, &t_view::is_deleted>(x);
  }
  for (auto x : remotes.copy()){
    x->m_matchers.clear();
  }
  for (auto x : models.copy()){
    if (x->m_node) x->m_node->about_to_be_deleted.disconnect<t_model, &t_model::is_deleted>(x);
  }
  for (auto x : params.copy()){
    x->m_matchers.clear();
  }
}

// ossia-pd library instance
ossia_pd& ossia_pd::instance()
{
  static ossia_pd library_instance;
  return library_instance;
}

}
} // namespace
