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

static std::vector<ossia::net::node_base*> list_all_child(ossia::net::node_base* node);

static ossia::presets::preset make_preset(ossia::net::node_base* node)
{
  ossia::presets::preset cue;
  auto nodes = list_all_child(node);
  for (auto n : nodes)
  {
    if (auto param = n->get_parameter())
    {
      std::string key = n->get_name();
      auto n1 = n->get_parent();
      while ( n1 != node )
      {
        key = n1->get_name() + key;
        n1 = n1->get_parent();
      }
      cue.push_back({key,param->value()});
    }
  }

  return cue;
}

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
      if(!x->m_matchers.empty())
      {
        // TODO oups how to get that ?
        node = x->m_matchers[0].get_node();
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
        auto preset = make_preset(node);
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

        for (auto& p : preset)
        {
          if(auto n = ossia::net::find_node(*node, p.first))
          {
            if (auto param = n->get_parameter())
            {
              param->push_value(p.second);
              trig_output_value(n);
            }
          }
        }
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
static std::vector<ossia::net::node_base*> list_all_child(ossia::net::node_base* node)
{
  std::vector<ossia::net::node_base*> children
      = node->children_copy();
  std::vector<ossia::net::node_base*> list;

  ossia::sort(children, [](auto n1, auto n2)
    { return ossia::net::get_priority(*n1) > ossia::net::get_priority(*n2); });

  for (auto it = children.begin(); it != children.end(); it++ )
  {
    list.push_back(*it);
    auto nested_list = list_all_child(*it);
    list.insert(list.end(), nested_list.begin(), nested_list.end());
  }

  return list;
}

void ossia::pd::node_base::get_namespace(object_base* x)
{
  t_symbol* prependsym = gensym("namespace");
  std::vector<ossia::net::node_base*> list;
  for (auto& m : x->m_matchers)
  {
    auto n = m.get_node();
    list = list_all_child(n);

    int pos = ossia::net::osc_parameter_string(*n).length();
    for (ossia::net::node_base* child : list)
    {
      if (child->get_parameter())
      {
        ossia::value name = ossia::net::osc_parameter_string(*child).substr(pos);
        ossia::value val = child->get_parameter()->value();

        std::vector<t_atom> va;
        value2atom vm{va};

        name.apply(vm);
        val.apply(vm);

        outlet_anything(x->m_dumpout, prependsym, va.size(), va.data());
      }
    }
  }
}

void node_base::push_default_value(node_base* x)
{
  std::vector<ossia::net::node_base*> list;
  for (auto& m : x->m_matchers)
  {
    auto n = m.get_node();
    list = list_all_child(n);

    for (ossia::net::node_base* child : list)
    {
      if (auto param = child->get_parameter())
      {
        auto val = ossia::net::get_default_value(*child);
        if(val)
          param->push_value(*val);
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
  eclass_addmethod(c, (method) node_base::push_default_value, "reset", A_NULL, 0);
}

void node_base::set(node_base* x, t_symbol* s, int argc, t_atom* argv)
{
  if (argc > 0 && argv[0].a_type == A_SYMBOL)
  {
    std::string addr = argv[0].a_w.w_symbol->s_name;
    argv++;
    argc--;
    auto v = atom2value(nullptr,argc,argv);
    for (auto& m : x->m_matchers)
    {
      auto nodes = ossia::net::find_nodes(*m.get_node(), addr);
      for (auto& node : nodes)
      {
        // TODO add a m_children member to node_base that list all registered t_matchers
        if (auto param = node->get_parameter())
        {
          param->push_value(v);
          trig_output_value(node);
        }
      }
    }
  }
}

} // namespace pd
} // namespace ossia
