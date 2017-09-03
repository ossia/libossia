#include "node_base.hpp"
#include <ossia/preset/preset.hpp>
#include <ossia/network/generic/generic_device.hpp>

namespace ossia
{
namespace pd
{

node_base::node_base(t_eclass* x)
  : object_base{x}
{ }


void node_base::preset(object_base *x, t_symbol*s, int argc, t_atom* argv)
{
  ossia::net::node_base* node{};
  switch (x->m_otype)
  {
    case object_class::client:
    case object_class::device:
      node = &x->m_device->get_root_node();
      break;
    case object_class::model:
    case object_class::view:
      // TODO oups how to get that ?
      node = x->m_nodes[0];
      break;
    default:
      node = nullptr;
  }

  t_atom status[3];
  status[0] = argv[0];

  if ( argc < 2
       || argv[0].a_type != A_SYMBOL
       || argv[1].a_type != A_SYMBOL )
  {
    pd_error(x, "Wrong argument number to 'preset' message"
                "needs 2 symbol arguments: <load|save> <filename>");
    return;
  }

  if (node)
  {
    if ( argv[0].a_w.w_symbol == gensym("save") )
    {
      argc--;
      argv++;

      SETFLOAT(status+1, 0);
      status[2] = argv[0];

      try {

        auto preset = ossia::presets::make_preset(*node);
        auto json = ossia::presets::write_json(x->m_name->s_name, preset);
        ossia::presets::write_file(json, argv[0].a_w.w_symbol->s_name);
        SETFLOAT(status+1, 1);

      } catch (std::ifstream::failure e) {
        pd_error(x,"Can't open file %s, error: %s", argv[0].a_w.w_symbol->s_name, e.what());
      }

      outlet_anything(x->m_dumpout, gensym("preset"),3, status);

    }
    else if ( argv[0].a_w.w_symbol == gensym("load") )
    {
      argc--;
      argv++;

      SETFLOAT(status+1, 0);
      status[2] = argv[0];
      try {

        auto json = ossia::presets::read_file(argv[0].a_w.w_symbol->s_name);
        auto preset = ossia::presets::read_json(json);
        ossia::presets::apply_preset(*node, preset);
        SETFLOAT(status+1, 1);

      } catch (std::ifstream::failure e) {

        pd_error(x,"Can't write file %s, error: %s", argv[0].a_w.w_symbol->s_name, e.what());

      }

      outlet_anything(x->m_dumpout, gensym("preset"),3, status);

    }
    else
      pd_error(x, "unknown preset command '%s'", argv[0].a_w.w_symbol->s_name);

  }
  else
  {
    pd_error(x, "No node to save or to load into.");
  }
}

} // namespace pd
} // namespace ossia
