#include <ossia-max/src/node_base.hpp>
#include <ossia/preset/preset.hpp>
#include <ossia-max/src/utils.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/oscquery/detail/value_to_json.hpp>

#include <boost/algorithm/string/case_conv.hpp>

#include <rapidjson/allocators.h>
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>

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
      assert(!x->m_matchers.empty());
      node = x->m_matchers[0]->get_node();
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

      try
      {
        if (make_kiss)
        {
          auto preset = ossia::presets::make_preset(*node);
          auto kiss = ossia::presets::to_string(preset);
          ossia::presets::write_file(kiss, filename);
        } else {

          auto json = ossia::presets::make_json_preset(*node);
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

        if (!ossia::presets::apply_json(json, *node, trig_output_value))
        {
          ossia::presets::apply_preset(json, *node, trig_output_value);
        }

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

void node_base::get_namespace(node_base* x)
{
  t_symbol* prependsym = gensym("namespace");
  std::vector<ossia::net::node_base*> list;
  for (auto& m : x->m_matchers)
  {
    auto n = m->get_node();
    list = ossia::net::list_all_child(n);

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
    auto n = m->get_node();
    list = ossia::net::list_all_child(n);

    for (ossia::net::node_base* child : list)
    {
      if (auto param = child->get_parameter())
      {
        auto val = ossia::net::get_default_value(*child);
        if(val)
        {
          param->push_value(*val);
          trig_output_value(child);
        }
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
  class_addmethod(c, (method) node_base::push_default_value, "reset", A_NOTHING, 0);
}

void node_base::set(node_base* x, t_symbol* s, int argc, t_atom* argv)
{
  if (argc > 0 && argv[0].a_type == A_SYM)
  {
    std::string addr = argv[0].a_w.w_sym->s_name;
    argv++;
    argc--;
    auto v = atom2value(nullptr,argc,argv);
    for (auto& m : x->m_matchers)
    {
      auto nodes = ossia::net::find_nodes(*m->get_node(), addr);
      for (auto& node : nodes)
      {
        if (auto param = node->get_parameter())
        {

          param->push_value(v);
          trig_output_value(node);
        }
      }
    }
  }
}


} // namespace max
} // namespace ossia
