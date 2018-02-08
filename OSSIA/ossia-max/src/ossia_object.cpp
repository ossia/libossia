#include "ossia_object.hpp"
#include "ossia-max.hpp"

using namespace ossia::max;

extern "C" void ossia_ossia_setup()
{
  t_class* c = class_new(
      "ossia", (method)ossia_object::create, (method)ossia_object::destroy,
      (long)sizeof(ossia::max::ossia_object), 0L, A_GIMME, 0);

  node_base::class_setup(c);
  class_addmethod(c, (method)device::expose, "expose", A_GIMME, 0);
  class_addmethod(c, (method)device::name, "name", A_GIMME, 0);

  class_register(CLASS_BOX, c);

  auto& ossia_library = ossia::max::ossia_max::instance();
  ossia_library.ossia_ossia_class = c;
}

namespace ossia
{
namespace max
{

void* ossia_object::create(t_symbol* name, long argc, t_atom* argv)
{
  auto& ossia_library = ossia::max::ossia_max::instance();

  auto x = make_ossia<ossia_object>();

  x->m_dumpout
      = outlet_new(x, NULL); // anything outlet to dump device state
  x->m_device = ossia_library.get_default_device();
  x->m_otype = object_class::device;
  x->m_name = gensym(x->m_device->get_name().c_str());
  x->m_matchers.emplace_back(&x->m_device->get_root_node(), (object_base*)nullptr);

  x->m_device->on_parameter_created.connect<device_base, &device_base::on_parameter_created_callback>(x);
  x->m_device->on_parameter_removing.connect<device_base, &device_base::on_parameter_deleted_callback>(x);

  if (argc > 0 && argv[0].a_type == A_SYM){
    x->m_name = argv[0].a_w.w_sym;
    x->m_device->set_name(x->m_name->s_name);
  }

  return (x);
}

void ossia_object::destroy(ossia_object *x)
{
  x->m_device->on_parameter_created.disconnect<device_base, &device_base::on_parameter_created_callback>(x);
  x->m_device->on_parameter_removing.disconnect<device_base, &device_base::on_parameter_deleted_callback>(x);

  ossia_max::instance().devices.remove_all(x);

  outlet_delete(x->m_dumpout);
  x->~ossia_object();
}

} // namespace max
} // namespace ossia
