// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia-max/src/object_base.hpp>
#include <ossia/network/osc/detail/osc.hpp>
#include <ossia/preset/preset.hpp>
#include <ossia/editor/dataspace/dataspace_visitors.hpp>
#include <ossia-max/src/ossia-max.hpp>
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
        [=] (const ossia::value& v) { enqueue_value(v); });

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
        [=] (const ossia::value& v) { enqueue_value(v); });

      set_parent_addr();
    }
  }

  return *this;
}

t_matcher::t_matcher(ossia::net::node_base* n, object_base* p) :
  node{n}, parent{p}, callbackit{ossia::none}
{
  if (auto param = node->get_parameter())
    callbackit = param->add_callback(
      [=](const ossia::value& v) { enqueue_value(v); });

  node->about_to_be_deleted.connect<object_base, &object_base::is_deleted>(
        parent);

  set_parent_addr();

  //clock_delay(x_regclock, 0);
}

t_matcher::~t_matcher()
{
  if(node && parent)
  {
    if (   parent->m_otype == object_class::param
        || parent->m_otype == object_class::model )
    {
      if (!parent->m_is_deleted)
      {
        auto param = node->get_parameter();
        if (param && callbackit) param->remove_callback(*callbackit);
        node->about_to_be_deleted.disconnect<object_base, &object_base::is_deleted>(parent);

        for (auto remote : ossia_max::instance().remotes.copy())
        {
          ossia::remove_one(remote->m_matchers,*this);
        }

        if (node->get_parent())
          node->get_parent()->remove_child(*node);
      }
      // if there vector is empty
      // remote should be quarantinized
      if (parent->m_matchers.size() == 0)
      {
        switch(parent->m_otype)
        {
          case object_class::model:
            object_quarantining<model>((model*) parent);
            break;
          case object_class::param:
            object_quarantining<parameter>((parameter*) parent);
            break;
          default:
            ;
        }
      }
    } else {

      if (!parent->m_is_deleted)
      {
        auto param = node->get_parameter();
        if (param && callbackit) param->remove_callback(*callbackit);
        node->about_to_be_deleted.disconnect<object_base, &object_base::is_deleted>(parent);
      }

      // if there vector is empty
      // remote should be quarantinized
      if (parent->m_matchers.size() == 0)
      {
        switch(parent->m_otype)
        {
          case object_class::remote:
            object_quarantining<remote>((remote*) parent);
            break;
          case object_class::view:
            object_quarantining<view>((view*) parent);
            break;
          default:
            ;
        }
      }
    }
    node = nullptr;
    parent = nullptr;
  }
}

void t_matcher::enqueue_value(const ossia::value& v)
{
  m_queue_list.enqueue(v);
}

