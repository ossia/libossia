#include <ossia-max/src/node_base.hpp>
#include <ossia/preset/preset.hpp>
#include <ossia-max/src/utils.hpp>
#include <ossia/network/generic/generic_device.hpp>

namespace ossia
{
namespace max
{

void node_base::preset(node_base *x, t_symbol*s, long argc, t_atom* argv)
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
       || argv[0].a_type != A_SYM
       || argv[1].a_type != A_SYM )
  {
    object_error((t_object*)x, "Wrong argument number to 'preset' message"
                "needs 2 symbol arguments: <load|save> <filename>");
    return;
  }

  if (node)
  {
    if ( argv[0].a_w.w_sym == gensym("save") )
    {
      argc--;
      argv++;

      A_SETFLOAT(status+1, 0);
      status[2] = argv[0];

      std::string filename = argv[0].a_w.w_sym->s_name;

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
        A_SETFLOAT(status+1, 1);

      } catch (std::ifstream::failure e) {
        object_error((t_object*)x,"Can't open file %s, error: %s", argv[0].a_w.w_sym->s_name, e.what());
      }

      outlet_anything(x->m_dumpout, gensym("preset"),3, status);

    }
    else if ( argv[0].a_w.w_sym == gensym("load") )
    {
      argc--;
      argv++;

      A_SETFLOAT(status+1, 0);
      status[2] = argv[0];
      try {

        auto json = ossia::presets::read_file(argv[0].a_w.w_sym->s_name);
        ossia::presets::preset preset;
        try {
          preset = ossia::presets::read_json(json);
        } catch (...) {
          preset = ossia::presets::from_string(json);
        }
        ossia::presets::apply_preset(*node, preset,  ossia::presets::keep_arch_on, {}, true);
        A_SETFLOAT(status+1, 1);

      } catch (std::ifstream::failure e) {
        object_error((t_object*)x,"Can't open file %s, error: %s", argv[0].a_w.w_sym->s_name, e.what());
      } catch (const std::exception& e) {
        object_error((t_object*)x,"Can't apply preset to current device:  %s", e.what());
      } catch (...) {
        object_error((t_object*)x,"Can't apply preset to current device...");
      }

      outlet_anything(x->m_dumpout, gensym("preset"),3, status);
    }
    else
      object_error((t_object*)x, "unknown preset command '%s'", argv[0].a_w.w_sym->s_name);

  }
  else
  {
    object_error((t_object*)x, "No node to save or to load into.");
  }
}

void list_all_child(const ossia::net::node_base& node, std::vector<std::string>& list){

  const auto children = node.children_copy();
  list.reserve(list.size() + children.size());
  for (const auto& child : children)
  {
    if (auto addr = child->get_parameter())
    {
      list.push_back(ossia::net::osc_parameter_string(*child));
    }
    list_all_child(*child,list);
  }
}

void list_all_child(const ossia::net::node_base& node, std::vector<ossia::net::node_base*>& list){

  const auto children = node.children_copy();
  list.reserve(list.size() + children.size());
  for (const auto& child : children)
  {
    list.push_back(child);
    list_all_child(*child,list);
  }
}

void node_base::get_namespace(node_base* x)
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

void node_base::class_setup(t_class* c)
{
  object_base::class_setup(c);
  class_addmethod(c, (method) node_base::set,           "set",       A_GIMME, 0);
  class_addmethod(c, (method) node_base::get_namespace, "namespace", A_NOTHING,  0);
  class_addmethod(c, (method) node_base::preset,        "preset",    A_GIMME, 0);
}

void node_base::set(node_base* x, t_symbol* s, int argc, t_atom* argv)
{
  if (argc > 0 && argv[0].a_type == A_SYM)
  {
    std::string addr = argv[0].a_w.w_sym->s_name;
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


} // namespace max
} // namespace ossia
