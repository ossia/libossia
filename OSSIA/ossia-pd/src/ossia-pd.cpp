// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia_pd_export.h>

#include <ossia-pd/src/device.hpp>
#include <ossia-pd/src/ossia-pd.hpp>
#include <ossia-pd/src/utils.hpp>

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

  x->m_dumpout = outlet_new((t_object*)x, gensym("dumpout"));
  x->m_device = opd.get_default_device();
  x->m_otype = object_class::device;
  x->m_name = gensym(x->m_device->get_name().c_str());
  x->m_matchers.push_back({&x->m_device->get_root_node(), (object_base*)nullptr});

  x->m_device->on_parameter_created.connect<device_base, &device_base::on_parameter_created_callback>(x);
  x->m_device->on_parameter_removing.connect<device_base, &device_base::on_parameter_deleted_callback>(x);

  x->m_patcher_hierarchy.push_back(x->m_obj.o_canvas);

  if (argc > 0 && argv[0].a_type == A_SYMBOL){
    x->m_name = argv[0].a_w.w_symbol;
    x->m_device->set_name(x->m_name->s_name);
  }

  return (x);
}

static void ossia_free(t_ossia *x)
{
  outlet_free(x->m_dumpout);

  x->m_device->on_parameter_created.disconnect<device_base, &device_base::on_parameter_created_callback>(x);
  x->m_device->on_parameter_removing.disconnect<device_base, &device_base::on_parameter_deleted_callback>(x);

  ossia_pd::instance().devices.remove_all(x);
}

extern "C" OSSIA_PD_EXPORT void ossia_setup(void)
{
  t_eclass* c = eclass_new(
      "ossia", (method)ossia_new, (method)ossia_free, sizeof(t_ossia),
      CLASS_DEFAULT, A_GIMME, 0);

  setup_ossia0x2eassert();
  setup_ossia0x2eattribute();
  setup_ossia0x2eclient();
  setup_ossia0x2edevice();
  setup_ossia0x2eexplorer();
  setup_ossia0x2elogger();
  setup_ossia0x2emodel();
  setup_ossia0x2eparam();
  setup_ossia0x2eremote();
  setup_ossia0x2eview();

  class_addcreator((t_newmethod)ossia_new,gensym("ø"), A_GIMME, 0);

  node_base::class_setup(c);

  eclass_addmethod(c, (method) device::expose, "expose",    A_GIMME, 0);
  eclass_addmethod(c, (method) device::name,   "name",      A_GIMME, 0);
  eclass_addmethod(c, (method) device::stop_expose, "stop", A_FLOAT, 0);
  eclass_addmethod(c, (method) device::get_mess_cb, "get", A_SYMBOL, 0);

  ossia_pd::ossia_class = c;

  auto& inst = ossia_pd::instance();
  inst.parameters.reserve(2048);
  inst.remotes.reserve(1024);
  inst.models.reserve(512);
  inst.attributes.reserve(512);
  inst.views.reserve(512);
  inst.devices.reserve(4);
  inst.clients.reserve(4);

  post("Welcome to ossia library");
  post("build SHA %s", ossia::get_commit_sha().c_str());
}

// initializers
t_eclass* ossia_pd::attribute_class;
t_eclass* ossia_pd::client_class;
t_eclass* ossia_pd::device_class;
t_eclass* ossia_pd::explorer_class;
t_eclass* ossia_pd::logger_class;
t_eclass* ossia_pd::model_class;
t_eclass* ossia_pd::param_class;
t_eclass* ossia_pd::remote_class;
t_eclass* ossia_pd::view_class;
t_eclass* ossia_pd::ossia_class;