void t_matcher::output_value()
{
  ossia::value v;
  while(m_queue_list.try_dequeue(v)) {

    outlet_anything(parent->m_dumpout,gensym("address"),1,&m_addr);

    auto param = node->get_parameter();

    auto filtered = ossia::net::filter_value(
          param->get_domain(),
          v,
          param->get_bounding());

    ossia::value converted;
    parameter_base* xparam = (parameter_base*)parent;
    if ( xparam->m_ounit != ossia::none )
      converted = ossia::convert(filtered, param->get_unit(), *xparam->m_ounit);
    else
      converted = filtered;

    value_visitor<object_base> vm;
    vm.x = (object_base*)parent;
    converted.apply(vm);
  }
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

void object_base::is_deleted(const ossia::net::node_base& n)
{
    m_is_deleted= true;
    ossia::remove_one_if(
      m_matchers,
      [&] (const auto& m) {
        return m.get_node() == &n;
    });
    m_is_deleted = false;
}

void object_base::set_priority()
{
  for (auto n : m_nodes)
    ossia::net::set_priority(*n, m_priority);
}

void object_base::set_description()
{
  if (m_description != gensym(""))
  {
    for (auto n : m_nodes)
      ossia::net::set_description(*n, m_description->s_name);
  }
}

void object_base::set_tags()
{
  std::vector<std::string> tags;
  for (int i = 0; i < m_tags_size; i++)
    tags.push_back(m_tags[i]->s_name);

  for (auto n : m_nodes)
    ossia::net::set_tags(*n, tags);
}

void object_base::set_hidden()
{
  for (t_matcher& m : m_matchers)
  {
    ossia::net::node_base* node = m.get_node();
    ossia::net::set_hidden(*node, m_hidden);
  }
}

void object_base::defer_set_output(object_base*x, t_symbol*s ,int argc, t_atom* argv){
  outlet_anything(x->m_set_out, s, argc, argv);
}

void object_base::update_attribute(object_base* x, ossia::string_view attribute)
{
  if ( attribute == ossia::net::text_priority() ){
    get_priority(x);
  } else if ( attribute == ossia::net::text_description() ){
    get_description(x);
  } else if ( attribute == ossia::net::text_tags() ){
    get_tags(x);
  } else if ( attribute == ossia::net::text_hidden() ){
    get_hidden(x);
  } else {
    object_error((t_object*)x, "no attribute %s", std::string(attribute).c_str());
  }
}

void object_base::get_tags(object_base*x)
{
  if (!x->m_matchers.empty())
  {
    ossia::net::node_base* node = x->m_matchers[0].get_node();
    auto optags = ossia::net::get_tags(*node);

    if (optags)
    {
      std::vector<std::string> tags = *optags;
      x->m_tags_size = tags.size() > OSSIA_MAX_MAX_ATTR_SIZE ? OSSIA_MAX_MAX_ATTR_SIZE : tags.size();
      for (int i=0; i < x->m_tags_size; i++)
        x->m_tags[i] = gensym(tags[i].c_str());

      //outlet_anything(x->m_dumpout, gensym("tags"),
      //                x->m_tags_size, x->m_tags);
    }
  }
}

void object_base::get_description(object_base*x)
{
  if (!x->m_matchers.empty())
  {
    ossia::net::node_base* node = x->m_matchers[0].get_node();
    auto description = ossia::net::get_description(*node);

    if (description)
    {
      std::string str = *description;
      x->m_description = gensym(str.c_str());

      // outlet_anything(x->m_dumpout, gensym("description"),
      //                x->m_description_size, x->m_description);
    }
  }
}

void object_base::get_priority(object_base*x)
{
  // assume all matchers have the same priority
  ossia::max::t_matcher& m = x->m_matchers[0];
  ossia::net::node_base* node = m.get_node();

  auto priority = ossia::net::get_priority(*node);
  if (priority)
  {
    x->m_priority = *priority;
    /*
    t_atom a;
    A_SETFLOAT(&a, x->m_priority);
    outlet_anything(x->m_dumpout, gensym("priority"), 1, &a);
    */
  }
}

void object_base::get_hidden(object_base*x)
{
  // assume all matchers have the same bounding_mode
  ossia::max::t_matcher& m = x->m_matchers[0];
  ossia::net::node_base* node = m.get_node();

  x->m_hidden = ossia::net::get_hidden(*node);

  // TODO notify object
}

void object_base::class_setup(t_class*c)
{
  CLASS_ATTR_LONG(c, "priority", 0, object_base, m_priority);

  CLASS_ATTR_SYM(c, "description", 0, object_base, m_description);

  CLASS_ATTR_SYM_VARSIZE(c, "tags", 0, object_base, m_tags, m_tags_size, OSSIA_MAX_MAX_ATTR_SIZE);

  CLASS_ATTR_LONG( c, "hidden", 0, object_base, m_hidden);
  CLASS_ATTR_STYLE(c, "hidden", 0, "onoff");

  class_addmethod(c, (method) object_base::get_address, "getaddress", A_NOTHING,  0);
}

void object_base::get_address(object_base *x)
{
  if (!x->m_matchers.empty())
  {
    t_symbol* sym_address = gensym("global_address");
    for (auto& m : x->m_matchers)
    {
      std::string addr = ossia::net::address_string_from_node(*m.get_node());
      t_atom a;
      A_SETSYM(&a, gensym(addr.c_str()));
      outlet_anything(x->m_dumpout, sym_address, 1, &a);
    }
  }
  else if (!x->m_nodes.empty())
  {
    for (auto n : x->m_nodes)
    {
      std::string addr = ossia::net::address_string_from_node(*n);
      t_atom a;
      A_SETSYM(&a, gensym(addr.c_str()));
      outlet_anything(x->m_dumpout, gensym("address"), 1, &a);
    }
  }
  else
    outlet_anything(x->m_dumpout, gensym("address"), 0, NULL);
}

} // max namespace
} // ossia namespace
