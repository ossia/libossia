#include <ossia-pd/src/node_base.hpp>
#include <ossia/preset/preset.hpp>
#include <ossia-pd/src/utils.hpp>
#include <ossia/network/generic/generic_device.hpp>

namespace ossia
{
namespace pd
{

node_base::node_base(t_eclass* x)
  : object_base{x}
{ }


void node_base::preset(node_base *x, t_symbol*s, int argc, t_atom* argv)
{
  ossia::net::node_base* node{};
  switch (x->m_otype)
  {
    case object_class::client:
    case object_class::device:
      if(x->m_device)
        node = &x->m_device->get_root_node();
      break;
    case object_class::model:
    case object_class::view:
    {
      if(!x->m_nodes.empty())
      {
        // TODO oups how to get that ?
        node = x->m_nodes[0];
      } else
        return;
      break;
    }
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

      std::string filename = argv[0].a_w.w_symbol->s_name;

      bool make_kiss = filename.size() >= 4 &&
                 filename.compare(filename.size() - 4, 4, ".txt") == 0;

      try {
        auto preset = ossia::presets::make_preset(*node);
        if (make_kiss)
        {
          auto kiss = ossia::presets::to_string(preset);
          ossia::presets::write_file(kiss, filename);
        } else {
          auto json = ossia::presets::write_json(x->m_name->s_name, preset);
          ossia::presets::write_file(json, filename);
        }
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
        ossia::presets::preset preset;
        try {
          preset = ossia::presets::read_json(json);
        } catch (...) {
          preset = ossia::presets::from_string(json);
        }
        ossia::presets::apply_preset(*node, preset,  ossia::presets::keep_arch_on, {}, true);
        SETFLOAT(status+1, 1);

      } catch (std::ifstream::failure e) {
        pd_error(x,"Can't read file %s, error: %s", argv[0].a_w.w_symbol->s_name, e.what());
      } catch (...) {
        pd_error(x,"Can't apply preset to current device...");
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

/**
 * @brief list_all_child : list all node childs recursively
 * @param node : starting point
 * @param list : reference to a node_base vector to store each node
 */
static void list_all_child(const ossia::net::node_base& node, std::vector<ossia::net::node_base*>& list){
  for (const auto& child : node.children_copy())
  {
    list.push_back(child);
    list_all_child(*child,list);
  }
}

void ossia::pd::node_base::get_namespace(object_base* x)
{
  t_symbol* prependsym = gensym("namespace");
  std::vector<ossia::net::node_base*> list;
  for (auto n : x->m_nodes)
  {
    list_all_child(*n, list);
    int pos = ossia::net::osc_parameter_string(*n).length();
    for (ossia::net::node_base* child : list)
    {
      if (child->get_parameter())
      {
        ossia::value name = ossia::net::osc_parameter_string(*child).substr(pos);
        ossia::value val = child->get_parameter()->fetch_value();

        std::vector<t_atom> va;
        value2atom vm{va};

        name.apply(vm);
        val.apply(vm);

        outlet_anything(x->m_dumpout, prependsym, va.size(), va.data());
      }
    }
  }
}

void node_base :: class_setup(t_eclass* c)
{
  object_base::class_setup(c);
  eclass_addmethod(c, (method) node_base::set,           "set",       A_GIMME, 0);
  eclass_addmethod(c, (method) node_base::get_namespace, "namespace", A_NULL,  0);
  eclass_addmethod(c, (method) node_base::preset,        "preset",    A_GIMME, 0);
}

void node_base::set(node_base* x, t_symbol* s, int argc, t_atom* argv)
{
  if (argc > 0 && argv[0].a_type == A_SYMBOL)
  {
    std::string addr = argv[0].a_w.w_symbol->s_name;
    argv++;
    argc--;
    auto v = atom2value(nullptr,argc,argv);
    for (auto n : x->m_nodes)
    {
      auto nodes = ossia::net::find_nodes(*n, addr);
      for (auto& node : nodes)
      {
        if (auto param = node->get_parameter()){
          param->push_value(v);
        }
      }
    }
  }
}

} // namespace pd
} // namespace ossia
