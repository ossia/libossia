#include "ossia-max.hpp"
#include "device.hpp"

using t_ossia = ossia::max::device;

namespace ossia
{
namespace max
{


extern "C" void* ossia_object_new(t_symbol* name, int argc, t_atom* argv)
{
  auto& ossia_library = ossia::max::ossia_max::instance();
  t_ossia* x = (t_ossia*) object_alloc(ossia_library.ossia_ossia_class);

  x->m_dumpout
      = outlet_new(x, NULL); // anything outlet to dump device state
  x->m_device = ossia_library.get_default_device();
  x->m_nodes = {&ossia_library.get_default_device()->get_root_node()};

  return (x);
}

extern "C" void ossia_object_free(t_ossia *x)
{
  outlet_delete(x->m_dumpout);
}

extern "C" void ossia_ossia_setup()
{
  t_class* c = class_new(
      "ossia", (method)ossia_object_new, (method)ossia_object_free,
      (short)sizeof(t_ossia), 0L, A_GIMME, 0);

  class_addmethod(c, (method)device::expose, "expose", A_GIMME, 0);
  class_addmethod(c, (method)device::name, "name", A_GIMME, 0);

  class_register(CLASS_BOX, c);

  auto& ossia_library = ossia::max::ossia_max::instance();
  ossia_library.ossia_ossia_class = c;
}


} // namespace max
} // namespace ossia
