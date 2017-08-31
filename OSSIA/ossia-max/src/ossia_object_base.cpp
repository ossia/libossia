// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "ossia_object_base.hpp"
#include <ossia/network/osc/detail/osc.hpp>
#include <ossia/preset/preset.hpp>
#include <ossia/editor/dataspace/dataspace_visitors.hpp>
#include <regex>

namespace ossia
{
namespace max
{

#pragma mark t_matcher

t_matcher::t_matcher(t_matcher&& other)
{
  node = other.node;
  other.node = nullptr;

  parent = other.parent;
  other.parent = nullptr;

  callbackit = other.callbackit;
  other.callbackit = ossia::none;

  if(node)
  {
    if(auto param = node->get_parameter())
    {
      if (callbackit)
        param->remove_callback(*callbackit);

      callbackit = param->add_callback(
        [=] (const ossia::value& v) { set_value(v); });

      set_parent_addr();
    }
  }
}

t_matcher& t_matcher::operator=(t_matcher&& other)
{
  node = other.node;
  other.node = nullptr;

  parent = other.parent;
  other.parent = nullptr;

  callbackit = other.callbackit;
  other.callbackit = ossia::none;

  if(node)
  {
    if(auto param = node->get_parameter())
    {
      if (callbackit)
        param->remove_callback(*callbackit);

      callbackit = param->add_callback(
        [=] (const ossia::value& v) { set_value(v); });

      set_parent_addr();
    }
  }

  return *this;
}

t_matcher::t_matcher(ossia::net::node_base* n, t_object_base* p) :
  node{n}, parent{p}, callbackit{ossia::none}
{
  callbackit = node->get_parameter()->add_callback(
      [=](const ossia::value& v) { set_value(v); });

  node->about_to_be_deleted.connect<t_object_base, &t_object_base::is_deleted>(
        parent);

  set_parent_addr();

  //clock_delay(x_regclock, 0);
}

t_matcher::~t_matcher()
{
  if(node)
  {
    if (parent->m_otype == object_class::param)
    {
      if (!parent->m_is_deleted)
      {
        if (node->get_parent())
          node->get_parent()->remove_child(*node);
      }
      // if there vector is empty
      // remote should be quarantinized
      if (parent->m_matchers.size() == 0)
      {
        object_quarantining<t_parameter>((t_parameter*) parent);
      }
    } else {
      auto param = node->get_parameter();
      if (param && callbackit) param->remove_callback(*callbackit);
      node->about_to_be_deleted.disconnect<t_object_base, &t_object_base::is_deleted>(parent);

      // if there vector is empty
      // remote should be quarantinized
      if (parent->m_matchers.size() == 0)
      {
        object_quarantining<t_remote>((t_remote*) parent);
      }
    }
  }
  node = nullptr;
}

void t_matcher::set_value(const ossia::value& v)
{
  outlet_anything(parent->m_dumpout,gensym("address"),1,&m_addr);

  auto param = node->get_parameter();

  auto filtered = ossia::net::filter_value(
        param->get_domain(),
        v,
        param->get_bounding());

  ossia::value converted;
  if ( parent->m_ounit != ossia::none )
    converted = ossia::convert(filtered, param->get_unit(), *parent->m_ounit);
  else
    converted = filtered;

  value_visitor<t_object_base> vm;
  vm.x = (t_object_base*)parent;
  converted.apply(vm);
}

void t_matcher::set_parent_addr()
{
  if (parent->m_parent_node){
    std::string addr = ossia::net::address_string_from_node(*node);
    std::string parent_addr = ossia::net::address_string_from_node(*parent->m_parent_node);
    if ( parent_addr.back() != '/' ) parent_addr += "/";

    std::regex addr_regex(parent_addr);
    std::smatch addr_match;

    if (std::regex_search(addr, addr_match, addr_regex))
    {
      A_SETSYM(&m_addr, gensym(addr_match.suffix().str().c_str()));
    } else {
      A_SETSYM(&m_addr, gensym(node->get_name().c_str()));
    }
  }
  else
  {
    A_SETSYM(&m_addr, gensym("."));
  }
}

void t_object_base::is_deleted(const ossia::net::node_base& n)
{
  if (!m_dead)
  {
    m_is_deleted= true;
    ossia::remove_one_if(
      m_matchers,
      [&] (const auto& m) {
        return m.get_node() == &n;
    });
    m_is_deleted = false;
  }
}

void t_object_base::push(t_object_base* x, t_symbol*s, int argc, t_atom* argv)
{
  for (auto& m : x->m_matchers)
  {
    x->m_node = m.get_node();
    auto parent = m.get_parent();
    auto param = x->m_node->get_parameter();

    if (x->m_node && param)
    {
      if (argc == 1)
      {
        // convert one element array to single element
        switch (argv->a_type )
        {
          case A_SYM:
            x->m_node->get_parameter()->push_value(
                  std::string(atom_getsym(argv)->s_name));
            break;
          case A_LONG:
            x->m_node->get_parameter()->push_value((int32_t)atom_getlong(argv));
            break;
          case A_FLOAT:
            x->m_node->get_parameter()->push_value(atom_getfloat(argv));
            break;
          default:
            object_error((t_object*)x, "value type not handled");
        }
      }
      else
      {
        std::vector<ossia::value> list;

        if (s && s != gensym("list") )
          list.push_back(std::string(s->s_name));

        for (; argc > 0; argc--, argv++)
        {
          switch(argv->a_type)
          {
            case A_SYM:
              list.push_back(std::string(atom_getsym(argv)->s_name));
              break;
            case A_LONG:
              list.push_back((int32_t)atom_getlong(argv));
              break;
            case A_FLOAT:
              list.push_back(atom_getfloat(argv));
              break;
            default:
              object_error((t_object*)x, "value type not handled");
          }
        }

        x->m_node->get_parameter()->push_value(list);
      }
    }
  }
}

void t_object_base::bang(t_object_base* x)
{
  for (auto& matcher : x->m_matchers)
  {
    matcher.set_value(matcher.get_node()->get_parameter()->value());
  }
}

void t_object_base::defer_set_output(t_object_base*x, t_symbol*s ,int argc, t_atom* argv){
  outlet_anything(x->m_set_out, s, argc, argv);
}

void list_all_child(const ossia::net::node_base& node, std::vector<std::string>& list){
  for (const auto& child : node.children_copy())
  {
    if (auto addr = child->get_parameter())
    {
      std::string s = ossia::net::osc_parameter_string(*child);
      list.push_back(s);
    }
    list_all_child(*child,list);
  }
}

void list_all_child(const ossia::net::node_base& node, std::vector<ossia::net::node_base*>& list){
  for (const auto& child : node.children_copy())
  {
    list.push_back(child);
    list_all_child(*child,list);
  }
}

void t_object_base::getnamespace(t_object_base* x)
{
  t_symbol* prependsym = gensym("namespace");
  std::vector<ossia::net::node_base*> list;
  list_all_child(*x->m_node, list);
  int pos = ossia::net::osc_parameter_string(*x->m_node).length();
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

void t_object_base::preset(t_object_base *x, t_symbol*s, int argc, t_atom* argv)
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
      node = x->m_node;
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

      try {

        auto preset = ossia::presets::make_preset(*node);
        auto json = ossia::presets::write_json(x->m_name->s_name, preset);
        ossia::presets::write_file(json, argv[0].a_w.w_sym->s_name);
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
        auto preset = ossia::presets::read_json(json);
        ossia::presets::apply_preset(*node, preset);
        A_SETFLOAT(status+1, 1);

      } catch (std::ifstream::failure e) {

        object_error((t_object*)x,"Can't write file %s, error: %s", argv[0].a_w.w_sym->s_name, e.what());

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


} // max namespace
} // ossia namespace
