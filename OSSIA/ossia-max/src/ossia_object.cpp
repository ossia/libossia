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