t_symbol* ossia_pd::o_sym_attribute = gensym("ossia.attribute");
t_symbol* ossia_pd::o_sym_client = gensym("ossia.client");
t_symbol* ossia_pd::o_sym_device = gensym("ossia.device");
t_symbol* ossia_pd::o_sym_logger = gensym("ossia.logger");
t_symbol* ossia_pd::o_sym_model = gensym("ossia.model");
t_symbol* ossia_pd::o_sym_param = gensym("ossia.param");
t_symbol* ossia_pd::o_sym_remote = gensym("ossia.remote");
t_symbol* ossia_pd::o_sym_view = gensym("ossia.view");
t_symbol* ossia_pd::o_sym_ossia = gensym("ossia");
t_symbol* ossia_pd::o_sym_set = gensym("set");
t_symbol* ossia_pd::o_sym_address = gensym("address");

// ossia-pd constructor
ossia_pd::ossia_pd():
  m_localProtocol{new ossia::net::local_protocol},
  m_device{std::unique_ptr<ossia::net::protocol_base>(m_localProtocol), "ossia_pd_device"}
{
  m_device.on_attribute_modified.connect<&device_base::on_attribute_modified_callback>();

  m_reg_clock = clock_new(this, (t_method) ossia_pd::register_nodes);
}

ossia_pd::~ossia_pd()
{
  m_device.on_attribute_modified.disconnect<&device_base::on_attribute_modified_callback>();
  for (auto x : devices.copy()){
    auto& multiplex = static_cast<ossia::net::multiplex_protocol&>(
        x->m_device->get_protocol());
    auto& protos = multiplex.get_protocols();
    for (auto& proto : protos)
    {
      multiplex.stop_expose_to(*proto);
    }
    x->m_protocols.clear();
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
  for (auto x : parameters.copy()){
    x->m_matchers.clear();
  }
}

// ossia-pd library instance
ossia_pd& ossia_pd::instance()
{
  static ossia_pd library_instance;
  return library_instance;
}

void ossia_pd::register_nodes(void* x)
{
  auto& inst = ossia_pd::instance();
  auto& map = inst.root_patcher;
  for (auto it = map.begin(); it != map.end(); it++)
  {
    t_canvas* patcher = it->first;
    for (auto dev : inst.devices.reference())
    {
      if(dev->m_patcher_hierarchy.back() == patcher)
        ossia::pd::device::register_children(dev);
    }
    for (auto model : inst.models.reference())
    {
      if ( model->m_patcher_hierarchy.back() == patcher
            && model->m_matchers.empty())
        ossia_register(model);
    }
    for (auto param : inst.parameters.reference())
    {
      if ( param->m_patcher_hierarchy.back() == patcher
            && param->m_matchers.empty())
        ossia_register(param);
    }

    for (auto client : inst.clients.reference())
    {
      if(client->m_patcher_hierarchy.back() == patcher)
        ossia::pd::client::register_children(client);
    }
    for (auto view : inst.views.reference())
    {
      if ( view->m_patcher_hierarchy.back() == patcher
            && view->m_matchers.empty())
        ossia_register(view);
    }
    for (auto remote : inst.remotes.reference())
    {
      if ( remote->m_patcher_hierarchy.back() == patcher
            && remote->m_matchers.empty())
        ossia_register(remote);
    }
    for (auto attr : inst.attributes.reference())
    {
      if ( attr->m_patcher_hierarchy.back() == patcher
            && attr->m_matchers.empty())
        ossia_register(attr);
    }

    for (auto dev : inst.devices.reference())
    {
      if(dev->m_patcher_hierarchy.back() == patcher)
        node_base::push_default_value(dev);
    }

    std::vector<ossia::net::node_base*> list;

    // send default value for default device's child
    // TODO make this a method of ossia Max's object
    auto n = &inst.get_default_device()->get_root_node();
    list = ossia::net::list_all_child(n);

    for (ossia::net::node_base* child : list)
    {
      if (auto param = child->get_parameter())
      {
        auto val = ossia::net::get_default_value(*child);
        if(val)
        {
          param->push_value(*val);
          trig_output_value(child);
        }
      }
    }
    it->second.is_loadbanged = true;
  }
}

}
} // namespace
