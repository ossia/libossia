#include "ossia_object.hpp"

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
  // ugly hack while waiting for C++ Max API
  auto place = object_alloc(ossia_library.ossia_ossia_class);
  t_object tmp;
  memcpy(&tmp, place, sizeof(t_object));
  ossia_object* x = new(place) ossia_object();
  memcpy(x, &tmp, sizeof(t_object));

  x->m_dumpout
      = outlet_new(x, NULL); // anything outlet to dump device state
  x->m_device = ossia_library.get_default_device();
  x->m_nodes = {&x->m_device->get_root_node()};

  return (x);
}

void ossia_object::destroy(ossia_object *x)
{
  outlet_delete(x->m_dumpout);
  x->~ossia_object();
}

} // namespace max
} // namespace ossia
