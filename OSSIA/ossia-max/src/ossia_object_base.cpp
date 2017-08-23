// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "ossia_object_base.hpp"
#include <ossia/network/osc/detail/osc.hpp>

namespace ossia
{
namespace max
{

void t_object_base::apply_value_visitor(const ossia::value& v)
{
  auto local_param = m_node->get_parameter();
  auto filtered = ossia::net::filter_value(
        local_param->get_domain(),
        v,
        local_param->get_bounding());
  value_visitor<t_object_base> vm;
  vm.x = this;
  filtered.apply(vm);
}

void t_object_base::push(t_object_base* x, t_symbol*, int argc, t_atom* argv)
{
  if (x->m_node && x->m_node->get_parameter())
  {
    if (argc == 1)
    {
      // convert one element array to single element
      if (argv->a_type == A_SYM)
        x->m_node->get_parameter()->push_value(
            std::string(atom_getsym(argv)->s_name));

      else if (argv->a_type == A_LONG)
        x->m_node->get_parameter()->push_value((int32_t)atom_getlong(argv));

      else if (argv->a_type == A_FLOAT)
        x->m_node->get_parameter()->push_value(atom_getfloat(argv));
    }
    else
    {
      std::vector<ossia::value> list;

      for (; argc > 0; argc--, argv++)
      {
        if (argv->a_type == A_SYM)
          list.push_back(std::string(atom_getsym(argv)->s_name));
        else if (argv->a_type == A_LONG)
          list.push_back((int32_t)atom_getlong(argv));
        else if (argv->a_type == A_FLOAT)
          list.push_back(atom_getfloat(argv));
        else
          object_error((t_object*)x, "value type not handled");
      }

      x->m_node->get_parameter()->push_value(list);
    }
  }
}

void t_object_base::bang(t_object_base* x)
{
  if (x->m_node && x->m_node->get_parameter())
    x->apply_value_visitor(x->m_node->get_parameter()->value());
}
/*
    void t_object_base :: tick(t_object_base* x)
    {
        if (x->m_last_opened_canvas)
        {
            glist_noselect(x->m_last_opened_canvas);
            x->m_last_opened_canvas = nullptr;
        }
    }
*/

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

void t_object_base::relative_namespace(t_object_base* x)
{
  if (x->m_node == nullptr) return;
  t_symbol* prependsym = gensym("namespace");
  std::vector<std::string> list;
  list_all_child(*x->m_node, list);
  int pos = ossia::net::osc_parameter_string(*x->m_node).length();
  for (auto& addr : list)
  {
    std::string s = addr.substr(pos);
    t_atom a;
    atom_setsym(&a,gensym(s.c_str()));
    outlet_anything(x->m_dump_out, prependsym,1,&a);
  }
}

} // max namespace
} // ossia namespace
