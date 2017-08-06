#include "ossia-max.hpp"
#include "device.hpp"

using t_ossia = ossia::max::t_device;

extern "C" void* ossia_object_new(t_symbol* name, int argc, t_atom* argv)
{
  auto& ossia_library = ossia::max::ossia_max::instance();
  t_ossia* x = (t_ossia*) object_alloc(ossia_library.ossia_ossia_class);

  x->m_dump_out
      = outlet_new(x, NULL); // anything outlet to dump device state
  x->m_device = ossia_library.get_default_device();
  x->m_node = &ossia_library.get_default_device()->get_root_node();

  return (x);
}

extern "C" void ossia_object_free(t_ossia *x)
{
  outlet_delete(x->m_dump_out);
}

extern "C" void ossia_ossia_setup()
{
  t_class* c = class_new(
      "ossia", (method)ossia_object_new, (method)ossia_object_free,
      (short)sizeof(t_ossia), 0L, A_GIMME, 0);

  class_addmethod(c, (method)ossia_device_expose, "expose", A_GIMME, 0);
  class_addmethod(c, (method)ossia_device_name, "name", A_GIMME, 0);
  class_addmethod(c, (method)ossia::max::t_object_base::relative_namespace, "namespace", A_NOTHING, 0);

  class_register(CLASS_BOX, c);

  auto& ossia_library = ossia::max::ossia_max::instance();
  ossia_library.ossia_ossia_class = c;
}