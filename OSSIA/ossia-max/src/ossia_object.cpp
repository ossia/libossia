#include "ossia-max.hpp"
#include "device.hpp"

namespace ossia
{
namespace max
{

using t_ossia = t_device;

extern "C" void* ossia_object_new(t_symbol* name, int argc, t_atom* argv)
{
  auto& ossia_library = ossia_max::instance();
  t_ossia* x = (t_ossia*) object_alloc(ossia_library.ossia_ossia_class);

  post("new method");

  x->m_dump_out
      = outlet_new(x, NULL); // anything outlet to dump device state
  x->m_device = ossia_library.get_default_device();

  return (x);
}

extern "C" void ossia_object_free(t_ossia *x)
{
  outlet_delete(x->m_dump_out);
}

extern "C" void ossia_ossia_setup(void)
{
  t_class* c = class_new(
      "ossia", (method)ossia_object_new, (method)ossia_object_free,
      (short)sizeof(t_ossia), 0L, A_GIMME, 0);

  post("setup method");

  // FIXME this doesn't seem to work
  class_alias(c, gensym("ossia-papa"));

  class_addmethod(c, (method)ossia_device_expose, "expose", A_GIMME, 0);
  //class_addmethod(c, (method)ossia_name, "name", A_GIMME, 0);
  //class_addmethod(c, (method)ossia_get_namespace, "namespace", A_GIMME, 0);

  class_register(CLASS_BOX, c);

  auto& ossia_library = ossia_max::instance();
  ossia_library.ossia_ossia_class = c;
}

} // namespace max
} // mnamespace ossia
