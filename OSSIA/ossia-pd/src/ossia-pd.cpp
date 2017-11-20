// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/network/dataspace/dataspace_visitors.hpp>
#include <ossia/network/oscquery/oscquery_server.hpp>
#include <ossia/network/osc/osc.hpp>
#include <ossia_pd_export.h>

#include <ossia-pd/src/device.hpp>
#include <ossia-pd/src/ossia-pd.hpp>

#include <git_info.h>

namespace ossia
{
namespace pd
{

using t_ossia = device;

static void* ossia_new(t_symbol* name, int argc, t_atom* argv)
{
  auto& opd = ossia_pd::instance();
  t_ossia* x = (t_ossia*) eobj_new(opd.ossia_class);

  opd.devices.push_back(x);

  x->m_dumpout = outlet_new((t_object*)x, gensym("dumpout"));
  x->m_device = opd.get_default_device();
  x->m_otype = object_class::device;
  x->m_name = gensym(x->m_device->get_name().c_str());

  x->m_device->on_parameter_created.connect<device, &device::on_parameter_created_callback>(x);
  x->m_device->on_parameter_removing.connect<device, &device::on_parameter_deleted_callback>(x);

  x->m_nodes = {&x->m_device->get_root_node()};

  if (argc > 0 && argv[0].a_type == A_SYMBOL){
      x->m_name = argv[0].a_w.w_symbol;
      x->m_device->set_name(x->m_name->s_name);
    }

  return (x);
}

static void ossia_free(t_ossia *x)
{
  outlet_free(x->m_dumpout);

  x->m_device->on_parameter_created.disconnect<device, &device::on_parameter_created_callback>(x);
  x->m_device->on_parameter_removing.disconnect<device, &device::on_parameter_deleted_callback>(x);

  ossia_pd::instance().devices.remove_all(x);
}

extern "C" OSSIA_PD_EXPORT void ossia_setup(void)
{
  t_eclass* c = eclass_new(
      "ossia", (method)ossia_new, (method)ossia_free, sizeof(t_ossia),
      CLASS_DEFAULT, A_GIMME, 0);

  setup_ossia0x2eattribute();
  setup_ossia0x2eclient();
  setup_ossia0x2edevice();
  setup_ossia0x2elogger();
  setup_ossia0x2emodel();
  setup_ossia0x2eparam();
  setup_ossia0x2eremote();
  setup_ossia0x2eview();

  class_addcreator((t_newmethod)ossia_new,gensym("ø"), A_GIMME, 0);

  node_base::class_setup(c);

  eclass_addmethod(c, (method) device::expose, "expose",    A_GIMME, 0);
  eclass_addmethod(c, (method) device::name,   "name",      A_GIMME, 0);

  ossia_pd::ossia_class = c;

  auto& inst = ossia_pd::instance();
  inst.params.reserve(2048);
  inst.remotes.reserve(1024);
  inst.models.reserve(512);
  inst.attributes.reserve(512);
  inst.views.reserve(512);
  inst.devices.reserve(4);
  inst.clients.reserve(4);
  post("Welcome to ossia library");
  post("build from SHA : %s %s", ossia::get_commit_sha().c_str(), ossia::get_commit_date().c_str());
}

// initializers
t_eclass* ossia_pd::attribute_class;
t_eclass* ossia_pd::client_class;
t_eclass* ossia_pd::device_class;
t_eclass* ossia_pd::logger_class;
t_eclass* ossia_pd::model_class;
t_eclass* ossia_pd::param_class;
t_eclass* ossia_pd::remote_class;
t_eclass* ossia_pd::view_class;
t_eclass* ossia_pd::ossia_class;

// ossia-pd constructor
ossia_pd::ossia_pd():
  m_localProtocol{new ossia::net::local_protocol},
  m_device{std::unique_ptr<ossia::net::protocol_base>(m_localProtocol), "ossia_pd_device"}
{
  sym_addr = gensym("address");
  sym_set  = gensym("set");
  m_device.on_attribute_modified.connect<&device_base::on_attribute_modified_callback>();
}

ossia_pd::~ossia_pd()
{
  m_device.on_attribute_modified.disconnect<&device_base::on_attribute_modified_callback>();
  for (auto x : devices.copy()){
    x->disconnect_slots();
  }
  for (auto x : views.copy()){
    x->m_matchers.clear();
  }
  for (auto x : remotes.copy()){
    x->m_matchers.clear();
  }
  for (auto x : models.copy()){
    x->m_matchers.clear();
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
